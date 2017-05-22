"""
    This module contains captcha generator.
"""

import os
import h5py
import multiprocessing
import threading
import numpy as np
import tensorflow as tf


class CaptchaDataManager(object):
    """
        Class for captcha data managment.
    """
    def __init__(self, batch_size, captcha_size,
                 hdf5_files_dir, coordinator, data_format, queue_size=32):
        self.batch_size = batch_size
        self.data_format = data_format
        self.coordinator = coordinator

        self.filenames = []
        self.data_size = 0
        for filename in os.listdir(hdf5_files_dir):
            if filename.endswith(".hdf5"):
                self.filenames.append(os.path.join(hdf5_files_dir, filename))
                cur_hdf5 = h5py.File(self.filenames[-1], "r")
                self.data_size += cur_hdf5["data"].maxshape[0]
                cur_hdf5.close()
        self.i = 0
        self.file_ind = 0

        self.cur_hdf5 = h5py.File(self.filenames[self.file_ind], "r")
        self.images = self.cur_hdf5["data"]
        self.labels = self.cur_hdf5["labels"]
        self.samples_count = self.images.maxshape[0]
        self.im_width = self.images.maxshape[2]
        self.im_height = self.images.maxshape[1]
        self.lock = threading.Lock()
        self.batches_count = self.samples_count / self.batch_size

        # Init queue parameters
        self.images_pl = tf.placeholder(tf.float32, [
            batch_size, self.im_width, self.im_height, 3])
        self.labels_pl = tf.placeholder(tf.int32, [batch_size, captcha_size])
        if self.data_format == "NCHW":
            self.images_pl = tf.transpose(self.images_pl, [0, 3, 1, 2])
        self.queue = tf.FIFOQueue(queue_size,
                                  [self.images_pl.dtype, self.labels_pl.dtype],
                                  [self.images_pl.get_shape(),
                                   self.labels_pl.get_shape()])
        self.threads = []
        self.enqueue_op = self.queue.enqueue([self.images_pl, self.labels_pl])

    def next_batch(self):
        """
            Return next batch. Cyclic.
        """
        with self.lock:
            selection = np.s_[self.i * self.batch_size:
                              (self.i + 1) * self.batch_size]
            if self.i + 1 < self.batches_count:
                self.i = self.i + 1
            else:
                self.i = 0
                self.file_ind = (self.file_ind + 1) % len(self.filenames)
                self.cur_hdf5.close()
                self.cur_hdf5 = h5py.File(self.filenames[self.file_ind], "r")
                self.images = self.cur_hdf5["data"]
                self.labels = self.cur_hdf5["labels"]
                self.samples_count = self.images.maxshape[0]
                self.im_width = self.images.maxshape[2]
                self.im_height = self.images.maxshape[1]

            #
            #
            images_batch = self.images[selection]
            labels_batch = self.labels[selection]

        images_batch = images_batch.astype(np.float32)
        images_batch = (images_batch - np.mean(images_batch, axis=(1, 2, 3),
                        keepdims=True)) / 255.0
        labels_batch = labels_batch.astype(np.int32)

        if self.data_format == "NCHW":
            images_batch = np.transpose(images_batch, axes=[0, 3, 1, 2])
        return images_batch, labels_batch

    def size(self):
        return self.queue.size()

    def dequeue(self):
        output = self.queue.dequeue()
        return output

    def thread_main(self, session):
        while not self.coordinator.should_stop():
            images, labels = self.next_batch()

            try:
                session.run(self.enqueue_op,
                            feed_dict={self.images_pl: images,
                                       self.labels_pl: labels})
            except tf.errors.CancelledError:
                return

    def start_threads(self, session, n_threads=multiprocessing.cpu_count()):
        for _ in range(n_threads):
            thread = threading.Thread(target=self.thread_main, args=(session,))
            thread.daemon = True  # Thread will close when parent quits.
            thread.start()
            self.threads.append(thread)
        return self.threads

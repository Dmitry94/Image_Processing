"""
    This module contains captcha generator.
"""

import os
import random
import collections
import multiprocessing
import cv2
import threading
import numpy as np
import tensorflow as tf

from captcha.image import ImageCaptcha


CaptchaSample = collections.namedtuple('CaptchaSample', ['captcha', 'text'])


class CaptchaGenerator(object):
    """
        Class that incapsulates captcha generation.
    """
    def __init__(self, coord, fonts_dir, alphabet, captcha_size, data_format,
                 batch_size=128, width=160, height=60, queue_size=32):
        fonts = []
        for file in os.listdir("fonts"):
            if file.endswith(".ttf"):
                fonts.append(os.path.join(fonts_dir, file))

        self.width = width
        self.height = height
        self.batch_size = batch_size
        self.data_format = data_format
        self.image_captcha_gen = ImageCaptcha(fonts=fonts,
                                              height=height, width=width)
        self.alphabet = alphabet
        self.captcha_size = captcha_size

        self.images_pl = tf.placeholder(
            tf.float32, [batch_size, height, width, 3])
        self.labels_pl = tf.placeholder(
            tf.int32, [batch_size, captcha_size])
        if self.data_format == "NCHW":
            self.images_pl = tf.transpose(self.images_pl, [0, 3, 1, 2])
        self.queue = tf.FIFOQueue(
            queue_size, [self.images_pl.dtype, self.labels_pl.dtype],
            [self.images_pl.get_shape(), self.labels_pl.get_shape()])
        self.threads = []
        self.coord = coord
        self.enqueue_op = self.queue.enqueue([self.images_pl, self.labels_pl])

    def get_batch(self):
        """
            Generates batch of samples.
        """
        samples = []
        labels = []
        for i in xrange(self.batch_size):
            captcha_text = random.sample(self.alphabet, self.captcha_size)
            data = self.image_captcha_gen.generate_image(captcha_text)
            captcha = np.array(data)
            captcha = cv2.resize(captcha, (self.width, self.height))
            label = self._get_label(captcha_text)
            labels.append(label)
            samples.append(captcha)

        samples = np.array(samples)
        labels = np.array(labels)
        samples = samples.astype(np.float32)
        samples = (samples - np.mean(samples, axis=(1, 2, 3),
                   keepdims=True)) / 255.0
        labels = labels.astype(np.int32)

        if self.data_format == "NCHW":
            samples = np.transpose(samples, axes=[0, 3, 1, 2])
        return samples, labels

    def _get_label(self, captcha_text):
        captcha_text = np.array(captcha_text)
        label = []
        for letter in captcha_text:
            i = np.argwhere(self.alphabet == letter)
            label.append(i)

        label = np.array(label).flatten()
        return label

    def size(self):
        return self.queue.size()

    def dequeue(self):
        output = self.queue.dequeue()
        return output

    def _thread_main(self, session):
        while not self.coord.should_stop():
            data, labels = self.get_batch()

            try:
                session.run(self.enqueue_op,
                            feed_dict={self.images_pl: data,
                                       self.labels_pl: labels})
            except tf.errors.CancelledError:
                return

    def start_threads(self, session, n_threads=multiprocessing.cpu_count()):
        for _ in range(n_threads):
            thread = threading.Thread(target=self._thread_main, args=(session,))
            thread.daemon = True  # Thread will close when parent quits.
            thread.start()
            self.threads.append(thread)
        return self.threads

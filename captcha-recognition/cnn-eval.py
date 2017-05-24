#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    evaluation on test script.
"""

import argparse
import math
import os
import time
from datetime import datetime
import numpy as np

import captcha_input as cap_in
import generate_data
import tensorflow as tf

ALPHABET = generate_data.ALPHABET
CAPTCHA_SIZE = generate_data.CAPTCHA_SIZE


def eval_once(app_args, saver):
    """
        Run Eval once.
        Args:
        saver: Saver.
        summary_writer: Summary writer.
        top_k_op: Top K op.
        summary_op: Summary op.
    """
    config = tf.ConfigProto(device_count={"GPU": app_args.gpu_count})
    sess = tf.InteractiveSession(config=config)
    saver.restore(sess, tf.train.latest_checkpoint(app_args.checkpoint_dir))
    coord = tf.train.Coordinator()

    manager = cap_in.CaptchaDataManager(app_args.batch_size,
                                        CAPTCHA_SIZE, app_args.hdf5_dir,
                                        coord, app_args.data_format)

    images, labels = tf.get_collection("inputs")

    logits = tf.get_collection("logits")[0]
    loss = tf.get_collection(tf.GraphKeys.LOSSES)[0]
    prediction = tf.argmax(logits, 2)
    actual = tf.argmax(labels, 2)
    equal = tf.equal(tf.cast(prediction, tf.int32),
                     tf.cast(actual, tf.int32))

    num_iter = int(math.ceil(manager.samples_count / app_args.batch_size))
    true_count = 0
    total_sample_count = num_iter * app_args.batch_size
    step = 0
    loss_mean = 0

    while step < num_iter and not coord.should_stop():
        im_feed, l_feed = manager.next_batch()

        loss_val, predictions = sess.run(
            [loss, equal], feed_dict={images: im_feed,
                                      labels: l_feed})
        loss_mean += loss_val
        true_count += np.sum(predictions)
        step += 1

    precision = true_count / (float(total_sample_count) * CAPTCHA_SIZE)
    print("%s: Precision = %f, Loss = %f" % (datetime.now(), precision,
                                             (loss_mean / step)))

    coord.request_stop()
    sess.run(manager.queue.close(cancel_pending_enqueues=True))


def evaluate(app_args):
    graph_path = ""
    for file in os.listdir(app_args.checkpoint_dir):
        if file.endswith(".meta"):
            if file > graph_path:
                graph_path = file
    graph_path = os.path.join(app_args.checkpoint_dir, graph_path)
    saver = tf.train.import_meta_graph(graph_path)
    while True:
        eval_once(app_args, saver)
        if app_args.eval_once:
            break
        else:
            time.sleep(app_args.eval_interval)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--hdf5-dir",
                        help="Path to the data directory",
                        default="test_data")

    parser.add_argument("--batch-size", type=int,
                        help="Number of images to process in a batch",
                        default=256)

    parser.add_argument("--log-dir",
                        help="Path to the directory, where log will write",
                        default="cnn_eval")

    parser.add_argument("--checkpoint-dir",
                        help="Path to the directory, where checkpoint stores",
                        default="cnn_train")

    parser.add_argument("--eval-interval", type=int,
                        help="How often to evaluate",
                        default=10)

    parser.add_argument("--eval-once", type=bool,
                        help="Eval one time or more",
                        default=False)

    parser.add_argument("--data-format",
                        help="Data format: NCHW or NHWC",
                        default="NHWC")

    parser.add_argument("--gpu-count", type=int,
                        help="Count of GPUs, if zero, then use CPU",
                        default=0)

    app_args = parser.parse_args()
    if tf.gfile.Exists(app_args.log_dir):
        tf.gfile.DeleteRecursively(app_args.log_dir)
    tf.gfile.MakeDirs(app_args.log_dir)
    tf.logging.set_verbosity("DEBUG")
    evaluate(app_args)

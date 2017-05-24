#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    evaluation on test script.
"""

import argparse
import cv2
import os
import time
import random
import numpy as np
from PIL import Image

import generate_data
import tensorflow as tf
from captcha.image import ImageCaptcha

ALPHABET = generate_data.ALPHABET
CAPTCHA_SIZE = generate_data.CAPTCHA_SIZE

fonts = []
for file in os.listdir("fonts"):
    if file.endswith(".ttf"):
        fonts.append(os.path.join("fonts", file))
image_captcha_gen = ImageCaptcha(fonts=fonts, height=60, width=160)


def next_im():
    captcha_text = random.sample(ALPHABET, CAPTCHA_SIZE)
    data = image_captcha_gen.generate_image(captcha_text)
    captcha = np.array(data)
    captcha = cv2.resize(captcha, (160, 60))
    label = generate_data._get_label(captcha_text)

    return captcha, label


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

    images, labels = tf.get_collection("inputs")
    logits = tf.get_collection("logits")[0]
    loss = tf.get_collection(tf.GraphKeys.LOSSES)[0]
    prediction = tf.argmax(logits, 2)

    while True:
        im, l = next_im()
        img = Image.fromarray(im, 'RGB')
        img.show()
        raw_input()

        im = (im - np.mean(im, axis=(0, 1, 2), keepdims=True)) / 255.0
        l = l.astype(np.int32)
        im = np.expand_dims(im, axis=0)
        l = np.expand_dims(l, axis=0)

        loss_val, predictions = sess.run(
            [loss, prediction], feed_dict={images: im, labels: l})
        inds = predictions.flatten()
        print(ALPHABET[inds])

    coord.request_stop()


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
            time.sleep(0)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument("--checkpoint-dir",
                        help="Path to the directory, where checkpoint stores",
                        default="cnn_train")

    parser.add_argument("--data-format",
                        help="Data format: NCHW or NHWC",
                        default="NHWC")

    parser.add_argument("--gpu-count", type=int,
                        help="Count of GPUs, if zero, then use CPU",
                        default=0)

    app_args = parser.parse_args()
    tf.logging.set_verbosity("DEBUG")
    evaluate(app_args)

#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    This script converts image-net original files to hdf5.
"""

import argparse
import os
import h5py
import string
import shutil
import cv2
import random
import numpy as np
from captcha.image import ImageCaptcha


ALPHABET = np.array(list(string.ascii_lowercase + string.digits))


def _get_label(captcha_text):
    captcha_text = np.array(captcha_text)
    label = []
    for letter in captcha_text:
        i = np.argwhere(ALPHABET == letter)
        label.append(i)

    label = np.array(label).flatten()
    return label


def main(app_args):
    """
        Main application.
    """
    if os.path.exists(app_args.out_path):
        shutil.rmtree(app_args.out_path)
    os.makedirs(app_args.out_path)

    fonts = []
    for file in os.listdir("fonts"):
        if file.endswith(".ttf"):
            fonts.append(os.path.join(app_args.fonts_dir, file))
    image_captcha_gen = ImageCaptcha(fonts=fonts, height=app_args.image_height,
                                     width=app_args.image_width)

    for i in xrange(app_args.files_count):
        samples = []
        labels = []
        for k in xrange(app_args.file_size):
            captcha_text = random.sample(ALPHABET, app_args.captcha_size)
            data = image_captcha_gen.generate_image(captcha_text)
            captcha = np.array(data)
            captcha = cv2.resize(captcha, (app_args.image_width,
                                           app_args.image_height))
            label = _get_label(captcha_text)
            labels.append(label)
            samples.append(captcha)
            if k % 1000 == 0:
                print("Generated: %d" % k)

        samples = np.array(samples)
        labels = np.array(labels)
        labels = labels.astype(np.int32)

        cur_file_name = "%d_set.hdf5" % i
        cur_hdf5 = h5py.File(os.path.join(app_args.out_path, cur_file_name),
                             "w")
        cur_hdf5.create_dataset("data", data=samples)
        cur_hdf5.create_dataset("labels", data=labels)
        cur_hdf5.close()

        print("Write %s" % cur_file_name)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--fonts-dir",
                        help="Path to the fonts directory",
                        default="fonts")

    parser.add_argument(
        "--out-path",
        help="Path to the directory, where results will write(hdf5 files)",
        default="train_data")

    parser.add_argument(
        "--files-count", type=int,
        help="Number of hdf5 files",
        default=1)

    parser.add_argument(
        "--file-size", type=int,
        help="Count of images in each file",
        default=5000)

    parser.add_argument(
        "--captcha-size", type=int,
        help="Count of symbols in each captcha",
        default=5)

    parser.add_argument(
        "--image-width", type=int,
        help="Image width value",
        default=160)

    parser.add_argument(
        "--image-height", type=int,
        help="Image height value",
        default=60)

    app_args = parser.parse_args()

    main(app_args)

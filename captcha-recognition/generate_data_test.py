#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
    This script checks results of image-net-to-hdf5.py
"""

import argparse
import h5py
import os
import random
import cv2
import numpy as np
import generate_data

ALPHABET = generate_data.ALPHABET


def show_data_in_the_path(path, images_count):
    images = []
    labels = []
    for file in os.listdir(path):
        if file.endswith(".hdf5"):
            cur_hdf5 = h5py.File(os.path.join(path, file), "r")
            cur_images = cur_hdf5["data"][0:images_count]
            cur_labels = cur_hdf5["labels"][0:images_count]

            cur_images = cur_images.astype(np.uint8)
            cur_labels = cur_labels.astype(np.int8)
            cur_labels = np.argmax(cur_labels, 2)

            images.extend(cur_images)
            labels.extend(cur_labels)

            cur_hdf5.close()

    images = np.array(images)
    labels = np.array(labels)
    indeces = random.sample(np.arange(0, len(images)), images_count)
    images = images[indeces]
    labels = labels[indeces]

    for image, label in zip(images, labels):
        print ALPHABET[label]
        cv2.imshow("1", image)
        cv2.waitKey(0)


def main(app_args):
    show_data_in_the_path(app_args.path,
                          app_args.images_count)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--path",
        help="Path to the data directory",
        default="train_data")

    parser.add_argument(
        "--images-count", type=int,
        help="Count of images, that we will show and check",
        default=20)

    app_args = parser.parse_args()

    main(app_args)

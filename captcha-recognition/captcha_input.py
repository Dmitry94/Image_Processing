"""
    This module contains captcha generator.
"""

import os
import random
import collections
import numpy as np

from captcha.image import ImageCaptcha


CaptchaSample = collections.namedtuple('CaptchaSample', ['captcha', 'text'])


class CaptchaGenerator(object):
    """
        Class that incapsulates captcha generation.
    """
    def __init__(self, fonts_dir, alphabet, captcha_size,
                 width=160, height=60):
        fonts = []
        for file in os.listdir("fonts"):
            if file.endswith(".ttf"):
                fonts.append(os.path.join(fonts_dir, file))

        self.width = width
        self.height = height
        self.image_captcha_gen = ImageCaptcha(fonts=fonts,
                                              height=height, width=width)
        self.alphabet = alphabet
        self.captcha_size = captcha_size

    def get_samples(self, count):
        """
            Generates batch of samples.
        """
        samples = []
        for i in xrange(count):
            captcha_text = random.sample(self.alphabet, self.captcha_size)
            data = self.image_captcha_gen.generate_image(captcha_text)
            captcha = np.array(data)
            cur_sample = CaptchaSample(captcha=captcha, text=captcha_text)
            samples.append(cur_sample)


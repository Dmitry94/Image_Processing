import os
import numpy as np

from captcha.image import ImageCaptcha
from matplotlib import pyplot as plt


class CaptchaGenerator(object):
    """
        Class that incapsulates captcha generation.
        Constructor gets fonts directory as inputs.
    """
    def __init__(self, fonts_dir, width=160, height=60):
        fonts = []
        for file in os.listdir("fonts"):
            if file.endswith(".ttf"):
                fonts.append(os.path.join(fonts_dir, file))

        self.width = width
        self.height = height
        self.image_captcha_gen = ImageCaptcha(fonts=fonts,
                                              height=height, width=width)

    def chariter(self, filelike):
        octet = filelike.read(1)
        while octet:
            yield ord(octet)
            octet = filelike.read(1)

    def get_samples(self, count):
        """
            Generates samples using fonts from constructor.
        """
        for i in xrange(count):
            data = self.image_captcha_gen.generate_image('1234')
            data = np.array(data)
            plt.imshow(data)
            plt.show()

generator = CaptchaGenerator("fonts")
generator.get_samples(100)
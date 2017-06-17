import cv2
import numpy as np
from matplotlib import pyplot as plt


def show_spectrum_and_original(image):
    f = cv2.dft(np.float32(image), flags = cv2.DFT_COMPLEX_OUTPUT)
    fshift = np.fft.fftshift(f)
    magnitude_spectrum = 20*np.log(cv2.magnitude(fshift[:,:,0], fshift[:,:,1]))

    plt.subplot(121), plt.imshow(image, cmap='gray')
    plt.subplot(122), plt.imshow(magnitude_spectrum, cmap='gray')
    plt.show()


def apply_filter(image, filter):
    dft = cv2.dft(np.float32(image), flags=cv2.DFT_COMPLEX_OUTPUT)
    dft_shift = np.fft.fftshift(dft)

    fshift = dft_shift * filter
    f_ishift = np.fft.ifftshift(fshift)
    idft = cv2.idft(f_ishift)
    idft = cv2.magnitude(idft[:, :, 0], idft[:, :, 1])

    return idft


if __name__ == "__main__":
    IMAGE_PATH = "skeleton.png"
    img = cv2.imread(IMAGE_PATH, cv2.IMREAD_GRAYSCALE)
    show_spectrum_and_original(img)

    rows, cols = img.shape
    crow, ccol = rows / 2, cols / 2
    mask_h = np.zeros((rows, cols, 2), np.float32)
    D0 = 25
    for i in xrange(rows):
        for j in xrange(cols):
            D = np.sqrt((i-crow)**2 + (j-ccol)**2)
            if D <= D0:
                mask_h[i, j] = 1.0

    res1 = apply_filter(img, mask_h)
    show_spectrum_and_original(mask_h[:,:,1])
    show_spectrum_and_original(res1)


    mask_l = np.zeros((rows, cols, 2), np.float32)
    D0 = 10
    for i in xrange(rows):
        for j in xrange(cols):
            D = np.sqrt((i-crow)**2 + (j-ccol)**2)
            if D > D0:
                mask_l[i, j] = 1.0

    res2 = apply_filter(img, mask_l)
    show_spectrum_and_original(mask_l[:,:,1])
    show_spectrum_and_original(res2)
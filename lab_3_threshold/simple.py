import cv2
import numpy as np


WINDOW_NAME = "WINDOW"


def nothing(x):
    pass


original_image = cv2.imread("../content/Lenna.png", cv2.IMREAD_GRAYSCALE)
original_image = cv2.resize(original_image, (800, 600))
showing_image = original_image.copy()
cv2.namedWindow(WINDOW_NAME)
cv2.moveWindow(WINDOW_NAME, 100, 100)

cv2.createTrackbar('Top threshold', WINDOW_NAME, 100, 255, nothing)
cv2.createTrackbar('Bot threshold', WINDOW_NAME, 255, 255, nothing)

while(1):
    k = cv2.waitKey(1) & 0xFF
    if k == 27:
        break

    top_thr = cv2.getTrackbarPos('Top threshold', WINDOW_NAME)
    bot_thr = cv2.getTrackbarPos('Bot threshold', WINDOW_NAME)

    mask = np.logical_and(original_image < top_thr, original_image < bot_thr)
    showing_image[mask] = 255
    showing_image[~mask] = 0
    cv2.imshow(WINDOW_NAME, showing_image)

cv2.destroyAllWindows()

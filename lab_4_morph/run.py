import cv2
from matplotlib import pyplot as plt
import numpy as np
import time as t

def get_small_ellipse(image_size, ellipse_r):
    ellipse = np.zeros((image_size, image_size, 1), np.uint8)
    center = image_size / 2
    def mask(x,y):
        return (x - center)**2 + (y - center)**2 < ellipse_r**2
    for i in xrange(image_size):
        for j in xrange(image_size):
            if mask(i, j):
                ellipse[i, j] = 1

    return ellipse

def get_ring(out_r, in_r=0):
    e = cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(out_r, out_r))
    e = e.reshape(e.shape[0], e.shape[1], 1)
    in_e = get_small_ellipse(out_r, in_r / 2)
    return e if in_r == 0 else e - in_e


print("OpenCV Version : %s " % cv2.__version__)

img_path = "sample.png"
img = cv2.imread(img_path)

WINDOW_NAME = "1"

holedots = cv2.erode(img, get_ring(100, 96))
cv2.imshow(WINDOW_NAME, holedots)
cv2.waitKey()

holefill = cv2.dilate(holedots, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (100, 100)))
cv2.imshow(WINDOW_NAME, holefill)
cv2.waitKey()

img_hole_fill = cv2.bitwise_or(holefill, img)
cv2.imshow(WINDOW_NAME, img_hole_fill)
cv2.waitKey()

gear_body = get_ring(100)
inner = cv2.morphologyEx(img_hole_fill, cv2.MORPH_OPEN, gear_body)
inner = cv2.dilate(inner, get_ring(2))
cv2.imshow(WINDOW_NAME, inner)
cv2.waitKey()

grar_batt = cv2.bitwise_and(img_hole_fill, cv2.bitwise_not(inner))
grar_batt = cv2.erode(grar_batt, get_ring(3))
cv2.imshow(WINDOW_NAME, grar_batt)
cv2.waitKey()

gear_area = cv2.dilate(grar_batt, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (18, 18)))
cv2.imshow(WINDOW_NAME, gear_area)
cv2.waitKey()

result_1 = cv2.bitwise_and(cv2.bitwise_xor(inner, cv2.dilate(inner, get_ring(10))), cv2.bitwise_not(gear_area))
result_1 = cv2.dilate(result_1, get_ring(20))
cv2.imshow(WINDOW_NAME, result_1)
cv2.waitKey()

defect_cue = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (20, 20))
result_2 = cv2.bitwise_or(gear_area, cv2.dilate(result_1, defect_cue))
cv2.imshow(WINDOW_NAME, result_2)
cv2.waitKey()

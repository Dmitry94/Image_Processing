import os
import cv2
import numpy as np

PATH = "3dwall"
WINDOW_NAME = "Window"

if __name__ == "__main__":
    cv2.namedWindow(WINDOW_NAME, cv2.WINDOW_NORMAL)
    cv2.moveWindow(WINDOW_NAME, 100, 100)

    files = os.listdir(PATH)
    # files = [files[-1]]
    for file in files:
        file_path = os.path.join(PATH, file)
        image = cv2.imread(file_path, cv2.IMREAD_COLOR)
        image = cv2.resize(image, (800, 600))
        if image is None:
            continue;

        cv2.imshow(WINDOW_NAME, image)
        cv2.waitKey(0)

        gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        gray_image = cv2.Canny(gray_image, 50, 150, apertureSize=3)
        cv2.imshow(WINDOW_NAME, gray_image)
        cv2.waitKey(0)

        output = image.copy()
        minLineLength = 100 #200
        maxLineGap = 10 # 25
        lines = cv2.HoughLinesP(gray_image, 1, np.pi/180, 100, minLineLength, maxLineGap)
        if lines is not None:
            for x1, y1, x2, y2 in lines[0]:
                cv2.line(output, (x1, y1), (x2, y2), (0, 0, 255), 2)

            cv2.imshow(WINDOW_NAME, output)
            cv2.waitKey(0)

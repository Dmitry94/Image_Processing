import cv2
import numpy as np

from matplotlib import pyplot as plt

def threshold_otsu(img):
    hist = cv2.calcHist([img], [0], None, [256], [0, 256])
    hist_norm = hist.ravel() / hist.max()
    Q = hist_norm.cumsum()
    bins = np.arange(256)
    fn_min = np.inf
    thresh = -1
    for i in xrange(1, 256):
        p1, p2 = np.hsplit(hist_norm, [i])  # probabilities
        q1, q2 = Q[i], Q[255] - Q[i]  # cum sum of classes
        b1, b2 = np.hsplit(bins, [i])  # weights
        # finding means and variances
        m1, m2 = np.sum(p1 * b1) / q1, np.sum(p2 * b2) / q2
        v1, v2 = np.sum(((b1 - m1) ** 2) * p1) / q1, np.sum(((b2 - m2) ** 2) * p2) / q2
        # calculates the minimization function
        fn = v1 * q1 + v2 * q2
        if fn < fn_min:
            fn_min = fn
            thresh = i

    otsu = img.copy()
    mask = img > thresh
    otsu[mask] = 255
    otsu[~mask] = 0

    return thresh, otsu


def get_fragments(img, x_frags, y_frags):
    x_size, y_size = img.shape[1] // x_frags, img.shape[0] // y_frags
    x_coords, y_coords = [], []
    for i in range(x_frags-1):
        x_coords.append((i*x_size, (i+1)*x_size))
    x_coords.append(((x_frags-1)*x_size, img.shape[1]))
    for i in range(y_frags-1):
        y_coords.append((i*y_size, (i+1)*y_size))
    y_coords.append(((y_frags-1)*y_size, img.shape[0]))
    fragments = []
    for yc in y_coords:
        for xc in x_coords:
            fragments.append((xc[0], yc[0], xc[1], yc[1]))
    return fragments

def show_fragments(frags, x_frags, y_frags):
    fig = plt.figure()
    for i in range(x_frags*y_frags):
        a = fig.add_subplot(y_frags, x_frags, i+1)
        imgplot = plt.imshow(frags[i], cmap='gray')
        a.set_title(str(i))

def quantization(img_src, levels_num):
    img = img_src.copy()
    level_len = 256 // levels_num
    img = img // level_len
    img = img * level_len
    return img


def otsu_hierarchical(img, start=0, end=256):
    if start == end:
        return end
    hist = cv2.calcHist([img], [0], None, [256], [0, 256])
    sum_all = 0
    for t in range(start, end):
        sum_all += t * hist[t]
    sum_back, w_back, w_fore, var_max, threshold = 0, 0, 0, 0, 0
    total = img.size
    for t in range(start, end):
        w_back += hist[t]
        if (w_back == 0):
            continue
        w_fore = total - w_back  # Weight Foreground
        if (w_fore == 0):
            break
        sum_back += t * hist[t]

        mean_back = sum_back / w_back  # Mean Background
        mean_fore = (sum_all - sum_back) / w_fore  # Mean Foreground

        # Calculate Between Class Variance
        var_between = w_back * w_fore * (mean_back - mean_fore) ** 2

        # Check if new maximum found
        if (var_between > var_max):
            var_max = var_between
            threshold = t

    return threshold

def diapasone_threshold(simg, oimg, start, end, t, white=True):
    if t == 0 and t < start:
        return oimg
    if white:
        col1 = 255
        col2 = 0
    else:
        col1 = 0
        col2 = 255
    for i in range(img.shape[0]):
        for j in range(img.shape[1]):
            c = img[i, j]
            if c >= start and c <= end:
                if c >= t:
                    oimg[i, j] = col1
                else:
                    oimg[i, j] = col2


if __name__ == "__main__":
    img = cv2.imread('Lenna.png', cv2.IMREAD_GRAYSCALE)
    thresh, otsu = threshold_otsu(img)

    cv2.imshow("Otsu global", otsu)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    img_loc = img.copy()
    x_frags, y_frags = 3, 3
    x_size, y_size = img_loc.shape[1] // x_frags, img_loc.shape[0] // y_frags
    frags = get_fragments(img_loc, x_frags, y_frags)

    img_frags = [img_loc[y1:y2, x1:x2] for x1, y1, x2, y2 in frags]
    show_fragments(img_frags, x_frags, y_frags)
    plt.show()

    otsu_frags = []
    otsu_local = np.zeros(img.shape)
    for coords in frags:
        x1, y1, x2, y2 = coords
        otsu_part = threshold_otsu(img_loc[y1:y2, x1:x2])[1]
        otsu_frags.append(otsu_part)
        otsu_local[y1:y2, x1:x2] = otsu_part

    show_fragments(otsu_frags, x_frags, y_frags)
    plt.show()
    cv2.imshow("Otsu local", otsu_local)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    quant_im = quantization(img, 8)
    cv2.imshow("Quantization", quant_im)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    count = np.unique(quant_im)
    print(count)

    # otsu_h = img.copy()
    # steps = 50
    # start, end = 0, 256
    # diapasones = [(start, end)]
    # thresholds = []
    # for i in range(steps - 1):
    #     new_diaps = []
    #     for s, e in diapasones:
    #         t = otsu_hierarchical(img, s, e)
    #         if t is not None and t != 0 and s != e:
    #             new_diaps.append((s, t))
    #             new_diaps.append((t, e))
    #         diapasones = new_diaps
    #
    # print(len(diapasones))
    # print(diapasones)
    #
    # white = True
    # for s, e in diapasones:
    #     print('{0},{1}: {2}'.format(s,e,t))
    #     t = otsu_hierarchical(otsu_h, s, e)
    #     diapasone_threshold(img, otsu_h, s, e, t, white=white)
    #     white = not white
    # cv2.imshow("Otsu hierarchical", otsu_h)
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()
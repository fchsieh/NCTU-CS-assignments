import os
from math import *

import numpy as np
from scipy import misc
from scipy.ndimage import gaussian_filter

import cv2


def reviseImage():
    img_names = [
        "1.jpeg", "2.jpeg", "3.jpeg", "dark.jpeg", "overexposure.jpeg"
    ]
    for filename in img_names:
        fn = "./img_data/cutted/" + filename
        img = cv2.imread(fn, cv2.IMREAD_GRAYSCALE)
        newimg = cv2.resize(img, (640, 480))
        cv2.imwrite("./part1/revised_" + filename, newimg)


def geoTransform():
    for fn in os.listdir("./part1"):
        path = "./part1/%s" % fn
        img = cv2.imread(path, cv2.IMREAD_UNCHANGED)
        # [1] https://docs.opencv.org/3.4.2/da/d6e/tutorial_py_geometric_transformations.html
        double = cv2.resize(img, (1280, 960), interpolation=cv2.INTER_LINEAR)
        shrink = cv2.resize(img, (320, 240), interpolation=cv2.INTER_CUBIC)
        stretch = cv2.resize(img, (1280, 720), interpolation=cv2.INTER_CUBIC)
        cv2.imwrite("./part2/double_" + fn, double)
        cv2.imwrite("./part2/shrink_" + fn, shrink)
        cv2.imwrite("./part2/stretch_" + fn, stretch)


def addGaussianNoises():
    img_names = [
        "1.jpeg", "2.jpeg", "3.jpeg", "dark.jpeg", "overexposure.jpeg"
    ]
    for (m, v) in [(10, 100), (50, 30), (200, 200)]:
        for filename in img_names:
            s = sqrt(v)
            fn = "./part1/revised_" + filename
            img = cv2.imread(fn, cv2.IMREAD_UNCHANGED)
            gaussian = np.random.normal(m, s, (480, 640))
            noisy_image = np.zeros(img.shape, np.float32)
            noisy_image = img + gaussian
            cv2.normalize(
                noisy_image, noisy_image, 0, 255, cv2.NORM_MINMAX, dtype=-1)
            noisy_image = noisy_image.astype(np.uint8)
            cv2.imwrite("./part3/%d_%d_" % (m, v) + filename, noisy_image)


def removeGaussianNoises():
    for fn in os.listdir("./part3"):
        path = "./part3/%s" % fn
        img = cv2.imread(path, cv2.IMREAD_UNCHANGED)
        newimg = cv2.fastNlMeansDenoising(img, None, 10, 7, 21)
        cv2.imwrite("./part4/denoise_" + fn, newimg)

# def 

def main():
    # part 1
    #reviseImage()
    # part 2
    #geoTransform()
    # part 3
    #addGaussianNoises()
    # part 4
    #removeGaussianNoises()
    # part 5


if __name__ == "__main__":
    main()

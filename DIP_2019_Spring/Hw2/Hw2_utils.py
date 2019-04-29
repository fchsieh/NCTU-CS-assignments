import os

import pandas as pd
import scipy.io
from PIL import Image

import cv2

pd.set_option('display.max_colwidth', -1)

#pd.set_option('display.max_columns', None)


def getPath(dir):
    for dirpath, _, filenames in os.walk(dir):
        for f in filenames:
            yield os.path.abspath(os.path.join(dirpath, f))


def resize():
    size = 960, 640
    for filename in getPath("testing"):
        if filename.endswith((".jpeg", ".jpg", ".png")):
            fn, ext = os.path.splitext(filename)
            jpg_format = str(fn + ".jpg")
            img = Image.open(filename)
            img = img.resize(size, Image.ANTIALIAS)
            img.save(jpg_format)


def rewriteImage():
    for testing in getPath("testing"):
        if testing.endswith(".png"):  # is a mask file
            img = cv2.imread(testing, 0)
            img = (img > 0) * 255
            cv2.imwrite(testing, img)


def outputMatRes():
    mat = scipy.io.loadmat("CNN_train/Test_out.mat")
    #mat = scipy.io.loadmat("test/all_superpixel_labels.mat")
    #print(mat["all_superpixel_labels"][0][0])
    print(mat)
    print(mat["out_mat"][0])


def main():
    #resize()
    #rewriteImage()
    outputMatRes()


if __name__ == "__main__":
    main()

import cv2
import numpy as np


def ReadImage():
    img = []
    img.append(cv2.imread("1.jpeg", 0))
    cv2.imshow('imgae', 0)
    return img


def main():
    img = ReadImage()


if __name__ == "__main__":
    main()
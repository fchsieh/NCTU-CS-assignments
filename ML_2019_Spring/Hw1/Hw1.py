import math
import random

import matplotlib.pyplot as plt
import numpy as np

import MatOP as mat
import PolyReg as poly


def generateTest():
    start = random.random() * -5
    cases = random.randint(15, 30)
    x = start
    with open("testfile.txt", "w") as file:
        for _ in range(cases):
            x = x + random.random()
            y = math.cos(x) + random.random() * random.random()  # shift
            file.write(str(x) + "," + str(y) + "\n")


def readInputFile():
    matrix = []
    with open("testfile.txt", "r") as file:
        dataLines = file.readlines()
        for data in dataLines:
            nums = data.split(",")
            matrix.append([float(i) for i in nums])  # Build data matrix
    return matrix


def printRes_LSE(coeff, error):
    # LSE:
    print("LSE:")
    print("Fitting line: ", end="")
    for j in range(len(coeff)):
        print(coeff[j], end="")
        if (j != len(coeff) - 1):
            print("X^%d + " % (len(coeff) - 1 - j), end="")
    print("\nTotal error: %f" % error)


def printRes_Newton(coeff, error):
    # Newton's
    print("\nNewton's method: ")
    print("Fitting line: ", end="")
    for j in range(len(coeff)):
        print(coeff[j], end="")
        if (j != len(coeff) - 1):
            print("X^%d + " % (len(coeff) - 1 - j), end="")
    print("\nTotal error: %f" % error)


def main():
    generateTest()
    data = readInputFile()
    lambda_ = 0
    for i in range(1, 11):
        print("\nn =", i, ", lambda = %d" % lambda_)
        n = poly.PolyReg(i, lambda_, data)
        # LSE
        coeff, err = n.rLSE()
        printRes_LSE(coeff, err)
        # Newton
        coeff, err = n.Newton()
        printRes_Newton(coeff, err)
        # Visualize
        n.visualize()

    lambda_ = 10000
    print("\nn = 3 , lambda = %d" % lambda_)
    n = poly.PolyReg(3, lambda_, data)
    # LSE
    coeff, err = n.rLSE()
    printRes_LSE(coeff, err)
    # Newton's
    coeff, err = n.Newton()
    printRes_Newton(coeff, err)
    # Visualize
    n.visualize()


if __name__ == "__main__":
    main()

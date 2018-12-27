#!/usr/bin/python
# -*- coding: utf-8 -*-
import numpy as np
import pandas as pd
from sklearn import model_selection
from sklearn import metrics
from itertools import chain, combinations_with_replacement


def scale(mat):
    mean = np.nanmean(mat, axis=0)
    scale = np.nanstd(mat, axis=0)
    mat -= mean
    mat /= scale
    return mat


def build_power_matrix():
    # power matrix is a map for expanding features
    # for degree 3 and 8 variables, the power matrix will be 165*8
    combinations = chain.from_iterable(combinations_with_replacement(range(8), i)\
                                       for i in range(0, 4))
    return np.vstack([np.bincount(
        c,
        minlength=8,
    )] for c in combinations)


def build_poly_matrix(X, poly):
    m = X.shape[0]
    n = poly.shape[0]

    # initialize poly feature matrix, new matrix's dimension is m*n
    X_poly = np.zeros(shape=(m, n))

    # build feature matrix
    for row in range(m):
        for col in range(n):
            X_poly[row][col] = np.product(np.power(X[row], poly[col]))

    return X_poly


def compute_cost(X, y, theta, Lambda):
    m = y.shape[0]

    J = (1/(2*m)) * np.sum(np.square(X.dot(theta) - y)) + \
            (Lambda/(2*m)) * np.sum(np.square(theta[1:]))

    grad = (1/m) * X.transpose().dot((X.dot(theta) - y)) + \
            (Lambda/m) * np.concatenate((np.zeros((1,1)), theta[1:]), axis=0)

    return (J, grad)


def gradient_descent(X, y, theta, alpha=0.01, Lambda=0, max_iter=10000):
    m = y.shape[0]

    for i in range(max_iter):
        J, grad = compute_cost(X, y, theta, Lambda)
        theta = theta - alpha * (1 / m) * grad

    return theta


if __name__ == "__main__":
    # read data
    dataframe = pd.read_csv("./Concrete_Data.csv")
    X = dataframe.iloc[:, :-1].values
    y = dataframe.iloc[:, -1].values.reshape(-1, 1)

    # first do feature scaling
    X = scale(X)
    y = scale(y)
    X_train, X_test, y_train, y_test = model_selection.train_test_split(
        X, y, test_size=0.2)
    # do feature expansion
    pow_matrix = build_power_matrix()
    poly_X_train = build_poly_matrix(X_train, pow_matrix)
    poly_X_test = build_poly_matrix(X_test, pow_matrix)

    # do gradient descent
    theta_init = np.random.randint(1, size=(poly_X_train.shape[1], 1))
    theta = gradient_descent(poly_X_train, y_train, theta_init, 3.3, 7, 300000)
    y_pred = poly_X_test.dot(theta)
    print(metrics.r2_score(y_test, y_pred))  # print r2 score

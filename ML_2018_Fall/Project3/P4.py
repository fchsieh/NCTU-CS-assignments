#!/usr/bin/python
# -*- coding: utf-8 -*-
import numpy as np
import pandas as pd
import math
from sklearn import model_selection
from sklearn import metrics
from itertools import chain, combinations_with_replacement
from sklearn.metrics import mean_squared_error

def build_power_matrix():
    # power matrix is a map for expanding features
    # for degree 3 and 8 variables, the power matrix will be 165*8
    combinations = chain.from_iterable(combinations_with_replacement(range(8), i)\
                                       for i in range(0, 4))
    return np.vstack([np.bincount(c, minlength=8,)] for c in combinations)

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

def gradientDescent(X,Y,theta,iters,alpha):
    r2 = 0
    MSE = 0
    for i in range(iters):
        r2 += metrics.r2_score(X.dot(theta), Y)
        MSE += mean_squared_error(G, Y_train)
        theta = theta - (alpha/len(X)) * X.transpose().dot((X.dot(theta) - Y))
    return theta, r2, MSE


if __name__ == "__main__":
    
    data = pd.read_csv("./Concrete_Data.csv")
    # first do feature scaling
    X = data.iloc[:, :-1].values
    Y = data.iloc[:, -1].values.reshape(-1,1)
    X -= np.nanmean(X, axis=0)
    X /= np.nanstd(X, axis=0)
    Y -= np.nanmean(Y, axis=0)
    Y /= np.nanstd(Y, axis=0)

    X_train, X_test, Y_train, Y_test = model_selection.train_test_split(X, Y, test_size=0.2)
    
    # do feature expansion
    pow_matrix = build_power_matrix()
    poly_X_train = build_poly_matrix(X_train, pow_matrix)
    poly_X_test = build_poly_matrix(X_test, pow_matrix)
    
    

    # delete the top N large MSE features
    m = X_train.shape[0]
    n = pow_matrix.shape[0]
    SS = np.zeros(shape=(n))
    for col in range(n):
        G = poly_X_train[:,col]
        SS[col] = mean_squared_error(G, Y_train)
    N = 10
    top_N_index = sorted(range(len(SS)), key=lambda i: SS[i])[-N:]
    for i in range(N):
        poly_X_train[:,top_N_index[i]] = 0
        poly_X_test[:,top_N_index[i]] = 0

    # do gradient descent
    theta = np.random.randint(1, size=(poly_X_train.shape[1], 1))
    theta, r2, MSE = gradientDescent(poly_X_train, Y_train, theta, 10000, 0.01)
    r2/=10000
    MSE/=10000
    
    Y_pred = poly_X_test.dot(theta)
    print("mean of training r2_score",r2) # print r2 score
    print("result r2_score", metrics.r2_score(Y_test, Y_pred)) # print r2 score
    print("mean of training MSE",MSE) # print r2 score
    print("result MSE", mean_squared_error(Y_test, Y_pred)) # print r2 score

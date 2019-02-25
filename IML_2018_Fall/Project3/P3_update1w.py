# -*- coding: utf-8 -*-

import numpy as np
import pandas as pd
import random
import matplotlib.pyplot as plt
from sklearn import linear_model
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import r2_score
from sklearn.metrics import mean_squared_error
data = pd.read_csv("./Concrete_Data.csv")

output = data[data.columns.values[-1]]  # y, Concrete compressive strength(MPa, megapascals)

x_train, x_test,z_train, z_test,aa_train, aa_test,bb_train, bb_test,cc_train, cc_test,dd_train, dd_test,ee_train, ee_test,ff_train, ff_test,y_train, y_test = train_test_split(
    data["Cement (component 1)(kg in a m^3 mixture)"],
    data["Blast Furnace Slag (component 2)(kg in a m^3 mixture)"],
    data["Fly Ash (component 3)(kg in a m^3 mixture)"],
    data["Water  (component 4)(kg in a m^3 mixture)"],
    data["Superplasticizer (component 5)(kg in a m^3 mixture)"],
    data["Coarse Aggregate  (component 6)(kg in a m^3 mixture)"],
    data["Fine Aggregate (component 7)(kg in a m^3 mixture)"],
    data["Age (day)"],
    output,
    test_size=0.2,
    random_state=None)

x_train = x_train.values.reshape(-1, 1)
x_test = x_test.values.reshape(-1, 1)
z_train = z_train.values.reshape(-1, 1)
z_test = z_test.values.reshape(-1, 1)
aa_train = aa_train.values.reshape(-1, 1)
aa_test = aa_test.values.reshape(-1, 1)
bb_train = bb_train.values.reshape(-1, 1)
bb_test = bb_test.values.reshape(-1, 1)
cc_train = cc_train.values.reshape(-1, 1)
cc_test = cc_test.values.reshape(-1, 1)
dd_train = dd_train.values.reshape(-1, 1)
dd_test = dd_test.values.reshape(-1, 1)
ee_train = ee_train.values.reshape(-1, 1)
ee_test = ee_test.values.reshape(-1, 1)
ff_train = ff_train.values.reshape(-1, 1)
ff_test = ff_test.values.reshape(-1, 1)
y_train1=y_train
y_train = y_train.values.reshape(-1, 1)
n = 0
pk=0
for i in x_train:
    n += 1
a = 0.000000000025 # learning rate
precision = 0.001 # a small value
w0 = random.uniform(-30, -20) 
w1 = random.uniform(0.1, 0.13) 
w2 = random.uniform(0.09, 0.11) 
w3 = random.uniform(0.08, 0.09)
w4 = random.uniform(-0.2, -.01) 
w5 = random.uniform(0.2, 0.3)
w6 = random.uniform(0.01, 0.03) 
w7 = random.uniform(0.01, 0.03)
w8 = random.uniform(0.1, 0.12) 

max_iter = 1000 # maximum iterations
diff0 = 1.0
diff1 = 1.0
diff2 = 1.0
diff3 = 1.0
diff4 = 1.0
diff5 = 1.0
diff6 = 1.0
diff7 = 1.0
diff8 = 1.0
k = 0

# gradient descent
while k<1000 and (diff0 > precision or diff1 > precision or diff2 > precision or diff3 > precision or diff4 > precision or diff5 > precision or diff6 > precision or diff7 > precision or diff8 > precision):
    i = 0
    errorDelta0 = 0.0
    errorDelta1 = 0.0
    errorDelta2 = 0.0
    errorDelta3 = 0.0
    errorDelta4 = 0.0
    errorDelta5 = 0.0
    errorDelta6 = 0.0
    errorDelta7 = 0.0
    errorDelta8 = 0.0
    while i < n:
        errorDelta0 += (y_train[i][0] - (w0 + (w1 * x_train[i][0])+ (w2 * z_train[i][0])+ (w3 * aa_train[i][0])+ (w4 * bb_train[i][0])+ (w5 * cc_train[i][0])+ (w6 * dd_train[i][0])+ (w7 * ee_train[i][0])+ (w8 * ff_train[i][0])))
        errorDelta1 += ((y_train[i][0] - (w0 + (w1 * x_train[i][0])+ (w2 * z_train[i][0])+ (w3 * aa_train[i][0])+ (w4 * bb_train[i][0])+ (w5 * cc_train[i][0])+ (w6 * dd_train[i][0])+ (w7 * ee_train[i][0])+ (w8 * ff_train[i][0]))) * x_train[i][0])
        errorDelta2 += ((y_train[i][0] - (w0 + (w1 * x_train[i][0])+ (w2 * z_train[i][0])+ (w3 * aa_train[i][0])+ (w4 * bb_train[i][0])+ (w5 * cc_train[i][0])+ (w6 * dd_train[i][0])+ (w7 * ee_train[i][0])+ (w8 * ff_train[i][0]))) * z_train[i][0])
        errorDelta3 += ((y_train[i][0] - (w0 + (w1 * x_train[i][0])+ (w2 * z_train[i][0])+ (w3 * aa_train[i][0])+ (w4 * bb_train[i][0])+ (w5 * cc_train[i][0])+ (w6 * dd_train[i][0])+ (w7 * ee_train[i][0])+ (w8 * ff_train[i][0]))) * aa_train[i][0])
        errorDelta4 += ((y_train[i][0] - (w0 + (w1 * x_train[i][0])+ (w2 * z_train[i][0])+ (w3 * aa_train[i][0])+ (w4 * bb_train[i][0])+ (w5 * cc_train[i][0])+ (w6 * dd_train[i][0])+ (w7 * ee_train[i][0])+ (w8 * ff_train[i][0]))) * bb_train[i][0])
        errorDelta5 += ((y_train[i][0] - (w0 + (w1 * x_train[i][0])+ (w2 * z_train[i][0])+ (w3 * aa_train[i][0])+ (w4 * bb_train[i][0])+ (w5 * cc_train[i][0])+ (w6 * dd_train[i][0])+ (w7 * ee_train[i][0])+ (w8 * ff_train[i][0]))) * cc_train[i][0])
        errorDelta6 += ((y_train[i][0] - (w0 + (w1 * x_train[i][0])+ (w2 * z_train[i][0])+ (w3 * aa_train[i][0])+ (w4 * bb_train[i][0])+ (w5 * cc_train[i][0])+ (w6 * dd_train[i][0])+ (w7 * ee_train[i][0])+ (w8 * ff_train[i][0]))) * dd_train[i][0])
        errorDelta7 += ((y_train[i][0] - (w0 + (w1 * x_train[i][0])+ (w2 * z_train[i][0])+ (w3 * aa_train[i][0])+ (w4 * bb_train[i][0])+ (w5 * cc_train[i][0])+ (w6 * dd_train[i][0])+ (w7 * ee_train[i][0])+ (w8 * ff_train[i][0]))) * ee_train[i][0])
        errorDelta8 += ((y_train[i][0] - (w0 + (w1 * x_train[i][0])+ (w2 * z_train[i][0])+ (w3 * aa_train[i][0])+ (w4 * bb_train[i][0])+ (w5 * cc_train[i][0])+ (w6 * dd_train[i][0])+ (w7 * ee_train[i][0])+ (w8 * ff_train[i][0]))) * ff_train[i][0])        
        i += 1
    if(k%9 ==0):
        w0 = w0 + a * errorDelta0
    elif(k%9 ==1):
        w1 = w1 + a * errorDelta1
    elif(k%9 ==2):
        w2 = w2 + a * errorDelta2
    elif(k%9 ==3):
        w3 = w3 + a * errorDelta3
    elif(k%9 ==4):
        w4 = w4 + a * errorDelta4
    elif(k%9 ==5):
        w5 = w5 + a * errorDelta5
    elif(k%9 ==6):
        w6 = w6 + a * errorDelta6
    elif(k%9 ==7):
        w7 = w7 + a * errorDelta7
    elif(k%9 ==8):
        w8 = w8 + a * errorDelta8   
    diff0 = abs(errorDelta0)
    diff1 = abs(errorDelta1)
    diff2 = abs(errorDelta2)
    diff3 = abs(errorDelta3)
    diff4 = abs(errorDelta4)
    diff5 = abs(errorDelta5)
    diff6 = abs(errorDelta6)
    diff7 = abs(errorDelta7)  
    diff8 = abs(errorDelta8) 
    k += 1
print("w0: ", w0)
print("W1: ", w1)  # weight
  # bias
print("w2: ", w2)
print("w3: ", w3)
print("w4: ", w4)
print("w5: ", w5)
print("w6: ", w6)
print("w7: ", w7)
print("w8: ", w8)    
pp=0
for test in x_test:
    pp+=1
y_pred = []
ll=0
while (ll < pp):
    pred = w0 + w1 * x_test[ll]+ w2 * z_test[ll]+ w3 * aa_test[ll]+ w4 * bb_test[ll]+ w5 * cc_test[ll]+ w6 * dd_test[ll]+ w7 * ee_test[ll]+ w8 * ff_test[ll]
    y_pred.append(pred)
    ll+=1
ppp=0
for tst in x_train:
    ppp+=1
y_pred1=[]
lll=0
while (lll<ppp):
    pred1 = w0 + w1 * x_train[lll]+ w2 * z_train[lll]+ w3 * aa_train[lll]+ w4 * bb_train[lll]+ w5 * cc_train[lll]+ w6 * dd_train[lll]+ w7 * ee_train[lll]+ w8 * ff_train[lll]
    y_pred1.append(pred1)
    lll+=1

# print the weight, the bias and the accuracy
print("Train Accuracy: ", r2_score(y_train1, y_pred1)) # accuracy"""
print("Train MSE: ", mean_squared_error(y_train1, y_pred1))
print("Test Accuracy: ", r2_score(y_test, y_pred)) # accuracy"""
print("Test MSE: ", mean_squared_error(y_test, y_pred))
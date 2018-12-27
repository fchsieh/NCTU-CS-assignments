# -*- coding: utf-8 -*-

import numpy as np
import pandas as pd
import random
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import r2_score

data = pd.read_csv("./Concrete_Data.csv")
output = data[data.columns.
              values[-1]]  # y, Concrete compressive strength(MPa, megapascals)
x_train, x_test, y_train, y_test = train_test_split(
    data["Cement (component 1)(kg in a m^3 mixture)"],
    output,
    test_size=0.2,
    random_state=None)
x_train = x_train.values.reshape(-1, 1)
x_test = x_test.values.reshape(-1, 1)
y_train = y_train.values.reshape(-1, 1)
n = 0
for i in x_train:
    n += 1
a = 0.000000025 # learning rate
precision = 0.001 # a small value
w0 = random.uniform(10, 15) # initial bias
w1 = random.uniform(0, 0.2) # initial weight
max_iter = 1000 # maximum iterations
diff0 = 1.0
diff1 = 1.0
k = 0

# gradient descent
while k < max_iter and (diff0 > precision or diff1 > precision):
    i = 0
    errorDelta0 = 0.0
    errorDelta1 = 0.0
    while i < n:
        errorDelta0 += (y_train[i][0] - (w0 + (w1 * x_train[i][0])))
        errorDelta1 += ((y_train[i][0] - (w0 + (w1 * x_train[i][0]))) * x_train[i][0])
        i += 1
    w0 = w0 + a * errorDelta0
    w1 = w1 + a * errorDelta1
    diff0 = abs(errorDelta0)
    diff1 = abs(errorDelta1)
    k += 1
    
# draw and save the picture
fig, ax = plt.subplots()
ax.scatter(x_test, y_test, color="black")
y_pred = []
for test in x_test:
    pred = w0 + w1 * test[0]
    y_pred.append(pred)
ax.plot(x_test, y_pred, color="blue", linewidth=1)
plt.xlabel("Cement (component 1)(kg in a m^3 mixture)")
plt.ylabel(data.columns.values[-1])
plt.xlim(0, 600)
plt.ylim(0, 100)
fig.savefig("p2")

# print the weight, the bias and the accuracy
print("Weight: ", w1)  # weight
print("Bias: ", w0)  # bias
print("Accuracy: ", r2_score(y_test, y_pred)) # accuracy

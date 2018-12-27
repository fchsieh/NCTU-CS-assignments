# coding: utf-8

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import r2_score
import seaborn as sns

data = pd.read_csv("./Concrete_Data.csv")

features_header = list(data.columns.values[:-1])  # x
output = data[data.columns.
              values[-1]]  # y, Concrete compressive strength(MPa, megapascals)

for feature in features_header:
    fig, ax = plt.subplots(figsize=(10, 7))
    plt.xlabel(feature)
    plt.ylabel(data.columns.values[-1])  # concrete compressive strength
    ax.scatter(data[feature], output, s=5)
    fig.savefig(feature)

# Show the correlation between two variables
corr = data.corr()
corr_map = sns.heatmap(
    corr,
    xticklabels=corr.columns.values,
    yticklabels=corr.columns.values,
    annot=True)
fig = corr_map.get_figure()
fig.savefig("corr")

# Showing that we should choose Cement as the feature
# We can also do this by observing figures we created
"""
for feature in features_header:
    x_train, x_test, y_train, y_test = train_test_split(
        data[feature], output, test_size=0.2, random_state=None)
    x_train = x_train.values.reshape(-1, 1)
    x_test = x_test.values.reshape(-1, 1)
    lm = LinearRegression()
    lm.fit(x_train, y_train)
    y_pred = lm.predict(x_test)
    print(feature, r2_score(y_test, y_pred))
"""
# Print the weight and the bias
x_train, x_test, y_train, y_test = train_test_split(
    data["Cement (component 1)(kg in a m^3 mixture)"],
    output,
    test_size=0.2,
    random_state=None)
x_train = x_train.values.reshape(-1, 1)
x_test = x_test.values.reshape(-1, 1)
lm = LinearRegression()
lm.fit(x_train, y_train)
y_pred = lm.predict(x_test)
fig, ax = plt.subplots()
ax.scatter(x_test, y_test, color="black")
ax.plot(x_test, y_pred, color="blue", linewidth=1)
plt.xlabel("Cement (component 1)(kg in a m^3 mixture)")
plt.ylabel(data.columns.values[-1])
plt.xlim(0, 600)
plt.ylim(0, 100)
fig.savefig("p1")
print("Weight: ", lm.coef_)  # weight
print("Bias: ", lm.intercept_)  # bias
print("Accuracy: ", r2_score(y_test, y_pred))

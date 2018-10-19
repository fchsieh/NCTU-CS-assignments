# -*- coding: utf-8 -*-
"""
Created on Tue Oct 16 19:09:52 2018

@author: k8567
"""
import math
import pandas as pd
import numpy as np
from sklearn.metrics import confusion_matrix
import os
import pydotplus
from IPython.display import Image
os.environ['PATH'] = os.environ['PATH'] + (';c:\\Program Files (x86)\\Graphviz2.38\\bin\\')
from sklearn import tree
from sklearn.tree import DecisionTreeRegressor
from sklearn.model_selection import train_test_split
from sklearn.tree import export_graphviz
review_data = pd.read_csv("./preprocessed_googleplaystore.csv", usecols=["Price", "Rating", "Reviews", "Installs"])
review_data = review_data.sample(frac=1).reset_index(drop=True)#shuffle
all_feature = ["Price", "Installs", "Reviews"]

confusion_matrix_rating = np.zeros((5, 5), dtype=np.int)
def chunks(arr, m):
    n = int(math.ceil(len(arr) / float(m)))
    return [arr[i:i + n] for i in range(0, len(arr), n)]

    
#resub valdation
K = 1
divided_data = chunks(review_data, K)

for i in range(K):
    ########################### choosing train_data and test_data
    x_train = review_data[["Price", "Installs", "Reviews"]]
    y_train = review_data["Rating"]
    x_test = x_train
    y_test = y_train
    ###########################train
    clf = DecisionTreeRegressor(criterion="mse", max_depth=4, random_state=None)
    clf.fit(x_train, y_train)
    pred = clf.predict(x_test)
    ###########################classify
    lth = len(pred)
    pred_class = strs = ["" for x in range(lth)]
    y_test_class = ["" for x in range(lth)]
    
    y_test_list = y_test.tolist()
    for ind in range(lth):

        if(pred[ind]<=3):
            pred_class[ind] = "0"
        elif(pred[ind]<=3.5):
            pred_class[ind] = "1"
        elif(pred[ind]<=4):
            pred_class[ind] = "2"
        elif(pred[ind]<=4.5):
            pred_class[ind] = "3"
        elif(pred[ind]<=5):
            pred_class[ind] = "4"
        if(y_test_list[ind]<=3):
            y_test_class[ind] = "0"
        elif(y_test_list[ind]<=3.5):
            y_test_class[ind] = "1"
        elif(y_test_list[ind]<=4):
            y_test_class[ind] = "2"
        elif(y_test_list[ind]<=4.5):
            y_test_class[ind] = "3"
        elif(y_test_list[ind]<=5):
            y_test_class[ind] = "4"
    ########################### build confusion_matrix
    arr = confusion_matrix(pred_class, y_test_class, labels=['0', '1', '2', '3', '4'])
    confusion_matrix_rating =  confusion_matrix_rating + arr

###########################
print("\n","class =",["<=3","<=3.5","<=4","<=4.5","<=5"],"\n")
print(confusion_matrix_rating)
precision = np.zeros(5)
recall = np.zeros(5)
accurate = 0
for i in range(5):
    precision[i] = confusion_matrix_rating[i][i] / sum(confusion_matrix_rating[i])
    recall[i] = confusion_matrix_rating[i][i] / sum([row[i] for row in confusion_matrix_rating])
    accurate += confusion_matrix_rating[i][i]

accuracy = accurate/sum(sum(confusion_matrix_rating))

print("precision =", precision)
print("recall =", recall)
print("accuracy =", accuracy)

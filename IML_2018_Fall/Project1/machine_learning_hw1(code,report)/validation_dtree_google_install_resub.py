# -*- coding: utf-8 -*-
"""
Created on Tue Oct 16 13:12:17 2018

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
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.tree import export_graphviz
review_data = pd.read_csv("./preprocessed_googleplaystore.csv", usecols=["Price", "Rating", "Reviews", "Installs"])
review_data = review_data.sample(frac=1).reset_index(drop=True)#shuffle

confusion_matrix_install = np.zeros((6, 6), dtype=np.int)
def chunks(arr, m):
    n = int(math.ceil(len(arr) / float(m)))
    return [arr[i:i + n] for i in range(0, len(arr), n)]
    
#resub valdation
K = 1
divided_data = chunks(review_data, K)
for i in range(K):
    ########################### choosing train_data and test_data
    x_train = review_data[["Price", "Rating", "Reviews"]]
    y_train = review_data["Installs"]
    x_test = x_train
    y_test = y_train
    ###########################train
    clf = DecisionTreeClassifier(criterion = 'entropy', max_depth =4, random_state = 0)
    clf.fit(x_train, y_train)
    pred = clf.predict(x_test)
    ###########################classify
    lth = len(pred)
    pred_class = strs = ["" for x in range(lth)]
    y_test_class = ["" for x in range(lth)]
    y_test_list = y_test.tolist()
    for ind in range(lth):

        if(pred[ind]<=1000):
            pred_class[ind] = "0"
        elif(pred[ind]<=10000):
            pred_class[ind] = "1"
        elif(pred[ind]<=100000):
            pred_class[ind] = "2"
        elif(pred[ind]<=1000000):
            pred_class[ind] = "3"
        elif(pred[ind]<=10000000):
            pred_class[ind] = "4"
        else:
            pred_class[ind] = "5"
        if(y_test_list[ind]<=1000):
            y_test_class[ind] = "0"
        elif(y_test_list[ind]<=10000):
            y_test_class[ind] = "1"
        elif(y_test_list[ind]<=100000):
            y_test_class[ind] = "2"
        elif(y_test_list[ind]<=1000000):
            y_test_class[ind] = "3"
        elif(y_test_list[ind]<=10000000):
            y_test_class[ind] = "4"
        else:
            y_test_class[ind] = "5"
    ########################### build confusion_matrix
    arr = confusion_matrix(pred_class, y_test_class, labels=['0', '1', '2', '3', '4', '5'])
    confusion_matrix_install =  confusion_matrix_install + arr
###########################
print("\n","class =",["<=1000","<=10000","<=100000","<=1000000","<=10000000","other"],"\n")
print(confusion_matrix_install)
precision = np.zeros(6)
recall = np.zeros(6)
accurate = 0
for i in range(6):
    precision[i] = confusion_matrix_install[i][i] / sum(confusion_matrix_install[i])
    recall[i] = confusion_matrix_install[i][i] / sum([row[i] for row in confusion_matrix_install])
    accurate += confusion_matrix_install[i][i]

accuracy = accurate/sum(sum(confusion_matrix_install))

print("precision =", precision)
print("recall =", recall)
print("accuracy =", accuracy)

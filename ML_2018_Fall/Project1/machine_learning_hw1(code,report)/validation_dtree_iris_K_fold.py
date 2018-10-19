# -*- coding: utf-8 -*-
"""
Created on Mon Oct 15 13:13:05 2018

@author: k8567
"""

import math
import numpy as np

from sklearn.metrics import confusion_matrix
import pandas 
import os
os.environ['PATH'] = os.environ['PATH'] + (';c:\\Program Files (x86)\\Graphviz2.38\\bin\\')
from sklearn import tree
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.tree import export_graphviz
import pydotplus
from IPython.display import Image
iris_data = pandas.read_csv("preprocessed_iris.csv")
mapping = {"Iris-setosa": "0", "Iris-versicolor": "1", "Iris-virginica": "2"}
iris = iris_data.replace(mapping)
iris = iris.sample(frac=1).reset_index(drop=True)#shuffle


confusion_matrix_iris = np.zeros((3, 3), dtype=np.int)
def chunks(arr, m):
    n = int(math.ceil(len(arr) / float(m)))
    return [arr[i:i + n] for i in range(0, len(arr), n)]
    
#K-fold valdation
K = 10
divided_data = chunks(iris, K)
for i in range(K):
    ########################### choosing train_data and test_data
    folded_data = iris[0:0]
    for j in range(K):
        if(i!=j):
            folded_data = folded_data.append(divided_data[j])
    
    x_train = folded_data[["sepal length", "sepal width", "petal length", "petal width"]]
    y_train = folded_data["class"]
    x_test = divided_data[i][["sepal length", "sepal width", "petal length", "petal width"]]
    y_test = divided_data[i]["class"]
    ###########################train
    clf = DecisionTreeClassifier(criterion = 'entropy', max_depth =3, random_state = 0)
    clf.fit(x_train, y_train)
    pred = clf.predict(x_test)
    ########################### build confusion_matrix
    arr = confusion_matrix(pred, y_test, labels=['0', '1', '2'])
    confusion_matrix_iris = confusion_matrix_iris + arr

###########################
print("\n","class =",["Iris-setosa", "Iris-versicolor", "Iris-virginica"],"\n")
print(confusion_matrix_iris)
precision = np.zeros(3)
recall = np.zeros(3)
accurate = 0
for i in range(0, 3):
    precision[i] = confusion_matrix_iris[i][i] / sum(confusion_matrix_iris[i])
    recall[i] = confusion_matrix_iris[i][i] / sum([row[i] for row in confusion_matrix_iris])
    accurate += confusion_matrix_iris[i][i]

accuracy = accurate/sum(sum(confusion_matrix_iris))

print("precision =", precision)
print("recall =", recall)
print("accuracy =", accuracy)


# -*- coding: utf-8 -*-
"""
Created on Mon Oct 15 13:13:05 2018

@author: k8567
"""
#import numpy 
import pandas 
import os
os.environ['PATH'] = os.environ['PATH'] + (';c:\\Program Files (x86)\\Graphviz2.38\\bin\\')
#from numpy import genfromtxt, zeros
from sklearn import tree
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.tree import export_graphviz
import pydotplus
from IPython.display import Image
iris_data = pandas.read_csv("preprocessed_iris.csv")
mapping = {"Iris-setosa": "0", "Iris-versicolor": "1", "Iris-virginica": "2"}
iris = iris_data.replace(mapping)

x = iris[["sepal length", "sepal width", "petal length", "petal width"]]
y = iris["class"]
#x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.3, random_state=None)
clf = DecisionTreeClassifier(criterion = 'entropy', max_depth =3, random_state = 0)
clf.fit(x, y)
#clf.predict(x_test)
#y_test.values
#print(clf.score(x_test, y_test))
dot_data=export_graphviz(clf, out_file = None, feature_names = ["sepal length", "sepal width", "petal length", "petal width"], filled=True, rounded=True,  
                         special_characters=True)
graph = pydotplus.graph_from_dot_data(dot_data)  
graph.write_pdf("iris_.pdf")

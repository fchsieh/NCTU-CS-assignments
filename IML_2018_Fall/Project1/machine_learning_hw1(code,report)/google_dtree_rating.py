# -*- coding: utf-8 -*-
"""
Created on Tue Oct 16 19:09:52 2018

@author: k8567
"""
import pandas 
import numpy
import os
import pydotplus
from IPython.display import Image
os.environ['PATH'] = os.environ['PATH'] + (';c:\\Program Files (x86)\\Graphviz2.38\\bin\\')
from sklearn import tree
from sklearn.tree import DecisionTreeRegressor
from sklearn.model_selection import train_test_split
from sklearn.tree import export_graphviz
review_data = pandas.read_csv("./preprocessed_googleplaystore.csv", usecols=["Price", "Rating", "Reviews", "Installs"])
numpy.isnan(review_data).any()
review_data.dropna(inplace=True)
all_feature = ["Price", "Installs", "Reviews"]
x = review_data[["Price", "Installs", "Reviews"]]
y = review_data["Rating"]
#x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.3, random_state=None)
clf = DecisionTreeRegressor(criterion="mse", max_depth=4, random_state=None)
clf.fit(x, y)
#print(x)

#print(clf.score(x, y))
#export_graphviz(clf, out_file = 'AAPPDtree.dot', feature_names = ["Price", "Rating", "Reviews"])
dot_data=export_graphviz(clf, out_file = None, feature_names = ["Price", "Installs", "Reviews"], filled=True, rounded=True,  
                         special_characters=True)
graph = pydotplus.graph_from_dot_data(dot_data)  
graph.write_pdf("Google_play_store_(Rating).pdf")


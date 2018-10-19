# -*- coding: utf-8 -*-
"""
Created on Tue Oct 16 13:12:17 2018

@author: k8567
"""

import pandas 
import numpy
import os
import pydotplus
from IPython.display import Image
os.environ['PATH'] = os.environ['PATH'] + (';c:\\Program Files (x86)\\Graphviz2.38\\bin\\')
from sklearn import tree
from sklearn.tree import DecisionTreeClassifier
from sklearn.model_selection import train_test_split
from sklearn.tree import export_graphviz
review_data = pandas.read_csv("./preprocessed_googleplaystore.csv", usecols=["Price", "Rating", "Reviews", "Installs"])
numpy.isnan(review_data).any()
review_data.dropna(inplace=True)
x = review_data[["Price", "Rating", "Reviews"]]
y = review_data["Installs"]
#x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.3, random_state=None)
clf = DecisionTreeClassifier(criterion = 'entropy', max_depth =4, random_state = 0)
clf.fit(x, y)
#print(x)

#print(clf.score(x, y))
#export_graphviz(clf, out_file = 'AAPPDtree.dot', feature_names = ["Price", "Rating", "Reviews"])
dot_data=export_graphviz(clf, out_file = None, feature_names = ["Price", "Rating", "Reviews"], filled=True, rounded=True,  
                         special_characters=True)
graph = pydotplus.graph_from_dot_data(dot_data)  
graph.write_pdf("Google_play_store_(Installs) .pdf")

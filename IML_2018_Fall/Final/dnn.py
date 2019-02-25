# -*- coding: utf-8 -*-
from keras.preprocessing.image import ImageDataGenerator, array_to_img, img_to_array, load_img
import numpy as np
from keras.models import Sequential
from keras.layers.core import Dense, Dropout, Activation
from keras.layers import Conv2D, MaxPooling2D, Flatten
from keras.optimizers import SGD, Adam
from keras.utils import np_utils
import os
import random
import matplotlib.pylab as plt
from glob import glob
import pandas as pd
from sklearn.model_selection import train_test_split
import piexif
#0:bike
#1:bus
#2:car
#3:truck
#x_train, y_train
x_train = []
y_train = []
#insert bike
r = os.path.abspath(os.path.join('data', 'train', 'bike'))
path = glob(os.path.join(r, '*.jpg'))
for p in path:
    try:
        piexif.remove(p)
    except:
        pass
    x_train.append(img_to_array(load_img(p, target_size=(250, 250))))
    y_train.append([0])
#insert bus
r = os.path.abspath(os.path.join('data', 'train', 'bus'))
path = glob(os.path.join(r, '*.jpg'))
for p in path:
    try:
        piexif.remove(p)
    except:
        pass
    x_train.append(img_to_array(load_img(p, target_size=(250, 250))))
    y_train.append([1])
#insert car
r = os.path.abspath(os.path.join('data', 'train', 'car'))
path = glob(os.path.join(r, '*.jpg'))
for p in path:
    try:
        piexif.remove(p)
    except:
        pass
    x_train.append(img_to_array(load_img(p, target_size=(250, 250))))
    y_train.append([2])
#insert truck
r = os.path.abspath(os.path.join('data', 'train', 'truck'))
path = glob(os.path.join(r, '*.jpg'))
for p in path:
    try:
        piexif.remove(p)
    except:
        pass
    x_train.append(img_to_array(load_img(p, target_size=(250, 250))))
    y_train.append([3])
#adjust the training data
x_train = np.asarray(x_train)
x_train = x_train.astype("float32")
x_train = x_train / 255
x_train = np.reshape(x_train, (320, -1))
y_train = np.asarray(y_train)
y_train = np_utils.to_categorical(y_train, num_classes=4)


#x_test, y_test
x_test = []
y_test = []
#insert bike
r = os.path.abspath(os.path.join('data', 'test', 'bike'))
path = glob(os.path.join(r, '*.jpg'))
for p in path:
    try:
        piexif.remove(p)
    except:
        pass
    x_test.append(img_to_array(load_img(p, target_size=(250, 250))))
    y_test.append([0])
#insert bus
r = os.path.abspath(os.path.join('data', 'test', 'bus'))
path = glob(os.path.join(r, '*.jpg'))
for p in path:
    try:
        piexif.remove(p)
    except:
        pass
    x_test.append(img_to_array(load_img(p, target_size=(250, 250))))
    y_test.append([1])
#insert car
r = os.path.abspath(os.path.join('data', 'test', 'car'))
path = glob(os.path.join(r, '*.jpg'))
for p in path:
    try:
        piexif.remove(p)
    except:
        pass
    x_test.append(img_to_array(load_img(p, target_size=(250, 250))))
    y_test.append([2])
#insert truck
r = os.path.abspath(os.path.join('data', 'test', 'truck'))
path = glob(os.path.join(r, '*.jpg'))
for p in path:
    try:
        piexif.remove(p)
    except:
        pass
    x_test.append(img_to_array(load_img(p, target_size=(250, 250))))
    y_test.append([3])
#adjust the testing data
x_test = np.asarray(x_test)
x_test = x_test.astype("float32")
x_test = x_test / 255
x_test = np.reshape(x_test, (80, -1))
y_test = np.asarray(y_test)
y_test = np_utils.to_categorical(y_test, num_classes=4)

model = Sequential()
model.add(Dense(512, input_dim=250*250*3, activation='relu'))
model.add(Dense(512, activation='relu'))
model.add(Dense(512, activation='relu'))
model.add(Dense(1024, activation='relu'))
model.add(Dense(1024, activation='relu'))
model.add(Dense(4, activation='softmax'))
model.compile(loss='categorical_crossentropy', optimizer='sgd', metrics=['accuracy'])
model.fit(x_train, y_train, batch_size=10, epochs=20)

score = model.evaluate(x_train, y_train, batch_size=10)
print('\nTotal loss on Training Set:', score[0])
print('\nAccuracy of Training Set:', score[1])
result = model.predict(x_train)
print('\nPrediction on Training Set:\n', result)

score = model.evaluate(x_test, y_test, batch_size=10)
print('\nTotal loss on Testing Set:', score[0])
print('\nAccuracy of Testing Set:', score[1])
result = model.predict(x_test)
print('\nPrediction on Testing Set:\n', result)

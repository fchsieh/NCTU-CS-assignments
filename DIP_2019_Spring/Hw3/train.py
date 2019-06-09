from __future__ import division

import glob
import json
import math
import os
import random
import sys
from datetime import timedelta
from time import localtime, strftime, time

import keras.backend as K
import numpy as np
import scipy.io as sio
from keras import applications, optimizers
from keras.callbacks import LearningRateScheduler, ModelCheckpoint
from keras.layers import (Activation, AveragePooling1D, Conv1D, Convolution1D, Conv2D,
                          Dense, Dropout, Flatten, Input, MaxPooling1D, merge, MaxPooling2D, BatchNormalization)
from keras.layers.pooling import GlobalAveragePooling1D
from keras.models import Model, Sequential, load_model
from keras.preprocessing.image import ImageDataGenerator
from PIL import Image
from skimage import color, io
from skimage.segmentation import mark_boundaries, slic
from skimage.util import img_as_float

### git@yash0307 ###
import tensorflow as tf
from tensorflow import keras


class TimeEval:
    """
    Needed modules:
    from datetime import timedelta
    from time import localtime, strftime, time
    """

    def __init__(self):
        self.start = time()

    def secondsToStr(self):
        return strftime("%Y-%m-%d %H:%M:%S", localtime())

    def log(self, s="", display_elapsed=False):
        print("=" * 50)
        print(self.secondsToStr(), "-", s)
        if display_elapsed is True:
            elapsed = time() - self.start
            print("Elapsed time:", str(timedelta(seconds=elapsed)))
        print("=" * 50)

    def endlog(self):
        print("=" * 50)
        elapsed = time() - self.start
        print("Total elapsed:", str(timedelta(seconds=elapsed)))
        print("=" * 50 + "\n")
        return str(timedelta(seconds=elapsed))


def initialize_net(train_params):
    act = "relu"
    model = Sequential()
    # Layer 1
    model.add(Conv1D(nb_filter=5,
                     filter_length=10,
                     init='glorot_uniform',
                     border_mode='same',
                     input_shape=(train_params['max_size'], 3),
                     bias=True))
    model.add(Activation(act))
    model.add(MaxPooling1D(pool_size=2))

    # Layer 2
    model.add(Conv1D(nb_filter=10,
                     filter_length=20,
                     init='glorot_uniform',
                     border_mode='same',
                     bias=True))
    model.add(Activation(act))
    model.add(MaxPooling1D(pool_size=2))

    # Layer 3
    model.add(Conv1D(nb_filter=20,
                     filter_length=20,
                     init='glorot_uniform',
                     border_mode='same',
                     bias=True))
    model.add(Activation(act))
    model.add(MaxPooling1D(pool_size=2))

    model.add(GlobalAveragePooling1D(input_shape=model.output_shape[1:]))

    model.add(Dense(input_dim=20,
                    output_dim=2,
                    init='glorot_uniform'))

    model.add(Activation(act))

    model.add(Dropout(0.3))

    model.add(Dense(input_dim=2,
                    output_dim=2,
                    init='glorot_uniform'))

    model.add(Activation('softmax'))

    return model


def initialize_params(train_data, data):
    print('neg_samples', len(data[0]))
    print('pos_samples', len(data[1]))
    train_params = {'batch_size': 256,
                    'max_size': 256,
                    'base_lr': 0.001,
                    'decay_steps': 5,
                    'decay_factor': 0.5,
                    'num_epochs': 5,
                    'neg_samples': len(data[0]),
                    'pos_samples': len(data[1]),
                    'total_samples': len(data[0])+len(data[1]),
                    'checkpoint': 1}

    return train_params


def get_train_data(train_data, train_labels):
    data = {1: [], 0: []}
    num_images = train_data.shape[1]
    for i in range(0, num_images):
        given_image_sp = train_data[0][i]
        # print('given_image_sp',len(given_image_sp))
        given_image_lb = train_labels[i][0]
        # print('given_image_lb',len(given_image_lb))
        num_sp = given_image_lb.shape[1]
        for j in range(0, num_sp):
            given_label = given_image_lb[0][j]
            given_rep = np.asarray(given_image_sp[j][:], dtype='float')
            #print('given_rep: ',given_rep)
            if given_label == 0:
                #given_rep = np.asarray(given_image_sp[j][:], dtype='float')
                #print('given_label == 0')
                data[0].append(given_rep)
            elif given_label == 1:
                #given_rep = np.asarray(given_image_sp[j][:], dtype='float')
                #print('given_label == 1')
                data[1].append(given_rep)
            else:
                print('SOMETHING IS WRONG !')
    return data


def load_data(data, train_params):
    data_frac = 0.5
    X_temp = np.zeros(
        (train_params['batch_size'], train_params['max_size'], 3))
    Y_temp = np.zeros((train_params['batch_size'], 2))
    #print('population: ', range(0,train_params['pos_samples']))
    #print('sample: ',int(train_params['batch_size']*data_frac+2))
    idx = random.sample(range(0, train_params['pos_samples']), int(
        train_params['batch_size']*data_frac+2))
    for i in range(0, int(train_params['batch_size']*data_frac)):
        Y_temp[i][1] = float(1)
        sam = data[1][idx[i]]
        sam_len = sam.shape[0]
        X_temp[i, :sam_len, :] = np.true_divide(sam, sam.max())
    idx = random.sample(range(0, train_params['neg_samples']), int(
        train_params['batch_size']-(train_params['batch_size']*data_frac)+2))
    for i in range(int(train_params['batch_size']*data_frac), train_params['batch_size']):
        Y_temp[i][0] = float(1)
        sam = data[0][idx[i-int(train_params['batch_size']*data_frac)]]
        sam_len = sam.shape[0]
        X_temp[i, :sam_len, :] = np.true_divide(sam, sam.max())
        X = np.zeros((train_params['batch_size'], train_params['max_size'], 3))
        Y = np.zeros((train_params['batch_size'], 2))
    perm_idx = np.random.permutation(train_params['batch_size'])
    for i in range(0, train_params['batch_size']):
        X[i, :, :] = X_temp[perm_idx[i], :, :]
        Y[i, :] = Y_temp[perm_idx[i], :]
    return (X, Y)


def readSLICandMDLInit(resultFile, all_Q_mat, superpixel_label_mat):
    print('----write result, read mats')
    f_out = open(resultFile, 'w')
    train_data = sio.loadmat(all_Q_mat)['all_Q']
    train_labels = sio.loadmat(superpixel_label_mat)['all_superpixel_labels']
    print('----get_train_data')
    data = get_train_data(train_data, train_labels)
    print(len(data))
    print('----initialize_params')
    train_params = initialize_params(train_data, data)
    print('----initialize_net')
    model = initialize_net(train_params)
    model.summary()
    print('----model compile')
    model.compile(loss='categorical_crossentropy',
                  optimizer=optimizers.Adam(lr=train_params['base_lr']),
                  metrics=['accuracy'])
    print('----ImageDataGenerator')
    train_datagen = ImageDataGenerator(
        featurewise_center=True,
        featurewise_std_normalization=True)

    for epoch in range(0, train_params['num_epochs']):
        num_iterations = int(
            train_params['total_samples']/train_params['batch_size']) + 1
        for iteration in range(0, num_iterations):
            print('Epoch : ' + str(epoch) + ' | Iteration : ' + str(iteration))
            given_data = load_data(data, train_params)
            X = given_data[0]
            #X = np.resize(X, (1, 256, 256, 3))
            Y = given_data[1]
            model.fit(X, Y,
                      epochs=1,
                      verbose=1)
        if epoch % train_params['decay_steps'] == 0 and epoch != 0:
            print(' Changing learning rate ... ')
            lr = K.get_value(model.optimizer.lr)
            K.set_value(model.optimizer.lr, lr*train_params['decay_factor'])
            print("lr changed to {}".format(lr*train_params['decay_factor']))
        if epoch % train_params['checkpoint'] == 0 and epoch != 0:
            print(' Saving model ... ')
            model_name = 'model_' + str(epoch) + '.h5'
            model.save(model_name)
        if epoch % 1 == 0:
            acu_pos = 0
            acu_neg = 0
            acu = 0
            for i in range(0, int(train_params['pos_samples']/train_params['batch_size'])):
                X = np.zeros(
                    (train_params['batch_size'], train_params['max_size'], 3))
                Y = np.zeros((train_params['batch_size'], 2))
                for j in range(0, train_params['batch_size']):
                    sam = data[1][i*train_params['batch_size'] + j]
                    sam_len = sam.shape[0]
                    X[j, :sam_len, :] = np.true_divide(sam, sam.max())
                    Y[j][1] = float(1)
                pred = model.evaluate(X, Y,
                                      batch_size=train_params['batch_size'])
                print(pred)
                acu_pos = acu_pos + pred[1]
                acu = acu + pred[1]
            for i in range(0, int(train_params['neg_samples']/train_params['batch_size'])):
                X = np.zeros(
                    (train_params['batch_size'], train_params['max_size'], 3))
                Y = np.zeros((train_params['batch_size'], 2))
                for j in range(0, train_params['batch_size']):
                    sam = data[0][i*train_params['batch_size'] + j]
                    sam_len = sam.shape[0]
                    X[j, :sam_len, :] = np.true_divide(sam, sam.max())
                    Y[j][0] = float(1)
                pred = model.evaluate(X, Y,
                                      batch_size=train_params['batch_size'])
                print(pred)
                acu_neg = acu_neg + pred[1]
                acu = acu + pred[1]
            acu_pos = float(
                acu_pos)/float(int(train_params['pos_samples']/train_params['batch_size']))
            acu_neg = float(
                acu_neg)/float(int(train_params['neg_samples']/train_params['batch_size']))
            acu = float(acu)/float(int(train_params['pos_samples']/train_params['batch_size']) + int(
                train_params['neg_samples']/train_params['batch_size']))
            f_out.write('acu_pos: ' + str(acu_pos)+', acu_neg: ' +
                        str(acu_neg)+', acu:'+str(acu)+'\n')


if __name__ == '__main__':
    K.clear_session()

    # training
    train_image_mat = '../train/all_Q.mat'
    train_mask_mat = '../train/all_superpixel_labels.mat'

    # start training
    start = TimeEval()
    start.log("Start training")
    readSLICandMDLInit('train_result.txt', train_image_mat, train_mask_mat)
    elapsed = start.endlog()
    with open("elapsed.txt", "w") as f:
        f.write(elapsed)

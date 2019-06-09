from __future__ import division

### git@yash0307 ###
import tensorflow as tf
from tensorflow import keras

from skimage.segmentation import slic
from skimage.segmentation import mark_boundaries
from skimage.util import img_as_float
from skimage import io, color

from keras import applications
from keras.preprocessing.image import ImageDataGenerator
from keras import optimizers
from keras.models import Sequential, Model, load_model
from keras.layers import Dropout, Flatten, Dense, merge, Activation, Conv1D, Input, MaxPooling1D, Convolution1D
from keras.layers.pooling import GlobalAveragePooling1D
from keras.callbacks import LearningRateScheduler
from keras.callbacks import ModelCheckpoint
import keras.backend as K
'''
from tensorflow.python.keras.layers import Dropout, Flatten, Dense, Activation, Conv1D, Input, MaxPooling1D, Convolution1D
from tensorflow.python.keras import activations
from tensorflow.python.keras import backend as K
'''
import numpy as np
import math
import json
import sys
import random
import scipy.io as sio
import os
from PIL import Image
import glob

# print('tensorflow Ver: ' + tf.VERSION)
# print('Keras Ver: ' +tf.keras.__version__)

def initialize_net(train_params):
    '''
    #tensorflow keras modification
    model = tf.keras.Sequential()
    
    model.add(Conv1D(nb_filter=5, 
                filter_length=10, 
                init='glorot_uniform', 
                border_mode='same', 
                input_shape=(train_params['max_size'], 3), 
                bias=True))
    
    model.add(tf.keras.layers.Activation('tanh'))
    
    model.add(MaxPooling1D(pool_size=2))
    
    model.add(Conv1D(nb_filter=10,
                filter_length=20, 
                init='glorot_uniform', 
                border_mode='same', 
                bias=True))
                
    model.add(tf.keras.layers.Activation('tanh'))
    
    model.add(MaxPooling1D(pool_size=2))
    
    model.add(Conv1D(nb_filter=20,
                filter_length=20,
                init='glorot_uniform',
                border_mode='same',
                bias=True))
    model.add(tf.keras.layers.Activation('tanh'))
    
    model.add(MaxPooling1D(pool_size=2))
    
    model.add(GlobalAveragePooling1D(input_shape=model.output_shape[1:]))
    
    model.add(Dense(input_dim=20, 
            output_dim=2,
            init='glorot_uniform'))
            
    model.add(tf.keras.layers.Activation('tanh'))
    
    model.add(Dropout(0.3))
    
    model.add(Dense(input_dim=2, 
            output_dim=2, 
            init='glorot_uniform'))
            
    model.add(tf.keras.layers.Activation('softmax'))
    '''     
    model = Sequential()    
    model.add(Conv1D(nb_filter=5, 
                filter_length=10, 
                init='glorot_uniform', 
                border_mode='same', 
                input_shape=(train_params['max_size'], 3), 
                bias=True))
    
    model.add(Activation('tanh'))
    
    model.add(MaxPooling1D(pool_size=2))
    model.add(Conv1D(nb_filter=10,
                filter_length=20, 
                init='glorot_uniform', 
                border_mode='same', 
                bias=True))     
    model.add(Activation('tanh'))
    model.add(MaxPooling1D(pool_size=2))
    
    model.add(Conv1D(nb_filter=20,
                filter_length=20,
                init='glorot_uniform',
                border_mode='same',
                bias=True))
    model.add(Activation('tanh'))
    model.add(MaxPooling1D(pool_size=2))
    model.add(GlobalAveragePooling1D(input_shape=model.output_shape[1:]))
    
    model.add(Dense(input_dim=20, 
            output_dim=2,
            init='glorot_uniform'))
    model.add(Activation('tanh'))
    model.add(Dropout(0.3))
    
    model.add(Dense(input_dim=2, 
            output_dim=2, 
            init='glorot_uniform'))
    model.add(Activation('softmax'))

    return model

def initialize_params(train_data, data):
    print('neg_samples', len(data[0]))
    print('pos_samples', len(data[1]))    
    train_params = {'batch_size':256, 
            'max_size':256, 
            'base_lr':0.001, 
            'decay_steps':5,
            'decay_factor':0.5, 
            'num_epochs':15, 
            'neg_samples':len(data[0]), 
            'pos_samples':len(data[1]), 
            'total_samples':len(data[0])+len(data[1]), 
            'checkpoint':1}

    return train_params

def get_train_data(train_data, train_labels):
    data = {1:[], 0:[]}
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
    X_temp = np.zeros((train_params['batch_size'], train_params['max_size'], 3))
    Y_temp = np.zeros((train_params['batch_size'], 2))
    #print('population: ', range(0,train_params['pos_samples']))
    #print('sample: ',int(train_params['batch_size']*data_frac+2))
    idx = random.sample(range(0,train_params['pos_samples']), int(train_params['batch_size']*data_frac+2))
    for i in range(0, int(train_params['batch_size']*data_frac)):
        Y_temp[i][1] = float(1)
        sam = data[1][idx[i]]
        sam_len = sam.shape[0]
        X_temp[i, :sam_len, :] = np.true_divide(sam, sam.max())
    idx = random.sample(range(0, train_params['neg_samples']), int(train_params['batch_size']-(train_params['batch_size']*data_frac)+2))
    for i in range(int(train_params['batch_size']*data_frac), train_params['batch_size']):
        Y_temp[i][0] = float(1)
        sam = data[0][idx[i-int(train_params['batch_size']*data_frac)]]
        sam_len = sam.shape[0]
        X_temp[i, :sam_len, :] = np.true_divide(sam, sam.max())
        X = np.zeros((train_params['batch_size'], train_params['max_size'], 3))
        Y = np.zeros((train_params['batch_size'], 2))
    perm_idx = np.random.permutation(train_params['batch_size'])
    for i in range(0, train_params['batch_size']):
        X[i,:,:] = X_temp[perm_idx[i],:,:]
        Y[i,:] = Y_temp[perm_idx[i],:]
    return (X,Y)
    print('----write result, read mats')
    f_out = open(resultFile,'w')
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
        num_iterations = int(train_params['total_samples']/train_params['batch_size']) + 1
        for iteration in range(0, num_iterations):
            print ('Epoch : ' + str(epoch) + ' | Iteration : ' + str(iteration))
            given_data = load_data(data, train_params)
            X = given_data[0]
            Y = given_data[1]
            model.fit(X,Y,
                epochs=1,
                verbose=1)
        if epoch%train_params['decay_steps'] == 0 and epoch != 0:
            print (' Changing learning rate ... ')
            lr = K.get_value(model.optimizer.lr)
            K.set_value(model.optimizer.lr, lr*train_params['decay_factor'])
            print("lr changed to {}".format(lr*train_params['decay_factor']))
        if epoch%train_params['checkpoint'] == 0 and epoch != 0:
            print (' Saving model ... ')
            model_name = 'model_' + str(epoch) + '.h5'
            model.save(model_name)
        if epoch%1 == 0:
            acu_pos = 0
            acu_neg = 0
            acu = 0
            for i in range(0, int(train_params['pos_samples']/train_params['batch_size'])):
                X = np.zeros((train_params['batch_size'], train_params['max_size'], 3))
                Y = np.zeros((train_params['batch_size'], 2))
                for j in range(0, train_params['batch_size']):
                    sam = data[1][i*train_params['batch_size'] + j]
                    sam_len = sam.shape[0]
                    X[j, :sam_len, :] = np.true_divide(sam, sam.max())
                    Y[j][1] = float(1)
                pred = model.evaluate(X,Y, 
                        batch_size=train_params['batch_size'])
                print(pred)
                acu_pos = acu_pos + pred[1]
                acu = acu + pred[1]
            for i in range(0, int(train_params['neg_samples']/train_params['batch_size'])):
                X = np.zeros((train_params['batch_size'], train_params['max_size'], 3))
                Y = np.zeros((train_params['batch_size'], 2))
                for j in range(0, train_params['batch_size']):
                    sam = data[0][i*train_params['batch_size'] + j]
                    sam_len = sam.shape[0]
                    X[j, :sam_len, :] = np.true_divide(sam, sam.max())
                    Y[j][0] = float(1)
                pred = model.evaluate(X,Y, 
                        batch_size=train_params['batch_size'])
                print(pred)
                acu_neg = acu_neg + pred[1]
                acu = acu + pred[1]
            acu_pos = float(acu_pos)/float(int(train_params['pos_samples']/train_params['batch_size'])) 
            acu_neg = float(acu_neg)/float(int(train_params['neg_samples']/train_params['batch_size']))
            acu = float(acu)/float(int(train_params['pos_samples']/train_params['batch_size']) + int(train_params['neg_samples']/train_params['batch_size']))
            f_out.write('acu_pos: ' + str(acu_pos)+', acu_neg: '+str(acu_neg)+', acu:'+str(acu)+'\n')

'''
def label2idx(img_mat,N):
    print('------label 2 index processing------')   
    #labelset
    idx_1d= np.arange(1,img_mat.size+1,1)
    #ind_eleMap=np.reshape(idx_1d, (img_mat.shape))
    #print(idx_1d)
    #ind_eleMap=np.zeros_like(img_mat)
    ind_eleMap=np.reshape(idx_1d,(img_mat.shape))
    #print(ind_eleMap)
    for pix in range(1,N):
        #np.where(a < 5, a, 10*a)
        bin_elementMap=np.where(img_mat==pix,1,0)
        remain_ele=np.trim_zeros(ind_eleMap*bin_elementMap)
        np.concatenate(labelset, remain_ele,axis=0)
        #print(vect)
        #print(bin_elementMap)
'''
def gaussian_weight(X,mu,sigma):
    return math.exp((-1/(2*sigma*sigma))*((X-mu)*(X-mu)));

def slicSeg(srcFiles_img,srcFiles_labels,img_mat,lbl_mat):
    #suerpixel extraction
    # Define hyperparameters
    dist_sigma = 10; #sigma for gaussian distance weight in Part 1
    gauss_weight = 1;
    numSegments=200
    
    # srcFiles_img = dir('G:\SPFExtarct\MSRA10K_Imgs_GT\MSRA10K_Imgs_GT\Imgs\*.jpg');
    # srcFiles_labels = dir('G:\SPFExtarct\MSRA10K_Imgs_GT\MSRA10K_Imgs_GT\Imgs\*.png');
    # may add some normalization to distance weight
    #path, dirs, files = next(os.walk(srcFiles_img+'/*.jpg'))
    #file_count = len(files)
    dir_img=glob.glob(srcFiles_img)
    dir_lbl=glob.glob(srcFiles_labels)
    filenum=3
    # filenum=len(dir_img)
    all_Q={}
    all_superpixel_labels={}
    #for img in glob.glob(srcFiles_img):
    for a in range(0,filenum):
        sp_tx = '--superpixel segmentation for image: %s--' % (a+1)
        print(sp_tx)
        # read image
        #print(dir_img[i])
        #print(dir_lbl[i])
        print(a)
        path_img=dir_img[a]
        path_lbl=dir_lbl[a]
        im_image = io.imread(path_img)
        #print(im_image)
        im_label = io.imread(path_lbl)
        #print(im_label)
        #[L,N] = superpixels(im_lab,200,'IsInputLab',1);
        # L= label numbers, N = superpixel numbers 
        # im_lab = rgb2lab(im);
        L = slic(img_as_float(im_image), n_segments = numSegments) #include the lab convert
        L=L+1 # start from 1
        N=np.amax(L)
        print('superpixel segment number: ', N)

    # Vectorize superpixels in R and make mean color vector for each r
        print('----mean color calculation----')
        #im_size = io.imread(path_img).size;
        C = np.zeros((N,3));
        #r_val=im_image[:,:,0]
        #g_val=im_image[:,:,1]
        #b_val=im_image[:,:,2]
        for i in range(1,N):
            #print(np.where(L==i,1,0))
            #r_val=im_image(:,:,0)
            #g_val=im_image(:,:,1)
            #b_valim_image(:,:,2)
            red_spi_value=np.mean(im_image[(np.where(L==i,1,0)==1),0])
            green_spi_value=np.mean(im_image[(np.where(L==i,1,0)==1),1])
            blue_spi_value=np.mean(im_image[(np.where(L==i,1,0)==1),2])
            #r_val_txt= 'sp:%s, r_val: %s'% (i,red_spi_value)
            #print(r_val_txt)
            
            C[i,:]=[red_spi_value, green_spi_value, blue_spi_value];
            #np.append(C,[red_spi_value, green_spi_value, blue_spi_value], axis=0)
        #print(C)
        print('----mean color calculation: done!----')
    
    # Find the superpixel center for each region r
        print('----center position calculation----')
        #P = np.zeros((N,1));
        segments_ids = np.unique(L)
        #print('sp segments id: ', segments_ids)
        # centers
        #label_idx = label2idx(L,N);
        for i in range(1,N):
            centers = np.round(np.array([np.mean(np.nonzero(L==i),axis=1) for i in segments_ids]))
            #P(i,1) = round(mean(label_idx{i}));
        #print(centers)
        print('----center position calculation: done!----')
    
    # Make contrast separation vector Q by comparing each superpixel
        print('----mat obtaining----')
        Q_color = np.zeros((N,N,3))
        Q = np.zeros((N,N,3))
        dist = np.zeros((N,N))
        
        for i in range(1,N):
            for j in range(1,N):
                p_i=centers[i]
                p_j=centers[j]
                #dist(i,j) = norm(p_i - p_j);
                dist[i,j] = np.linalg.norm(p_i - p_j);
                #dist_txt='i: %s, j: %s, Euc distance: %s'% (p_i,p_j,dist[i,j])
                #print(dist_txt)
                #print('----distance of inter-superpixel: finished----')
                #count of unit number in each superpixel
                t_j = np.sum((L==j).astype(int)) #np.sum([np.nonzero(L==j)]) #numel(label_idx{j});
                dist_weight = gaussian_weight(dist[i,j],0,dist_sigma);
                #print(t_j)
               
                Q[i,j,0] = t_j*abs(C[i,0]-C[j,0])*gauss_weight*dist_weight;
                Q[i,j,1] = t_j*abs(C[i,1]-C[j,1])*gauss_weight*dist_weight;
                Q[i,j,2] = t_j*abs(C[i,2]-C[j,2])*gauss_weight*dist_weight;
                #print('----Q weighted by distance: finished----')
            
            #print(dist[i,:])
            #print(np.argsort(dist[i,:],axis=0))
            #[~,I] = sort(dist(i,:)];
            I=np.argsort(dist[i,:],axis=0)
            Q_color[i,:,:] = Q[i,I,:]
            #print('----Q_color weighted by distance: finished----')  
        #all_Q(1,a) = {Q_color};
        #print(Q_color)
        all_Q = dict(zip([1,a], Q_color)) #{Q_color}
        print('------all_Q obtaining: done!------')
        
        
        #label
        superpixel_label = np.zeros((1,N))
        im_bw=im_label #binary
        for j in range(1,N): #1:size(label_idx,1)
            #label_idx_j = label_idx{j};
            label_region = L==j
            if ( np.count_nonzero(label_region)>np.count_nonzero(~label_region) ):
                superpixel_label[1,j]= 1;
                  
        all_superpixel_labels = dict(zip([a,1], superpixel_label))  #transpose
        print('------all_superpixel_labels obtaining: done!------')
        
    #save imagelists and segmentation labels 
    #save('all_Q.mat','all_Q');
    
    sio.savemat(img_mat,{'all_Q':all_Q});
    print('--save mat: All_Q.mat done!--')
    
    sio.savemat(lbl_mat,{'all_superpixel_labels':all_superpixel_labels});
    print('--save mat: all_superpixel_labels.mat done!--')
    
def testing_eval(testingResult,testingQmat,testingLabelmat):
    f_out = open(testingResult,'w')
    train_data = sio.loadmat(testingQmat)['all_Q']
    train_labels = sio.loadmat(testingLabelmat)['all_superpixel_labels']
    data = get_train_data(train_data, train_labels)
    train_params = initialize_params(train_data, data)

    model = load_model('model_4.h5')
    num_images = train_data.shape[1]
    actual_images=0;
    avg_acu = 0
    out_mat = np.zeros((num_images, train_params['max_size']))
    try:
        for i in range(0, num_images):
            given_image_sp = train_data[0][i]
            given_image_lb = train_labels[i][0]
            num_sp = given_image_lb.shape[1]
            acu = 0
            for j in range(0, num_sp):
                given_label = given_image_lb[0][j]
                X = np.zeros((1,train_params['max_size'], 3))
                if given_label == 0:
                    given_rep = np.asarray(given_image_sp[j][:], dtype='float')
                    sam_len = given_rep.shape[0]
                    #X[0,:sam_len, :] = np.true_divide(given_rep, given_rep.max())
                    if (given_rep.max()==0):
                        X[0,:sam_len, :] = given_rep
                    else:
                        X[0,:sam_len, :] = np.true_divide(given_rep, given_rep.max())                    
                    pred = model.predict(X)
                    pred_idx = np.where(pred == pred.max())[1][0]
                    #if (pred.max() < 0.60) and (pred_idx == 1): # constraint
                        # pred_idx = 0
                    out_mat[i][j] = pred_idx
                    if pred_idx == given_label:
                        acu += 1
                    else:
                        pass
                elif given_label == 1:
                    given_rep = np.asarray(given_image_sp[j][:], dtype='float')
                    sam_len = given_rep.shape[0]
                    #X[0,:sam_len, :] = np.true_divide(given_rep, given_rep.max())
                    if (given_rep.max()==0):
                        X[0,:sam_len, :] = given_rep
                    else:
                        X[0,:sam_len, :] = np.true_divide(given_rep, given_rep.max())
                    pred = model.predict(X)
                    pred_idx = np.where(pred == pred.max())[1][0]
                    out_mat[i][j] = pred_idx
                    if pred_idx == given_label:
                        acu += 1
                    else:
                        pass
                else:
                    print('SOMETHING IS WRONG !')
                    sys.exit(1)
            acu = float(acu)/float(num_sp)
            print('Given Image Acu: ' + str(acu))
            avg_acu = avg_acu + acu
            actual_images +=1
            f_out.write('acu:'+str(acu)+'\n')
    except IndexError:
        avg_acu = float(avg_acu)/float(actual_images)
        print('(Index Except) Over Acu: ' + str(avg_acu))
        #sio.savemat('./Test_predict.mat', mdict={'predict_mat':pred})    
        sio.savemat('./Test_out.mat', mdict={'out_mat':out_mat})
        f_out.write('(Index Except) Over Acu:'+str(avg_acu)+'\n')
    else:    
        avg_acu = float(avg_acu)/float(num_images)
        print('Over Acu: ' + str(avg_acu))
        #sio.savemat('./Test_predict.mat', mdict={'predict_mat':pred})    
        sio.savemat('./Test_out.mat', mdict={'out_mat':out_mat})
        f_out.write('Over Acu:'+str(avg_acu)+'\n')

if __name__ == '__main__':
    
    #SLIC mat information
    '''
    train_image='../MSRA10K_Imgs_GT/Imgs/*.jpg'
    train_mask='../MSRA10K_Imgs_GT/Imgs/*.png'
    test_image='../testing/*.jpg'
    test_mask='../testing/*.png'
    slicSeg(train_image,train_mask,train_image_mat,train_mask_mat)
    slicSeg(test_image,test_mask,test_image_mat,test_mask_mat)
    '''
    # make_superpixel_labels()
    #input data directory path training
    train_image_mat='../train/all_Q.mat'
    train_mask_mat='../train/all_superpixel_labels.mat'
    #testing
    test_image_mat='../test/all_Q.mat'
    test_mask_mat='../test/all_superpixel_labels.mat'

    # training
    #readSLICandMDLInit('train_result.txt',train_image_mat,train_mask_mat)
    #testing
    testing_eval('test_results.txt',test_image_mat,test_mask_mat)


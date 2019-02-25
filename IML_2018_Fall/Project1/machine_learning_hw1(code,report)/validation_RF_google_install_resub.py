import numpy as np
import pandas as pd
import random
import math

import itertools

from sklearn.model_selection import train_test_split
from sklearn import tree
from sklearn.metrics import accuracy_score, r2_score
from sklearn.metrics import confusion_matrix
review_data = pd.read_csv("./preprocessed_googleplaystore.csv", usecols=["Price", "Rating", "Reviews", "Installs"])
review_data = review_data.sample(frac=1).reset_index(drop=True)#shuffle
def chunks(arr, m):
    n = int(math.ceil(len(arr) / float(m)))
    return [arr[i:i + n] for i in range(0, len(arr), n)]
    
def DecisionTree(train_data, feature_selected):
    # First separate feature and target
    train_x = train_data[feature_selected]
    train_y = train_data.loc[:, "Installs"]
    clr = tree.DecisionTreeClassifier(criterion="entropy", random_state=None)
    review_clr = clr.fit(train_x, train_y)
    return review_clr
    
def BootstrapResample(train_data):
    n = len(train_data)
    resample_i = np.floor(np.random.rand(n)*len(train_data)).astype(int)
    train_resampled = train.iloc[resample_i]
    df_train = pd.DataFrame(train_resampled, columns=(review_data.columns))
    return df_train
    
def VoteResult(lst):
    return max(lst, key=lst.count)

#resub validation  
K = 1
divided_data = chunks(review_data, K)

all_feature = ["Rating", "Reviews"]
feature_list = all_feature

select_list = []
for L in range(1, len(feature_list)+1):
    for subset in itertools.combinations(feature_list, L):
        select_list.append(list(subset))
        
        
confusion_matrix_rating = np.zeros((6, 6), dtype=np.int)
np.set_printoptions(suppress=True)
for i in range(K):
    ########################### choosing train_data and test_data
    x_train = review_data[all_feature]
    y_train = review_data["Installs"]
    x_test = x_train
    y_test = y_train

    train = pd.concat([x_train, y_train], axis=1)
    ###########################train
    pred_list = []

    # Build 100 trees
    for t in range(100):
        # Randomly select features and resample
        rng = np.random.randint(0, len(feature_list))
        select = select_list[rng]
        
        train_data = train[select]
        #print(train)
        single_tree_train = BootstrapResample(train_data)
        
        # test data also need to be reshaped
        x_test_reshaped = x_test[select]
        
        # Build tree
        clr = DecisionTree(single_tree_train, select)
        y_pred = clr.predict(x_test_reshaped)
        
        # First iteration, build empty 2d-list
        if(t == 0):
            for j in range(len(x_test)):
                pred_list.append([y_pred[j]])
        else:
            for j in range(len(x_test)):
                pred_list[j].append(y_pred[j])

    vote_list = []

    for lst in pred_list:
        vote_list.append(VoteResult(lst))
    ###########################classify
    lth = len(vote_list)
    vote_list_class = strs = ["" for x in range(lth)]
    y_test_class = ["" for x in range(lth)]    
    y_test_list = y_test.tolist()

    for ind in range(lth):

        if(vote_list[ind]<=1000):
            vote_list_class[ind] = "0"
        elif(vote_list[ind]<=10000):
            vote_list_class[ind] = "1"
        elif(vote_list[ind]<=100000):
            vote_list_class[ind] = "2"
        elif(vote_list[ind]<=1000000):
            vote_list_class[ind] = "3"
        elif(vote_list[ind]<=10000000):
            vote_list_class[ind] = "4"
        else:
            vote_list_class[ind] = "5"
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
    arr = confusion_matrix(vote_list_class, y_test_class, labels=['0', '1', '2', '3', '4', '5'])
    confusion_matrix_rating =  confusion_matrix_rating + arr
###########################
print("\n","class =",["<=1000","<=10000","<=100000","<=1000000","<=10000000","other"],"\n")
print(confusion_matrix_rating)
precision = np.zeros(6)
recall = np.zeros(6)
accurate = 0
for i in range(0, 6):
    precision[i] = confusion_matrix_rating[i][i] / sum(confusion_matrix_rating[i])
    recall[i] = confusion_matrix_rating[i][i] / sum([row[i] for row in confusion_matrix_rating])
    accurate += confusion_matrix_rating[i][i]

accuracy = accurate/sum(sum(confusion_matrix_rating))


print("precision =", precision)
print("recall =", recall)
print("accuracy =", accuracy)
import numpy as np
import pandas as pd
import random
import math
import itertools
from sklearn.model_selection import train_test_split
from sklearn import tree
from sklearn.metrics import accuracy_score, r2_score
from sklearn.metrics import confusion_matrix
#review_data = pd.read_csv("./preprocessed_googleplaystore.csv", usecols=["Price", "Installs", "Reviews", "Rating"])
#review_data = review_data.sample(frac=1).reset_index(drop=True)#shuffle


def chunks(arr, m):
    n = int(math.ceil(len(arr) / float(m)))
    return [arr[i:i + n] for i in range(0, len(arr), n)]
    
def DecisionTree(train_data, feature_selected):
    # First separate feature and target
    train_x = train_data[feature_selected]
    train_y = train_data.loc[:, "class"]
    classifier = tree.DecisionTreeClassifier(criterion="entropy", max_depth=3, random_state=None)
    iris_clr = classifier.fit(train_x, train_y)
    return iris_clr
    
def BootstrapResample(train_data):
    n = len(train_data)
    resample_i = np.floor(np.random.rand(n)*len(train_data)).astype(int)
    train_resampled = train.iloc[resample_i]
    df_train = pd.DataFrame(train_resampled, columns=(iris.columns))
    return df_train
    
def VoteResult(lst):
    return max(lst, key=lst.count)
iris_data = pd.read_csv("./preprocessed_iris.csv")
mapping = {"Iris-setosa": "0", "Iris-versicolor": "1", "Iris-virginica": "2"}
iris = iris_data.replace(mapping)
iris = iris.sample(frac=1).reset_index(drop=True)#shuffle

    
    

#K-fold validation  
K = 1
divided_data = chunks(iris, K)


feature_list = list(iris.columns[:-1])
select_list = []
for L in range(1, len(feature_list)+1):
    for subset in itertools.combinations(feature_list, L):
        select_list.append(list(subset))


confusion_matrix_iris = np.zeros((3, 3), dtype=np.int)
for i in range(K):
    ########################### choosing train_data and test_data
    x_train = iris[["sepal length", "sepal width", "petal length", "petal width"]]
    y_train = iris["class"]
    x_test = x_train
    y_test = y_train
    train = pd.concat([x_train, y_train], axis=1)
    ###########################train
    pred_list = []

    # Build 100 trees
    for t in range(100):
        # Randomly select features and resample
        rng = np.random.randint(0, 15)
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
 
    ########################### build confusion_matrix
    arr = confusion_matrix(vote_list, y_test, labels=['0', '1', '2'])
    confusion_matrix_iris = confusion_matrix_iris + arr;

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

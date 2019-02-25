# coding: utf-8

# In[1]:

### Import and Global vars ###

# Commonly used module
import numpy as np
import pandas as pd

# For build decision tree
from sklearn import tree
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score, r2_score
from sklearn.utils import *

# For calculating combination
import math
import itertools

google_data = pd.read_csv(
    "./preprocessed_googleplaystore.csv",
    usecols=["Price", "Rating", "Reviews", "Installs"])

# In[3]:

### Split data to training

all_feature = ["Rating", "Reviews"]
x = google_data[all_feature]
y = google_data["Installs"]

# Set random_state to 42 for testing (will get same training set)
# Set None for using random seed
x_train, x_test, y_train, y_test = train_test_split(
    x, y, test_size=0.3, random_state=None)

# Concat training data first
train = pd.concat([x_train, y_train], axis=1)

# In[4]:

### Generate subsample for Building multiple trees ###
### By resampling training dataset ###


def BootstrapResample(train_data):
    n = len(train_data)
    resample_i = np.floor(np.random.rand(n) * len(train_data)).astype(int)
    train_resampled = train.iloc[resample_i]
    df_train = pd.DataFrame(train_resampled, columns=(google_data.columns))
    return df_train


# In[5]:

### Randomly select features for building random tree ###


def nCr(n, r):
    f = math.factorial
    return f(n) // f(r) // f(n - r)


# All posible feature selections to select
feature_list = all_feature
select_list = []

for L in range(1, len(feature_list) + 1):
    for subset in itertools.combinations(feature_list, L):
        select_list.append(list(subset))

# In[6]:

### Build decision tree ###


def DecisionTree(train_data, feature_selected):
    # First separate feature and target
    train_x = train_data[feature_selected]
    train_y = train_data.loc[:, "Installs"]
    clr = tree.DecisionTreeClassifier(
        criterion="entropy", max_depth=None, random_state=None)
    review_clr = clr.fit(train_x, train_y)
    return review_clr


# In[7]:

### Build Random Forest ###

pred_list = []

# Build 100 trees
for i in range(100):
    # Randomly select features and resample
    rng = np.random.randint(0, len(feature_list))
    select = select_list[rng]

    train_data = train[select]
    single_tree_train = BootstrapResample(train_data)

    # test data also need to be reshaped
    x_test_reshaped = x_test[select]

    # Build tree
    clr = DecisionTree(single_tree_train, select)
    y_pred = clr.predict(x_test_reshaped)

    # First iteration, build empty 2d-list
    if (i == 0):
        for j in range(len(x_test)):
            pred_list.append([y_pred[j]])
    else:
        for j in range(len(x_test)):
            pred_list[j].append(y_pred[j])

# In[8]:

### Vote result for each data row ###

vote_list = []


def VoteResult(lst):
    return max(lst, key=lst.count)


for lst in pred_list:
    vote_list.append(VoteResult(lst))

# In[9]:

### Evaluate accuracy of RF ###

print("Evaluate by accuracy score: ", end="")
print(accuracy_score(y_test, vote_list))

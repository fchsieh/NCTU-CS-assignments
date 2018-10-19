
# coding: utf-8

# In[1]:


### Import and Global vars ###

# Commonly used module
import numpy as np
import pandas as pd
from sklearn import tree

# For build decision tree
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
from sklearn.utils import *

# For calculating combination
import math
import itertools


# In[2]:


### Remove duplicate rows in googleplaystore.csv ###

playstore_data = pd.read_csv("./preprocessed_googleplaystore.csv")

# Remove duplicate rows 
playstore_data = playstore_data.drop_duplicates(subset=["App"], keep="first")

# Remove '+'  suffix in installs column
playstore_data = playstore_data.applymap(lambda x: str(x).rstrip('+'))

# Remove 'and up'  suffix in Android ver
playstore_data["Android Ver"] = playstore_data["Android Ver"].str.replace(r" and up$", "")

# Replace 'Varies with device' with 0
playstore_data["Android Ver"] = playstore_data["Android Ver"].str.replace(r"Varies with device", "0")

# Remove '$' prefix in price
playstore_data = playstore_data.applymap(lambda x: str(x).lstrip('$'))

# Output to csv
playstore_data.to_csv("preprocessed_googleplaystore.csv", sep=",", encoding="utf-8", index=False)


# In[3]:


### Put ratings to reviews.csv ###

playstore_data = pd.read_csv('./preprocessed_googleplaystore.csv')
review_data = pd.read_csv("./preprocessed_googleplaystore_user_reviews.csv")

app_name = playstore_data["App"]
installs = playstore_data["Installs"]
price = playstore_data["Price"]
category = playstore_data["Category"]
rating = playstore_data["Rating"]
reviews = playstore_data["Reviews"]

install_dict = dict(zip(app_name, installs))
price_dict = dict(zip(app_name, price))
category_dict = dict(zip(app_name, category))
rating_dict = dict(zip(app_name, rating))
reviews_dict = dict(zip(app_name, reviews))
data_dict = dict(zip(app_name, app_name))

for key, val in install_dict.items():
    if key in data_dict:
        data_dict[key] = [val]
for key, val in price_dict.items():
    if key in data_dict:
        data_dict[key].append(val)
for key, val in category_dict.items():
    if key in data_dict:
        data_dict[key].append(val)
for key, val in rating_dict.items():
    if key in data_dict:
        data_dict[key].append(val)
for key, val in reviews_dict.items():
    if key in data_dict:
        data_dict[key].append(val)


review_data["Installs"] = ""
review_data["Price"] = ""
review_data["Category"] = ""
review_data["Rating"] = ""
review_data["Reviews"] = ""
review_data.head()


# In[4]:


# Insert rating to reviews.csv
for index, row in review_data.iterrows():
    if row["App"] in rating_dict:
        row["Installs"] = data_dict[row["App"]][0]
        row["Price"] = data_dict[row["App"]][1]
        row["Category"] = data_dict[row["App"]][2]
        row["Rating"] = data_dict[row["App"]][3]
        row["Reviews"] = data_dict[row["App"]][4]
        review_data.loc[index, "Installs"] = row["Installs"]
        review_data.loc[index, "Price"] = row["Price"]*100
        review_data.loc[index, "Category"] = row["Category"]
        review_data.loc[index, "Rating"] = row["Rating"]*100
        review_data.loc[index, "Reviews"] = row["Reviews"]
    else:
        row["Installs"] = "NaN"
        row["Price"] = "NaN"
        row["Category"] = "NaN"
        row["Rating"] = "NaN"
        row["Reviews"] = "NaN"
        review_data.loc[index, "Installs"] = row["Installs"]
        review_data.loc[index, "Price"] = row["Price"]
        review_data.loc[index, "Category"] = row["Category"]
        review_data.loc[index, "Rating"] = row["Rating"]
        review_data.loc[index, "Reviews"] = row["Reviews"]


# In[5]:


review_data.head()


# In[6]:


# Output to csv
review_data.to_csv("preprocessed_googleplaystore_user_reviews.csv", sep=",", encoding="utf-8", index=False)


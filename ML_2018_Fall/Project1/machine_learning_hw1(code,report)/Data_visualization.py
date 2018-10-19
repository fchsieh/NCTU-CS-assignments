
# coding: utf-8

# In[1]:


### Import and Global vars ###

# Commonly used module
import numpy as np
import pandas as pd

# For data visualization 
import matplotlib.pyplot as plt
import seaborn as sns

iris_data = pd.read_csv("./preprocessed_iris.csv")


# In[2]:


g = sns.pairplot(iris_data, hue="class", diag_kind="kde")
g.savefig('iris_output.png')


# In[3]:


google_data = pd.read_csv("./preprocessed_googleplaystore.csv", usecols=["Category", "Rating", "Reviews", "Installs", "Price", "Last Updated"])
cat_list = list(google_data["Category"].unique())
replace_list = list(range(0, len(cat_list)))
mymap = dict(zip(cat_list, replace_list))

google_data = google_data.applymap(lambda s: mymap.get(s) if s in mymap else s)


# In[4]:


g = sns.pairplot(google_data, hue="Installs", diag_kind="auto")
g.savefig("google_output1.png")


# In[5]:


### Visualize Installs - Features ###

review_data = pd.read_csv("./preprocessed_googleplaystore_user_reviews.csv", usecols=["Category", "Rating", "Reviews", "Installs", "Price", "Sentiment_Polarity", "Sentiment_Subjectivity"])

review_data = review_data.dropna(axis=0, how="any")

cat_list = list(review_data["Category"].unique())
replace_list = list(range(0, len(cat_list)))
mymap = dict(zip(cat_list, replace_list))

review_data = review_data.applymap(lambda s: mymap.get(s) if s in mymap else s)


# In[6]:


g = sns.pairplot(review_data, hue="Installs", diag_kind="auto")
g.savefig("google_output2.png")


# In[7]:


### 分析 Rating 和 feature間的關聯 ###

google_data = pd.read_csv("./preprocessed_googleplaystore.csv", usecols=["Category", "Rating", "Reviews", "Installs", "Price", "Last Updated"])
cat_list = list(google_data["Category"].unique())
replace_list = list(range(0, len(cat_list)))
mymap = dict(zip(cat_list, replace_list))

google_data = google_data.applymap(lambda s: mymap.get(s) if s in mymap else s)


# In[8]:


g = sns.pairplot(google_data, hue="Rating", diag_kind="auto")
g.savefig("google_output3.png")


# In[9]:


review_data = pd.read_csv("./preprocessed_googleplaystore_user_reviews.csv", usecols=["Category", "Rating", "Reviews", "Installs", "Price", "Sentiment_Polarity", "Sentiment_Subjectivity"])

review_data = review_data.dropna(axis=0, how="any")

cat_list = list(review_data["Category"].unique())
replace_list = list(range(0, len(cat_list)))
mymap = dict(zip(cat_list, replace_list))

review_data = review_data.applymap(lambda s: mymap.get(s) if s in mymap else s)


# In[10]:


g = sns.pairplot(review_data, hue="Rating", diag_kind="auto")
g.savefig("google_output4.png")


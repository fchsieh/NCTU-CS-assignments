# coding: utf-8
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# For implement Kmeans
import random
from copy import deepcopy

# For accuracy
from sklearn.metrics import confusion_matrix
import seaborn as sns

# For cost function
from scipy.spatial import distance

plt.style.use("ggplot")

# Read data and do preprocessing
data = pd.read_csv("./datasets/data_noah.csv")
mapping = {"CH": 0, "FF": 1, "CU": 2}
data = data.replace({"pitch_type": mapping})
x = data["x"].values
y = data["y"].values
pitch_type = data["pitch_type"].values
Noah = np.array(list(zip(x, y)))

# Plotting values
plt.xlabel("Horizontal movement (x)")
plt.ylabel("Vertical movement (y)")
plt.scatter(x, y, c="black", s=5)


# Calculate Euclidean distance
def EuclideanDist(a, b, ax=1):
    return np.linalg.norm(a - b, axis=ax)


# Number  of clusters
k = 3


# Implement kmeans
def kmeans(k, dataset):
    # Pick k points as initial centroid
    # May cause error if use below method(duplicate centroid)
    #center = dataset[np.random.randint(dataset.shape[0], size=k), :]
    center = dataset[:k]
    # Store old centroid when it updates
    center_old = np.zeros(center.shape)
    # Cluster labels: 3 clusters (0, 1, 2)
    clusters = np.zeros(len(dataset))
    # Error function
    err = EuclideanDist(center, center_old, None)
    # Loop will run till the err becomes 0
    while err != 0:
        # Assign values to its closest centroid
        for i in range(len(dataset)):
            distances = EuclideanDist(dataset[i], center)
            cluster = np.argmin(distances)
            clusters[i] = cluster
        # Store the old centroid value
        center_old = deepcopy(center)
        for i in range(k):
            points = [
                dataset[j] for j in range(len(dataset)) if clusters[j] == i
            ]
            center[i] = np.mean(points, axis=0)
        err = EuclideanDist(center, center_old, None)
    return clusters, center


colors = ['r', 'g', 'b']
clusters, center = kmeans(3, Noah)
fig, ax = plt.subplots(figsize=(10, 7))
points = None
for i in range(k):
    points = np.array([Noah[j] for j in range(len(Noah)) if clusters[j] == i])
    ax.scatter(points[:, 0], points[:, 1], s=5, c=colors[i], label=colors[i])
ax.scatter(center[:, 0], center[:, 1], marker="*", s=250, c="black")
ax.legend(["CH", "FF", "CU"])
plt.xlabel("Horizontal movement (x)")
plt.ylabel("Vertical movement (y)")
plt.title("Kmeans result")
# Save result
fig.savefig("Kmeans_result.png")


# Build accuracy score function
def accuracy(y_true, y_pred):
    total = len(y_true)
    TP = 0
    for i in range(len(y_true)):
        if y_pred[i] == y_true[i]:
            TP += 1
    return TP / total


# Calculate accuracy
fig, ax = plt.subplots(figsize=(10, 7))
cm = confusion_matrix(pitch_type, clusters)
sns.heatmap(cm, annot=True, ax=ax, fmt="d")
ax.set_xlabel("Predicted labels")
ax.set_ylabel("True labels")
ax.set_title("Confusion matrix")
ax.xaxis.set_ticklabels(["CH", "FF", "CU"])
ax.yaxis.set_ticklabels(["CH", "FF", "CU"])
# Save result
fig.savefig("Confusion_matrix.png")
print("Accuracy of using x and y: ", accuracy(pitch_type, clusters))


# Show why there is 3 clusters
def wcss(k, points, centers):
    wcss = 0
    for i in range(k):
        for point in points[i]:
            wcss += (abs(EuclideanDist(point, centers[i], None)))**2
    return wcss


wcss_res = []
for k in range(1, 11):
    points = []
    clusters, center = kmeans(k, Noah)
    for i in range(k):
        point = np.array(
            [Noah[j] for j in range(len(Noah)) if clusters[j] == i])
        points.append(point)
    wcss_res.append(wcss(k, points, center))

k = range(1, 11)
fig, ax = plt.subplots(figsize=(10, 7))
plt.plot(k, wcss_res)
plt.title("Elbow method")
plt.xlabel("k clusters")
plt.ylabel("wcss")
# save result of elbow method
fig.savefig("elbow_method.png")

# Use another two or more attributes to partition
x = data["tstart"].values
y = data["y"].values
NewData = np.array(list(zip(x, y)))
plt.scatter(x, y, c="black", s=5)

clusters, center = kmeans(3, NewData)
colors = ['r', 'g', 'b']
fig, ax = plt.subplots(figsize=(10, 7))
points = []
for i in range(3):
    point = np.array(
        [NewData[j] for j in range(len(NewData)) if clusters[j] == i])
    points.append(point)
    ax.scatter(point[:, 0], point[:, 1], s=5, c=colors[i], label=colors[i])
ax.scatter(center[:, 0], center[:, 1], marker="*", s=250, c="black")
ax.legend(["CH", "FF", "CU"])
plt.xlabel("tsart")
plt.ylabel("Vertical movement (y)")
plt.title("Kmeans result, tstart and Vertical movement")
# Save result2
fig.savefig("Kmeans_result2.png")

# Calculate accuracy
fig, ax = plt.subplots(figsize=(10, 7))
cm = confusion_matrix(pitch_type, clusters)
sns.heatmap(cm, annot=True, ax=ax, fmt="d")
ax.set_xlabel("Predicted labels")
ax.set_ylabel("True labels")
ax.set_title("Confusion matrix, tstart and Vertical movement")
ax.xaxis.set_ticklabels(["CH", "FF", "CU"])
ax.yaxis.set_ticklabels(["CH", "FF", "CU"])
# Save result
fig.savefig("Confusion_matrix2.png")
print("Accuracy of using tstart and y: ", accuracy(pitch_type, clusters))

# Do elbow method again with new data
wcss_res = []
for k in range(1, 11):
    points = []
    clusters, center = kmeans(k, NewData)
    for i in range(k):
        point = np.array(
            [NewData[j] for j in range(len(NewData)) if clusters[j] == i])
        points.append(point)
    wcss_res.append(wcss(k, points, center))

k = range(1, 11)
fig, ax = plt.subplots(figsize=(10, 7))
plt.plot(k, wcss_res)
plt.title("Elbow method, tstart and Vertical movement")
plt.xlabel("k clusters")
plt.ylabel("wcss")
# save result of elbow method of new data
fig.savefig("elbow_method2.png")

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy.io import loadmat

#Be careful with the file path!
data = loadmat('data/hw4.mat')
from sklearn.preprocessing import OneHotEncoder
encoder = OneHotEncoder(sparse=False, categories='auto')
y_onehot = encoder.fit_transform(data['y'])


def sigmoid(z):
    return 1 / (1 + np.exp(-z))


def forward_propagate(X, theta1, theta2):
    m = X.shape[0]

    # foward propagation
    a1 = np.c_[np.ones((m, 1)), X]  # need to add bias here
    z2 = a1 * theta1.T
    a2 = np.c_[np.ones((m, 1)), sigmoid(z2)]  # need to add bias here
    z3 = a2 * theta2.T
    h = sigmoid(z3)

    return a1, z2, a2, z3, h


def cost(params, input_size, hidden_size, num_labels, X, y, learning_rate):
    m = X.shape[0]
    X = np.matrix(X)
    y = np.matrix(y)
    # reshape the parameter array into parameter matrices for each layer
    theta1 = np.matrix(
        np.reshape(params[:hidden_size * (input_size + 1)],
                   (hidden_size, (input_size + 1))))
    theta2 = np.matrix(
        np.reshape(params[hidden_size * (input_size + 1):],
                   (num_labels, (hidden_size + 1))))
    # run the feed-forward pass
    a1, z2, a2, z3, h = forward_propagate(X, theta1, theta2)
    # compute the cost
    J = 0
    for i in range(m):
        first_term = np.multiply(-y[i, :], np.log(h[i, :]))
        second_term = np.multiply((1 - y[i, :]), np.log(1 - h[i, :]))
        J += np.sum(first_term - second_term)

    J = J / m
    J += (float(learning_rate) / (2 * m) * (np.sum(np.power(
        theta1[:, 1:], 2)) + np.sum(np.power(theta2[:, 1:], 2))))

    return J


# initial setup
input_size = 400
hidden_size = 10
num_labels = 10
learning_rate = 1
# randomly initialize a parameter array of the size of the full network's parameters
params = (np.random.random(size=hidden_size * (input_size + 1) + num_labels *
                           (hidden_size + 1)) - 0.5) * 0.2
m = data['X'].shape[0]
X = np.matrix(data['X'])
y = np.matrix(data['y'])
# unravel the parameter array into parameter matrices for each layer
theta1 = np.matrix(
    np.reshape(params[:hidden_size * (input_size + 1)], (hidden_size,
                                                         (input_size + 1))))
theta2 = np.matrix(
    np.reshape(params[hidden_size * (input_size + 1):], (num_labels,
                                                         (hidden_size + 1))))

a1, z2, a2, z3, h = forward_propagate(X, theta1, theta2)


def sigmoid_gradient(z):
    return np.multiply(sigmoid(z), (1 - sigmoid(z)))


def backprop(params, input_size, hidden_size, num_labels, X, y, learning_rate):
    m = X.shape[0]

    #Write codes here

    theta1 = np.matrix(
        np.reshape(params[:hidden_size * (input_size + 1)],
                   (hidden_size, (input_size + 1))))
    theta2 = np.matrix(
        np.reshape(params[hidden_size * (input_size + 1):],
                   (num_labels, (hidden_size + 1))))

    # do forward_propagation first
    a1, z2, a2, _, h = forward_propagate(X, theta1, theta2)

    # inits
    J = 0
    theta1_grad = np.zeros(theta1.shape)
    theta2_grad = np.zeros(theta2.shape)

    # compute the cost
    J = np.sum(
        np.multiply(-y, np.log(h)) - np.multiply((1 - y), np.log(1 - h)))
    J /= m

    J += (float(learning_rate) / (2 * m)) * (np.sum(
        np.power(theta1[:, 1:], 2)) + np.sum(np.power(theta2[:, 1:], 2)))

    # do backpropagation
    delta3 = h - y
    delta2 = np.multiply((theta2.T * delta3.T).T,
                         sigmoid_gradient(np.c_[np.ones((m, 1)), z2]))
    theta1_grad += (delta2[:, 1:]).T * a1
    theta2_grad += delta3.T * a2
    theta1_grad /= m
    theta2_grad /= m

    theta1_grad[:, 1:] += (theta1[:, 1:] * learning_rate) / m
    theta2_grad[:, 1:] += (theta2[:, 1:] * learning_rate) / m

    grad = np.concatenate((np.ravel(theta1_grad), np.ravel(theta2_grad)))

    return J, grad


from scipy.optimize import minimize
# minimize the objective function
fmin = minimize(
    fun=backprop,
    x0=params,
    args=(input_size, hidden_size, num_labels, X, y_onehot, learning_rate),
    method='TNC',
    jac=True,
    options={'maxiter': 250})

X = np.matrix(X)
theta1 = np.matrix(
    np.reshape(fmin.x[:hidden_size * (input_size + 1)], (hidden_size,
                                                         (input_size + 1))))
theta2 = np.matrix(
    np.reshape(fmin.x[hidden_size * (input_size + 1):], (num_labels,
                                                         (hidden_size + 1))))
a1, z2, a2, z3, h = forward_propagate(X, theta1, theta2)
y_pred = np.array(np.argmax(h, axis=1) + 1)

correct = [1 if a == b else 0 for (a, b) in zip(y_pred, y)]
accuracy = (sum(map(int, correct)) / float(len(correct)))
print('accuracy = {0}%'.format(accuracy * 100))

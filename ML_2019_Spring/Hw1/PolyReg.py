import math

import matplotlib.pyplot as plt
import numpy as np

import MatOP as mat


class PolyReg():
    # private:
    def __init__(self, dimension, lambda_, data):
        self.data = data
        self.dim = dimension - 1  # n = ?, minor bug here, hard to fix
        self.lambda_ = lambda_  # lambda = ?
        self.matA = []  # A
        self.vecX = []  # x
        self.vecB = []  # b
        self.__buildMatA()  # build matrix A and b
        self.__buildVecB()
        self.LSEcoeff = []
        self.NewtonCoeff = []

    def __buildMatA(self):
        for datarow in self.data:
            x = datarow[0]
            n = self.dim
            row = []
            while n >= 0:
                row.append(pow(x, n))
                n -= 1
            self.matA.append(row)

    def __buildVecB(self):
        for datarow in self.data:
            self.vecB.append([datarow[1]])

    def __gradient(self, A, b, x):
        res = mat.transpose(self.matA)  # A^T
        res = mat.matrixScalar(2, res)  # 2A^T
        res = mat.matrixMult(res, self.matA)  # 2A^TA
        res = mat.matrixMult(res, x)  # 2A^TAx
        tmp = mat.matrixScalar(2, mat.transpose(self.matA))  # 2A^T
        tmp = mat.matrixMult(tmp, b)  # 2A^Tb
        res = mat.matrixSub(res, tmp)  # 2A^TAx - 2A^Tb
        return res
    
    # public:
    def rLSE(self):
        x = []
        rATA = mat.matrixMult(mat.transpose(self.matA), self.matA)  # A^TA
        for i in range(len(rATA)):
            for j in range(len(rATA[0])):
                rATA[i][j] += mat.lambdaI(self.lambda_, len(
                    self.matA[0]))[i][j]  # do regularization
        rATA = mat.inverse(rATA)  # (A^TA + lambdaI)^-1
        x = mat.matrixMult(rATA,
                           mat.transpose(self.matA))  # (A^TA + lambdaI)^-1A^Tb
        x = mat.matrixMult(x, self.vecB)  # our final result(coefficient)
        flat_x = [item for sublist in x for item in sublist]  # flattern
        self.LSEcoeff = flat_x

        # Calculate error
        error = 0
        polyLSE = np.poly1d(self.LSEcoeff)
        for datarow in self.data:
            error += math.pow(datarow[1] - polyLSE(datarow[0]), 2)
        return flat_x, error

    def Newton(self, iters=100):
        # Build Hessian^-1 = (2A^TA)^-1
        AT = mat.transpose(self.matA)
        Hessian = mat.matrixScalar(2, AT)
        Hessian = mat.matrixMult(Hessian, self.matA)
        Hessian = mat.inverse(Hessian)  # Hf(x)^-1
        # init point (x0)
        x = []
        for _ in range(self.dim + 1):
            x.append([0])

        it = 0
        while it <= iters:
            x = mat.matrixSub(
                x,
                mat.matrixMult(Hessian, self.__gradient(
                    self.matA, self.vecB, x)))
            it += 1

        flat_x = [item for sublist in x for item in sublist]  # flattern
        self.NewtonCoeff = flat_x

        # Calculate error
        error = 0
        polyNewton = np.poly1d(self.NewtonCoeff)
        for datarow in self.data:
            error += math.pow(datarow[1] - polyNewton(datarow[0]), 2)

        return flat_x, error

    def visualize(self):
        x = []
        y = []
        # plot data points
        for datarow in self.data:
            x.append(datarow[0])
            y.append(datarow[1])
        plt.style.use("ggplot")
        fig, axs = plt.subplots(2, 1, constrained_layout=True)
        fig.suptitle("n = %d, lambda = %d" % (self.dim + 1, self.lambda_))

        # Plot LSE:
        axs[0].set_title("LSE")
        axs[0].set_xlabel("X")
        axs[0].set_ylabel("y")
        axs[0].plot(x, y, "bo")
        polyLSE = np.poly1d(self.LSEcoeff)
        polyLSE_y = polyLSE(x)
        axs[0].plot(x, polyLSE_y)
        # Plot Newton's
        axs[1].set_title("Newton's method")
        axs[1].set_xlabel("X")
        axs[1].set_ylabel("y")
        axs[1].plot(x, y, "bo")
        polyNewton = np.poly1d(self.NewtonCoeff)
        polyNewton_y = polyNewton(x)
        axs[1].plot(x, polyNewton_y)

        figname = "n%s_lambda%s" % (self.dim + 1, self.lambda_)
        fig.savefig(figname)
        plt.close()

import matplotlib.pyplot as plt
import random
import math
from pandas import *  # for debug


def generateTest():
    with open("testfile.txt", "w") as file:
        for _ in range(math.floor(random.randint(15, 30))):  # random testcases
            seed = random.randint(-50, 50)
            x = str(random.random() * seed)
            y = str(random.random() * seed)
            test = x + "," + y + "\n"
            file.write(test)


def readInputFile():
    matrix = []
    with open("testfile.txt", "r") as file:
        dataLines = file.readlines()
        for data in dataLines:
            nums = data.split(",")
            matrix.append([float(i) for i in nums])  # Build data matrix
    return matrix


def transpose(mat):
    return list(map(list, zip(*mat)))


def matmult(mat1, mat2):
    row = []
    result = []
    for i in range(len(mat1)):
        row = []
        for j in range(len(mat2[0])):
            sums = 0
            for k in range(len(mat2)):
                sums += mat1[i][k] * mat2[k][j]
            row.append(sums)
        result.append(row)
    return result


def lambdaI(lambda_, len):
    m = []
    for i in range(len):
        r = []
        for j in range(len):
            if i == j:
                r.append(lambda_)
            else:
                r.append(0)
        m.append(r)
    return m


def LUdecomp(mat):
    l_temp = []
    for j in range(len(mat)):
        l_tmp = lambdaI(1, len(mat))  # init Ln
        l_tmp_inv = lambdaI(1, len(mat))
        for i in range(j + 1, len(mat)):
            l_tmp[i][j] = -mat[i][j] / mat[j][j]
            l_tmp_inv[i][j] = mat[i][j] / mat[j][j]

        mat = matmult(l_tmp, mat)  # Ln * Ln-1 * ... L1 * A = U
        l_temp.append(l_tmp_inv)  # L = L1^-1 * L2^-1 * ... * Ln^-1

    l = lambdaI(1, len(mat))
    for i in range(len(l_temp)):
        l = matmult(l, l_temp[i])

    # return l, u
    return l, mat  # final mat = u


def forward(L, c):  # Lz = c
    z = []
    for i in range(len(c)):
        z.append(c[i])
        for j in range(i):
            z[i] = z[i] - L[i][j] * z[j]
        z[i] /= L[i][i]
    return z


def backward(U, z):  # Ux = z
    x = []
    for i in range(len(z)):
        x.append(0)
    for i in range(len(z) - 1, -1, -1):
        x[i] += z[i]
        for j in range(len(z) - 1, i, -1):
            x[i] = x[i] - U[i][j] * x[j]
        x[i] /= U[i][i]
    return x  # x is the column vector of the inverse matrix


def inverse(mat):
    res = []
    l, u = LUdecomp(mat)  # get A = LU
    dim = len(mat)
    c = lambdaI(1, dim)  # identity matrix c
    z = []
    # Solve z matrix: Lz = c
    for i in range(dim):
        z.append(forward(l, transpose(c)[i]))
    z = transpose(z)
    # Solve x matrix: Ux = z
    for i in range(dim):
        res.append(backward(u, transpose(z)[i]))
    res = transpose(res)
    return res  # A^-1


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

    # public:
    def rLSE(self):
        x = []
        rATA = matmult(transpose(self.matA), self.matA)  # A^TA
        for i in range(len(rATA)):
            for j in range(len(rATA[0])):
                rATA[i][j] += lambdaI(self.lambda_, len(
                    self.matA[0]))[i][j]  # do regularization
        rATA = inverse(rATA)  # (A^TA + lambdaI)^-1
        x = matmult(rATA, transpose(self.matA))  # (A^TA + lambdaI)^-1A^Tb
        x = matmult(x, self.vecB)
        flat_x = [item for sublist in x for item in sublist
                  ]  # reduce dim(list of list to single list)
        return flat_x

    def visualize(self):
        x = []
        y = []
        # plot data points
        for datarow in self.data:
            x.append(datarow[0])
            y.append(datarow[1])
        plt.plot(x, y, "ro")
        figname = "n%s_lambda%s" % (self.dim, self.lambda_)
        plt.savefig(figname)


def main():
    #generateTest()
    data = readInputFile()
    for i in range(2, 4):
        print("n =", i, ", lambda = 0")
        n = PolyReg(i, 0, data)
        coeff = n.rLSE()
        print("LSE:")
        print("Fitting Line: ", end="")
        for j in range(len(coeff)):
            print(coeff[j], end="")
            if (j != len(coeff) - 1):
                print("X^%d + " % (len(coeff) - 1 - j), end="")
        print("")


if __name__ == "__main__":
    main()
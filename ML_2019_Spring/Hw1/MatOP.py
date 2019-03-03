"""
Operations of matrix
"""


def transpose(mat):
    return list(map(list, zip(*mat)))


def matrixMult(mat1, mat2):
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


def matrixScalar(c, mat):
    for i in range(len(mat)):
        for j in range(len(mat[0])):
            mat[i][j] *= c
    return mat


def matrixSub(mat1, mat2):
    result = []
    for i in range(len(mat1)):
        row = []
        for j in range(len(mat1[0])):
            row.append(mat1[i][j] - mat2[i][j])
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

        mat = matrixMult(l_tmp, mat)  # Ln * Ln-1 * ... L1 * A = U
        l_temp.append(l_tmp_inv)  # L = L1^-1 * L2^-1 * ... * Ln^-1

    l = lambdaI(1, len(mat))
    for i in range(len(l_temp)):
        l = matrixMult(l, l_temp[i])

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
        x.append(0)  # init x
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

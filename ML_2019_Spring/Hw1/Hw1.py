import matplotlib.pyplot as plt


def readInputFile():
    matrix = []
    with open("testfile.txt", "r") as file:
        dataLines = file.readlines()
        for data in dataLines:
            nums = data.split(",")
            matrix.append([float(i) for i in nums])  # Build data matrix
    return matrix


class LinearRegression():
    def __init__(self, dimension, data):
        self.dim = dimension
        self.data = data

    def visualize(self):
        x = []
        y = []
        # plot data points
        for datarow in self.data:
            x.append(datarow[0])
            y.append(datarow[1])
        plt.plot(x, y, "ro")
        figname = "n%s" % self.dim
        plt.savefig(figname)


def main():
    data = readInputFile()
    n = LinearRegression(0, data)
    n.visualize()


if __name__ == "__main__":
    main()
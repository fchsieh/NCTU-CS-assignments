class MultiStageGraph():
    def __init__(self, daysToAllocate, courseToStudy, maxResource):
        # stages
        self.stages = courseToStudy
        # resource
        self.days = daysToAllocate
        # budget
        self.maxResource = maxResource
        # first node and last node(result)
        self.root = None
        self.sink = None

    def BuildGraph(self, pTable):
        #i , j, m
        prevStage = []
        currStage = []
        self.root = Node(0, 0, 0, None)

        currentNode = None
        # Build 1st stage
        for i in range(self.days):
            currentNode = Node(1, i + 1, pTable[0][i], self.root)
            prevStage.append(currentNode)
        # Build 2 ~ M-1 stage
        for i in range(2, self.stages):
            # reset current stage
            currStage = []
            for j in range(
                    i,
                    min(self.maxResource - (self.stages - i - 1),
                        prevStage[-1].cumRes + self.days + 1)):
                currentNode = Node(i, j, 0, None)
                # Find best value of this node and update it
                currStage.append(currentNode)
                # connect previous node
                prevStartRes = j - self.days
                for k in range(self.days):
                    # out of range
                    if (prevStartRes + k > prevStage[-1].cumRes) \
                    or (prevStartRes + k < i-1):
                        pass
                    else:
                        # can connect
                        index = prevStartRes + k - i + 1
                        pastNode = prevStage[index]
                        profit = pTable[i - 1][-1 - k]
                        cumProfit = profit + pastNode.cumProf
                        if (cumProfit >= currentNode.cumProf):
                            currentNode.cumProf = cumProfit
                            currentNode.pred = pastNode
            # update previous stage
            prevStage = currStage
        # sink and get final result
        sink = Node(self.stages, 0, 0, None)

        for node in prevStage:
            for possibleUnit in range(
                    min(self.maxResource - node.cumRes, self.days)):
                profit = pTable[-1][possibleUnit]
                cumProfit = node.cumProf + profit
                if (cumProfit >= sink.cumProf):
                    sink.pred = node
                    sink.cumProf = cumProfit
                    sink.cumRes = possibleUnit + node.cumRes
        # return sink node
        self.sink = sink

    def BackPass(self):
        return self.sink.cumProf


class Node():
    def __init__(self, course, cumulateResources, cumulateProfit, predecessor):
        self.course = course
        self.cumRes = cumulateResources
        self.cumProf = cumulateProfit
        self.pred = predecessor


def removeLastNewline():
    # remove useless newlines, can avoid some errors
    with open("input.txt", "r") as infile:
        data = infile.readlines()
    while data[len(data) - 1] == "\n":
        data.pop()  # pop last element if it is a newline
    with open("input.txt", "w", newline="\r\n") as outfile:
        for i in range(len(data)):
            outfile.writelines(data[i])


def readResources():
    resourceOfProjects = []
    tmpRes = []
    tmp = []
    with open("input.txt", "r") as infile:
        data = infile.readlines()
    for i in range(len(data)):
        if i is not len(data) - 1:
            if data[i - 1] == '\n' and data[i + 1] == '\n':
                if len(data[i].split(' ')) != 1:
                    # is project table with only one row(is this legal??)
                    tmpRes.append(tmp)
                    tmp = []
                    pass
                else:
                    # general type, is a resource description(problem)
                    tmp.append(data[i])
            if data[i - 1] == '\n' and data[i + 1] != '\n':
                # new project, push saved problem
                tmpRes.append(tmp)
                tmp = []
    if (data[len(data) - 1]) != '\n':
        tmp.append(data[len(data) - 1])
    tmpRes.append(tmp)
    for i in range(len(tmpRes)):
        resourceOfProjects.append(list(map(int, tmpRes[i])))

    return resourceOfProjects


def readProjectTable():
    tableList = []
    tmpTable = []
    with open("input.txt", "r") as infile:
        data = infile.readlines()
    for i in range(len(data) - 1):
        if len(data[i].split(' ')) != 1:
            # is a project table row
            tmpRow = data[i].split(' ')
            tmpRow = [int(i) for i in tmpRow]
            tmpTable.append(tmpRow)
            if data[i + 1] == '\n':
                tableList.append(tmpTable)
                tmpTable = []
    return tableList


def transMat(mat):
    return list(map(list, zip(*mat)))


def init():
    removeLastNewline()
    problemsList = readResources()  # problems of each table
    tmpTable = readProjectTable()  # profit tables
    pTable = []
    for matT in tmpTable:
        pTable.append(transMat(matT))
    return problemsList, pTable


def main():
    projects, pTableList = init()
    with open("output.txt", "w", newline="\r\n") as outfile:
        for i in range(len(pTableList)):
            # projects to be solve
            for j in range(len(projects[i])):
                graph = MultiStageGraph(
                    len(pTableList[i][0]), len(pTableList[i]), projects[i][j])
                graph.BuildGraph(pTableList[i])
                outfile.write(str(graph.BackPass()) + "\n")


if __name__ == "__main__":
    main()
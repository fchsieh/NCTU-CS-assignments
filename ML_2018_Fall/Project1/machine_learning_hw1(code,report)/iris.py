def slFunc(e):
    return e["sepal_len"]
def swFunc(e):
    return e["sepal_wid"]
def plFunc(e):
    return e["petal_len"]
def pwFunc(e):
    return e["petal_wid"]
def classFunc(e):
    return e["class"]
def Q(LIST, string, count, i):
    if i == 1:
        n = 0.25
    elif i == 2:
        n = 0.5
    elif i == 3:
        n = 0.75
    else:
        print("Error: In function Q, the last argument can only be 1 or 2 or 3")
    index = count * n
    remain = index - int(index)
    if remain != 0.0:
        return LIST[int(index)][string]
    else:
        return round((LIST[int(index) - 1][string] + LIST[int(index)][string]) / 2, 2)



#data quality reports of the continuous features and categorical features  
con_f = {"sl": {},
         "sw": {},
         "pl": {},
         "pw": {}
        }
for x in con_f:
  con_f[x] = {"Count": 0,
              "Miss%": 0.0,
              "Card": 0,
              "Min": 0.0,
              "1stQrt": 0.0,
              "Mean": 0.0,
              "Median": 0.0,
              "3rdQrt": 0.0,
              "Max": 0.0,
              "Std_Dev": 0.0
             }
cat_f = {"class": {}}
cat_f["class"] = {"Count": 0,
                  "Miss%": 0.0,
                  "Card": 0,
                  "Mode": "-",
                  "Mode_Freq": 0,
                  "Mode%": 0.0,
                  "2nd_Mode": "-",
                  "2nd_Mode_Freq": 0,
                  "2nd_Mode%": 0.0
                 }
#insert all the data into irisList
irisList = []
irisList2 = [] #For the preprocessed file
iris = {"sepal_len": 0.0,
        "sepal_wid": 0.0,
        "petal_len": 0.0,
        "petal_wid": 0.0,
        "class": ""
}
f = open("iris.csv", "r")
f.readline()
count = 0
for line in f:
    line2 = line.rstrip("\n")
    theList = line2.split(",")
    iris["sepal_len"] = float(theList[0])
    iris["sepal_wid"] = float(theList[1])
    iris["petal_len"] = float(theList[2])
    iris["petal_wid"] = float(theList[3])
    iris["class"] = theList[4]
    irisList.append(iris.copy())
    irisList2.append(iris.copy())
    count += 1
f.close()
#count in con_f and cat_f
con_f["sl"]["Count"] = con_f["sw"]["Count"] = con_f["pl"]["Count"] = con_f["pw"]["Count"] = cat_f["class"]["Count"] = count
for element in irisList:
  con_f["sl"]["Mean"] += element["sepal_len"]
  con_f["sw"]["Mean"] += element["sepal_wid"]
  con_f["pl"]["Mean"] += element["petal_len"]
  con_f["pw"]["Mean"] += element["petal_wid"]
#mean in con_f
con_f["sl"]["Mean"] = round(con_f["sl"]["Mean"] / count, 2)
con_f["sw"]["Mean"] = round(con_f["sw"]["Mean"] / count, 2)
con_f["pl"]["Mean"] = round(con_f["pl"]["Mean"] / count, 2)
con_f["pw"]["Mean"] = round(con_f["pw"]["Mean"] / count, 2)
for x in irisList: 
    con_f["sl"]["Std_Dev"] += pow(x["sepal_len"] - con_f["sl"]["Mean"], 2)
    con_f["sw"]["Std_Dev"] += pow(x["sepal_wid"] - con_f["sw"]["Mean"], 2)
    con_f["pl"]["Std_Dev"] += pow(x["petal_len"] - con_f["pl"]["Mean"], 2)
    con_f["pw"]["Std_Dev"] += pow(x["petal_wid"] - con_f["pw"]["Mean"], 2)
#standard deviation in con_f
con_f["sl"]["Std_Dev"] = round(pow(con_f["sl"]["Std_Dev"] / count, 0.5), 2)
con_f["sw"]["Std_Dev"] = round(pow(con_f["sw"]["Std_Dev"] / count, 0.5), 2)
con_f["pl"]["Std_Dev"] = round(pow(con_f["pl"]["Std_Dev"] / count, 0.5), 2)
con_f["pw"]["Std_Dev"] = round(pow(con_f["pw"]["Std_Dev"] / count, 0.5), 2)
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of sepal length in con_f
irisList.sort(key = slFunc)
con_f["sl"]["Min"] = irisList[0]["sepal_len"] 
con_f["sl"]["1stQrt"] = Q(irisList, "sepal_len", count, 1)
con_f["sl"]["Median"] = Q(irisList, "sepal_len", count, 2)
con_f["sl"]["3rdQrt"] = Q(irisList, "sepal_len", count, 3)
con_f["sl"]["Max"] = irisList[count - 1]["sepal_len"]
buf = []
buf.append(irisList[0]["sepal_len"])
j = 0
i = 1
while i < count:
    if irisList[i]["sepal_len"] != buf[j]:
        buf.append(irisList[i]["sepal_len"])
        j += 1
    i += 1
con_f["sl"]["Card"] = len(buf)
buf.clear()
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of sepal width in con_f
irisList.sort(key = swFunc)
con_f["sw"]["Min"] = irisList[0]["sepal_wid"] 
con_f["sw"]["1stQrt"] = Q(irisList, "sepal_wid", count, 1)
con_f["sw"]["Median"] = Q(irisList, "sepal_wid", count, 2)
con_f["sw"]["3rdQrt"] = Q(irisList, "sepal_wid", count, 3)
con_f["sw"]["Max"] = irisList[count - 1]["sepal_wid"]
buf.append(irisList[0]["sepal_wid"])
j = 0
i = 1
while i < count:
    if irisList[i]["sepal_wid"] != buf[j]:
        buf.append(irisList[i]["sepal_wid"])
        j += 1
    i += 1
con_f["sw"]["Card"] = len(buf)
buf.clear()
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of petal length in con_f
irisList.sort(key = plFunc)
con_f["pl"]["Min"] = irisList[0]["petal_len"] 
con_f["pl"]["1stQrt"] = Q(irisList, "petal_len", count, 1)
con_f["pl"]["Median"] = Q(irisList, "petal_len", count, 2)
con_f["pl"]["3rdQrt"] = Q(irisList, "petal_len", count, 3)
con_f["pl"]["Max"] = irisList[count - 1]["petal_len"]
buf.append(irisList[0]["petal_len"])
j = 0
i = 1
while i < count:
    if irisList[i]["petal_len"] != buf[j]:
        buf.append(irisList[i]["petal_len"])
        j += 1
    i += 1
con_f["pl"]["Card"] = len(buf)
buf.clear()
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of petal width in con_f
irisList.sort(key = pwFunc)
con_f["pw"]["Min"] = irisList[0]["petal_wid"] 
con_f["pw"]["1stQrt"] = Q(irisList, "petal_wid", count, 1)
con_f["pw"]["Median"] = Q(irisList, "petal_wid", count, 2)
con_f["pw"]["3rdQrt"] = Q(irisList, "petal_wid", count, 3)
con_f["pw"]["Max"] = irisList[count - 1]["petal_wid"]
buf.append(irisList[0]["petal_wid"])
j = 0
i = 1
while i < count:
    if irisList[i]["petal_wid"] != buf[j]:
        buf.append(irisList[i]["petal_wid"])
        j += 1
    i += 1
con_f["pw"]["Card"] = len(buf)
buf.clear()
#Cardinality of class in cat_f
irisList.sort(key = classFunc)
buf.append(irisList[0]["class"])
j = 0
i = 1
while i < count:
    if irisList[i]["class"] != buf[j]:
        buf.append(irisList[i]["class"])
        j += 1
    i += 1
cat_f["class"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each class
i = 0
j = 0
times = 0
while i < count:
    if irisList[i]["class"] == buf[j]:
        times += 1
    else: 
        buf2.append(times)
        times = 1
        j += 1
    i += 1
buf2.append(times)
#find the most appearances
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#Mode, Mode_Freq, Mode% of class in cat_f
cat_f["class"]["Mode"] = buf[maxIndex]
cat_f["class"]["Mode_Freq"] = buf2[maxIndex]
cat_f["class"]["Mode%"] = round(buf2[maxIndex] * 100 / count, 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of class in cat_f
cat_f["class"]["2nd_Mode"] = buf[maxIndex]
cat_f["class"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["class"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / count, 2)
buf.clear()
buf2.clear()
#print the result to a file
f = open("iris_data_quality_report.csv", "w")
f.write("Continuous Features\n")
f.write("Feature,Count,Miss%,Card,Min,1st Qrt,Mean,Median,3rd Qrt,Max,Std Dev\n")
f.write("sepal length," + str(con_f["sl"]["Count"]) + "," + str(con_f["sl"]["Miss%"]) + "," + str(con_f["sl"]["Card"]) + "," + str(con_f["sl"]["Min"]) + "," + str(con_f["sl"]["1stQrt"]) + "," + str(con_f["sl"]["Mean"]) + "," + str(con_f["sl"]["Median"]) + "," + str(con_f["sl"]["3rdQrt"]) + "," + str(con_f["sl"]["Max"]) + "," + str(con_f["sl"]["Std_Dev"]) + "\n")
f.write("sepal width," + str(con_f["sw"]["Count"]) + "," + str(con_f["sw"]["Miss%"]) + "," + str(con_f["sw"]["Card"]) + "," + str(con_f["sw"]["Min"]) + "," + str(con_f["sw"]["1stQrt"]) + "," + str(con_f["sw"]["Mean"]) + "," + str(con_f["sw"]["Median"]) + "," + str(con_f["sw"]["3rdQrt"]) + "," + str(con_f["sw"]["Max"]) + "," + str(con_f["sw"]["Std_Dev"]) + "\n")
f.write("petal length," + str(con_f["pl"]["Count"]) + "," + str(con_f["pl"]["Miss%"]) + "," + str(con_f["pl"]["Card"]) + "," + str(con_f["pl"]["Min"]) + "," + str(con_f["pl"]["1stQrt"]) + "," + str(con_f["pl"]["Mean"]) + "," + str(con_f["pl"]["Median"]) + "," + str(con_f["pl"]["3rdQrt"]) + "," + str(con_f["pl"]["Max"]) + "," + str(con_f["pl"]["Std_Dev"]) + "\n")
f.write("petal width," + str(con_f["pw"]["Count"]) + "," + str(con_f["pw"]["Miss%"]) + "," + str(con_f["pw"]["Card"]) + "," + str(con_f["pw"]["Min"]) + "," + str(con_f["pw"]["1stQrt"]) + "," + str(con_f["pw"]["Mean"]) + "," + str(con_f["pw"]["Median"]) + "," + str(con_f["pw"]["3rdQrt"]) + "," + str(con_f["pw"]["Max"]) + "," + str(con_f["pw"]["Std_Dev"]) + "\n")
f.write("\n")
f.write("Categorical Features\n")
f.write("Feature,Count,Miss%,Card,Mode,Mode Freq,Mode%,2nd Mode,2nd Mode Freq,2nd Mode%\n")
f.write("class," + str(cat_f["class"]["Count"]) + "," + str(cat_f["class"]["Miss%"]) + "," + str(cat_f["class"]["Card"]) + "," + str(cat_f["class"]["Mode"]) + "," + str(cat_f["class"]["Mode_Freq"]) + "," + str(cat_f["class"]["Mode%"]) + "," + str(cat_f["class"]["2nd_Mode"]) + "," + str(cat_f["class"]["2nd_Mode_Freq"]) + "," + str(cat_f["class"]["2nd_Mode%"]) + "\n")
f.close()
#generate the preprocessed data
f = open("preprocessed_iris.csv", "w")
f1 = open("iris.csv", "r")
buf = f1.readline()
f.write(buf)
f1.close()
feature = {"sepal_len": "sl",
           "sepal_wid": "sw",
           "petal_len": "pl",
           "petal_wid": "pw",
           "class": "class"
}
for e in irisList2:
    for i in feature:
        if e[i] == "nan" or e[i] == "NaN" or e[i] == "Varies with device":
            if i in con_f:
                f.write(str(con_f[feature[i]]["Mean"]))
            elif i in cat_f:
                f.write(str(cat_f[feature[i]]["Mode"]))
            else:
                print("Error: Cannot write into the file!")
        else:
            f.write(str(e[i]))
        if i == "class":
            f.write("\n")
        else:
            f.write(",")
f.close()
    

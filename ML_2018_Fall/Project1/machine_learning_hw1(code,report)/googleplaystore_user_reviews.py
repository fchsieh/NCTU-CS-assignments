def spFunc(e):
    if e["sp"] == "nan":
        return 100.0
    else:
        return float(e["sp"])
def ssFunc(e):
    if e["ss"] == "nan":
        return 100.0
    else:
        return float(e["ss"])
def appFunc(e):
    return e["app"]
def trFunc(e):
    return e["tr"]
def sFunc(e):
    return e["s"]
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
        return round(float(LIST[int(index)][string]), 2)
    else:
        return round((float(LIST[int(index) - 1][string]) + float(LIST[int(index)][string])) / 2, 2)



#data quality reports of the continuous features and categorical features  
con_f = {"sp": {},
         "ss": {}
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
cat_f = {"app": {},
         "tr": {},
         "s": {}
        }
for x in cat_f:
    cat_f[x] = {"Count": 0,
                "Miss%": 0.0,
                "Card": 0,
                "Mode": "-",
                "Mode_Freq": 0,
                "Mode%": 0.0,
                "2nd_Mode": "-",
                "2nd_Mode_Freq": 0,
                "2nd_Mode%": 0.0
                }
#insert all the data into gpsurList
gpsurList = []
gpsurList2 = [] #For the preprocessed file
gpsur = {"app": "",
         "tr": "",
         "s": "",
         "sp": 0.0,
         "ss": 0.0
}
f = open("googleplaystore_user_reviews.csv", "r", encoding = "UTF-8")
f.readline()
count = 0
miss = [0, 0, 0, 0, 0]
for line in f:
    line2 = line.strip(" \n")
    idx1 = line2.rfind(",")
    gpsur["ss"] = line2[idx1 + 1: len(line2)]
    if gpsur["ss"] == "nan":
        miss[4] += 1
    idx2 = line2.rfind(",", 0, idx1)
    gpsur["sp"] = line2[idx2 + 1: idx1]
    if gpsur["sp"] == "nan":
        miss[3] += 1
    idx3 = line2.rfind(",", 0, idx2)
    gpsur["s"] = line2[idx3 + 1: idx2]
    if gpsur["s"] == "nan":
        miss[2] += 1
    if line2[0] == "\"":
        idx4 = line2.find("\"", 1, idx3)
        idx4 += 1
    else:
        idx4 = line2.find(",")
    gpsur["tr"] = line2[idx4 + 1: idx3]
    if gpsur["tr"] == "nan":
        miss[1] += 1
    gpsur["app"] = line2[0: idx4]
    if gpsur["app"] == "nan":
        miss[0] += 1
    gpsurList.append(gpsur.copy())
    gpsurList2.append(gpsur.copy())
    count += 1
f.close()
#count and miss rate in con_f and cat_f
cat_f["app"]["Count"] = cat_f["tr"]["Count"] = cat_f["s"]["Count"] = con_f["sp"]["Count"] = con_f["ss"]["Count"] = count
cat_f["app"]["Miss%"] = round(miss[0] * 100 / count, 2)
cat_f["tr"]["Miss%"] = round(miss[1] * 100 / count, 2)
cat_f["s"]["Miss%"] = round(miss[2] * 100 / count, 2)
con_f["sp"]["Miss%"] = round(miss[3] * 100 / count, 2)
con_f["ss"]["Miss%"] = round(miss[4] * 100 / count, 2)

for element in gpsurList:
    if element["sp"] != "nan": 
        con_f["sp"]["Mean"] += float(element["sp"])
    if element["ss"] != "nan": 
        con_f["ss"]["Mean"] += float(element["ss"])
#mean in con_f
con_f["sp"]["Mean"] = round(con_f["sp"]["Mean"] / (count - miss[3]), 2)
con_f["ss"]["Mean"] = round(con_f["ss"]["Mean"] / (count - miss[4]), 2)
for x in gpsurList: 
    if x["sp"] != "nan": 
        con_f["sp"]["Std_Dev"] += pow(float(x["sp"]) - con_f["sp"]["Mean"], 2)
    if x["ss"] != "nan": 
        con_f["ss"]["Std_Dev"] += pow(float(x["ss"]) - con_f["ss"]["Mean"], 2)
#standard deviation in con_f
con_f["sp"]["Std_Dev"] = round(pow(con_f["sp"]["Std_Dev"] / (count - miss[3]), 0.5), 2)
con_f["ss"]["Std_Dev"] = round(pow(con_f["ss"]["Std_Dev"] / (count - miss[4]), 0.5), 2)

#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of sentiment polarity in con_f
gpsurList.sort(key = spFunc)
con_f["sp"]["Min"] = round(float(gpsurList[0]["sp"]), 2) 
con_f["sp"]["1stQrt"] = Q(gpsurList, "sp", count - miss[3], 1)
con_f["sp"]["Median"] = Q(gpsurList, "sp", count - miss[3], 2)
con_f["sp"]["3rdQrt"] = Q(gpsurList, "sp", count - miss[3], 3)
con_f["sp"]["Max"] = round(float(gpsurList[count - miss[3] - 1]["sp"]), 2)
buf = []
buf.append(gpsurList[0]["sp"])
j = 0
i = 1
while i < count - miss[3]:
    if gpsurList[i]["sp"] != buf[j]:
        buf.append(gpsurList[i]["sp"])
        j += 1
    i += 1
con_f["sp"]["Card"] = len(buf)
buf.clear()
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of sentiment subjectivity in con_f
gpsurList.sort(key = ssFunc)
con_f["ss"]["Min"] = round(float(gpsurList[0]["ss"]), 2) 
con_f["ss"]["1stQrt"] = Q(gpsurList, "ss", count - miss[4], 1)
con_f["ss"]["Median"] = Q(gpsurList, "ss", count - miss[4], 2)
con_f["ss"]["3rdQrt"] = Q(gpsurList, "ss", count - miss[4], 3)
con_f["ss"]["Max"] = round(float(gpsurList[count - miss[4] - 1]["ss"]), 2)
buf = []
buf.append(gpsurList[0]["ss"])
j = 0
i = 1
while i < count - miss[4]:
    if gpsurList[i]["ss"] != buf[j]:
        buf.append(gpsurList[i]["ss"])
        j += 1
    i += 1
con_f["ss"]["Card"] = len(buf)
buf.clear()

#Cardinality of app in cat_f
gpsurList.sort(key = appFunc)
buf.append(gpsurList[0]["app"])
j = 0
i = 1
while i < count:
    if gpsurList[i]["app"] != buf[j]:
        buf.append(gpsurList[i]["app"])
        j += 1
    i += 1
if (miss[0]) > 0:
    buf.remove("nan")
cat_f["app"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each app
i = 0
j = 0
times = 0
while i < count:
    if gpsurList[i]["app"] == buf[j]:
        times += 1
    elif gpsurList[i]["app"] != "nan": 
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
#Mode, Mode_Freq, Mode% of app in cat_f
cat_f["app"]["Mode"] = buf[maxIndex]
cat_f["app"]["Mode_Freq"] = buf2[maxIndex]
cat_f["app"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss[0]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of app in cat_f
cat_f["app"]["2nd_Mode"] = buf[maxIndex]
cat_f["app"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["app"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss[0]), 2)
buf.clear()
buf2.clear()
#Cardinality of translated review in cat_f
gpsurList.sort(key = trFunc)
buf.append(gpsurList[0]["tr"])
j = 0
i = 1
while i < count:
    if gpsurList[i]["tr"] != buf[j]:
        buf.append(gpsurList[i]["tr"])
        j += 1
    i += 1
if (miss[1]) > 0:
    buf.remove("nan")
cat_f["tr"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each translated review
i = 0
j = 0
times = 0
while i < count:
    if gpsurList[i]["tr"] == buf[j]:
        times += 1
    elif gpsurList[i]["tr"] != "nan": 
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
#Mode, Mode_Freq, Mode% of translated review in cat_f
cat_f["tr"]["Mode"] = buf[maxIndex]
cat_f["tr"]["Mode_Freq"] = buf2[maxIndex]
cat_f["tr"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss[1]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of translated review in cat_f
cat_f["tr"]["2nd_Mode"] = buf[maxIndex]
cat_f["tr"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["tr"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss[1]), 2)
buf.clear()
buf2.clear()
#Cardinality of sentiment in cat_f
gpsurList.sort(key = sFunc)
buf.append(gpsurList[0]["s"])
j = 0
i = 1
while i < count:
    if gpsurList[i]["s"] != buf[j]:
        buf.append(gpsurList[i]["s"])
        j += 1
    i += 1
if (miss[2]) > 0:
    buf.remove("nan")
cat_f["s"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each sentiment
i = 0
j = 0
times = 0
while i < count:
    if gpsurList[i]["s"] == buf[j]:
        times += 1
    elif gpsurList[i]["s"] != "nan": 
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
#Mode, Mode_Freq, Mode% of sentiment in cat_f
cat_f["s"]["Mode"] = buf[maxIndex]
cat_f["s"]["Mode_Freq"] = buf2[maxIndex]
cat_f["s"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss[2]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of sentiment in cat_f
cat_f["s"]["2nd_Mode"] = buf[maxIndex]
cat_f["s"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["s"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss[2]), 2)
buf.clear()
buf2.clear()
#print the result to a file
f = open("googleplaystore_user_reviews_data_quality_report.csv", "w", encoding = "UTF-8")
f.write("Continuous Features\n")
f.write("Feature,Count,Miss%,Card,Min,1st Qrt,Mean,Median,3rd Qrt,Max,Std Dev\n")
f.write("Sentiment Polarity," + str(con_f["sp"]["Count"]) + "," + str(con_f["sp"]["Miss%"]) + "," + str(con_f["sp"]["Card"]) + "," + str(con_f["sp"]["Min"]) + "," + str(con_f["sp"]["1stQrt"]) + "," + str(con_f["sp"]["Mean"]) + "," + str(con_f["sp"]["Median"]) + "," + str(con_f["sp"]["3rdQrt"]) + "," + str(con_f["sp"]["Max"]) + "," + str(con_f["sp"]["Std_Dev"]) + "\n")
f.write("Sentiment Subjectivity," + str(con_f["ss"]["Count"]) + "," + str(con_f["ss"]["Miss%"]) + "," + str(con_f["ss"]["Card"]) + "," + str(con_f["ss"]["Min"]) + "," + str(con_f["ss"]["1stQrt"]) + "," + str(con_f["ss"]["Mean"]) + "," + str(con_f["ss"]["Median"]) + "," + str(con_f["ss"]["3rdQrt"]) + "," + str(con_f["ss"]["Max"]) + "," + str(con_f["ss"]["Std_Dev"]) + "\n")
f.write("\n")
f.write("Categorical Features\n")
f.write("Feature,Count,Miss%,Card,Mode,Mode Freq,Mode%,2nd Mode,2nd Mode Freq,2nd Mode%\n")
f.write("App name," + str(cat_f["app"]["Count"]) + "," + str(cat_f["app"]["Miss%"]) + "," + str(cat_f["app"]["Card"]) + "," + str(cat_f["app"]["Mode"]) + "," + str(cat_f["app"]["Mode_Freq"]) + "," + str(cat_f["app"]["Mode%"]) + "," + str(cat_f["app"]["2nd_Mode"]) + "," + str(cat_f["app"]["2nd_Mode_Freq"]) + "," + str(cat_f["app"]["2nd_Mode%"]) + "\n")
f.write("Translated Review," + str(cat_f["tr"]["Count"]) + "," + str(cat_f["tr"]["Miss%"]) + "," + str(cat_f["tr"]["Card"]) + "," + str(cat_f["tr"]["Mode"]) + "," + str(cat_f["tr"]["Mode_Freq"]) + "," + str(cat_f["tr"]["Mode%"]) + "," + str(cat_f["tr"]["2nd_Mode"]) + "," + str(cat_f["tr"]["2nd_Mode_Freq"]) + "," + str(cat_f["tr"]["2nd_Mode%"]) + "\n")
f.write("Sentiment," + str(cat_f["s"]["Count"]) + "," + str(cat_f["s"]["Miss%"]) + "," + str(cat_f["s"]["Card"]) + "," + str(cat_f["s"]["Mode"]) + "," + str(cat_f["s"]["Mode_Freq"]) + "," + str(cat_f["s"]["Mode%"]) + "," + str(cat_f["s"]["2nd_Mode"]) + "," + str(cat_f["s"]["2nd_Mode_Freq"]) + "," + str(cat_f["s"]["2nd_Mode%"]) + "\n")
f.close()
#generate the preprocessed data
f = open("preprocessed_googleplaystore_user_reviews.csv", "w", encoding = "UTF-8")
f1 = open("googleplaystore_user_reviews.csv", "r", encoding = "UTF-8")
buf = f1.readline()
f.write(buf)
f1.close()
feature = {"app": "app",
           "tr": "tr",
           "s": "s",
           "sp": "sp",
           "ss": "ss"
}
for e in gpsurList2:
    for i in feature:
        if e[i] == "nan" or e[i] == "NaN" or e[i] == "Varies with device":
            if i in con_f:
                f.write(str(con_f[feature[i]]["Mean"]))
            elif i in cat_f:
                f.write(str(cat_f[feature[i]]["Mode"]))
            else:
                print("Error: Cannot write into the file!")
        else:
            if i == "sp" or i == "ss":
                f.write(str(round(float(e[i]), 2)))
            else:
                f.write(str(e[i]))
        if i == "ss":
            f.write("\n")
        else:
            f.write(",")
f.close()

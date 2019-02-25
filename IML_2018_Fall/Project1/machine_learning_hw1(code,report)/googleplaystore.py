def RatingFunc(e):
    if e["Rating"] == "NaN":
        return 1000000000000000000000000.0
    else:
        return float(e["Rating"])
def ReviewsFunc(e):
    if e["Reviews"] == "NaN":
        return 1000000000000000000000000.0
    else:
        return float(e["Reviews"])
def SizeFunc(e):
    if e["Size"] == "NaN":
        return 1000000000000000000000000.0
    elif e["Size"] == "Varies with device":
        return 99999999999999999999999.0
    elif e["Size"][len(e["Size"]) - 1] == "k":
        return float(e["Size"].strip("k")) / 1000
    elif e["Size"][len(e["Size"]) - 1] == "M":
        return float(e["Size"].strip("M"))
    else:
        print("Error: No option!\n")
def InstallsFunc(e):
    if e["Installs"] == "NaN":
        return 1000000000000000000000000.0
    else:
        return float(e["Installs"].strip("+").replace(",", ""))
def PriceFunc(e):
    if e["Price"] == "NaN":
        return 1000000000000000000000000.0
    else:
        return float(e["Price"].strip("$"))
def AppFunc(e):
    return e["App"]
def CategoryFunc(e):
    return e["Category"]
def TypeFunc(e):
    return e["Type"]
def Content_RatingFunc(e):
    return e["Content_Rating"]
def GenresFunc(e):
    return e["Genres"]
def Last_UpdatedFunc(e):
    return e["Last_Updated"]
def Current_VerFunc(e):
    return e["Current_Ver"]
def Android_VerFunc(e):
    return e["Android_Ver"]
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
    if string == "Size":
        if remain != 0.0:
            if LIST[int(index)][string][len(LIST[int(index)][string]) - 1] == "k":
                return round(float(LIST[int(index)][string].strip("k")) / 1000, 2)
            elif LIST[int(index)][string][len(LIST[int(index)][string]) - 1] == "M":
                return round(float(LIST[int(index)][string].strip("M")), 2)
            else:
                print("Error: No option")
        else:
            if LIST[int(index) - 1][string][len(LIST[int(index) - 1][string]) - 1] == "k":
                a = float(LIST[int(index) - 1][string].strip("k")) / 1000
            elif LIST[int(index) - 1][string][len(LIST[int(index) - 1][string]) - 1] == "M":
                a = float(LIST[int(index) - 1][string].strip("M"))
            else:
                print("Error: No option")
            if LIST[int(index)][string][len(LIST[int(index)][string]) - 1] == "k":
                b = float(LIST[int(index)][string].strip("k")) / 1000
            elif LIST[int(index)][string][len(LIST[int(index)][string]) - 1] == "M":
                b = float(LIST[int(index)][string].strip("M"))
            else:
                print("Error: No option")
            return round((a + b) / 2, 2)
    elif string == "Installs":
        if remain != 0.0:
            return round(float(LIST[int(index)][string].strip("+").replace(",", "")), 2)
        else:
            return round((float(LIST[int(index) - 1][string].strip("+").replace(",", "")) + float(LIST[int(index)][string].strip("+").replace(",", ""))) / 2, 2)
    elif string == "Price":
        if remain != 0.0:
            return round(float(LIST[int(index)][string].strip("$")), 2)
        else:
            return round((float(LIST[int(index) - 1][string].strip("$")) + float(LIST[int(index)][string].strip("$"))) / 2, 2)
    else:
        if remain != 0.0:
            return round(float(LIST[int(index)][string]), 2)
        else:
            return round((float(LIST[int(index) - 1][string]) + float(LIST[int(index)][string])) / 2, 2)



#data quality reports of the continuous features and categorical features  
con_f = {"Rating": {},
         "Reviews": {},
         "Size": {},
         "Installs": {},
         "Price": {}
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
cat_f = {"App": {},
         "Category": {},
         "Type": {},
         "Content_Rating": {},
         "Genres": {},
         "Last_Updated": {},
         "Current_Ver": {},
         "Android_Ver": {}
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
gpsList = []
gpsList2 = [] #For the preprocessed file
gps = {"Android_Ver": "",
       "Current_Ver": "",
       "Last_Updated": "",
       "Genres": "",
       "Content_Rating": "",
       "Price": "",
       "Type": "",
       "Installs": "",
       "Size": "",
       "Reviews": "",
       "Rating": "",
       "Category": "",
       "App": ""
}
f = open("googleplaystore.csv", "r", encoding = "UTF-8")
f.readline()
count = 0
miss = {"Android_Ver": 0,
        "Current_Ver": 0,
        "Last_Updated": 0,
        "Genres": 0,
        "Content_Rating": 0,
        "Price": 0,
        "Type": 0,
        "Installs": 0,
        "Size": 0,
        "Reviews": 0,
        "Rating": 0,
        "Category": 0,
        "App": 0
}
for line in f:
    line2 = line.strip(" \n")
    if line2 == "Life Made WI-Fi Touchscreen Photo Frame,1.9,19,3.0M,\"1,000+\",Free,0,Everyone,,\"February 11, 2018\",1.0.19,4.0 and up":
        line2 = "Life Made WI-Fi Touchscreen Photo Frame,NaN,1.9,19,3.0M,\"1,000+\",Free,0,Everyone,NaN,\"February 11, 2018\",1.0.19,4.0 and up"
    if line2 == "Market Update Helper,LIBRARIES_AND_DEMO,4.1,20145,11k,\"1,000,000+\",Free,0,Everyone,Libraries & Demo,\"February 12, 2013\",,1.5 and up":
        line2 = "Market Update Helper,LIBRARIES_AND_DEMO,4.1,20145,11k,\"1,000,000+\",Free,0,Everyone,Libraries & Demo,\"February 12, 2013\",NaN,1.5 and up"
    idx1 = len(line2) - 1
    for i in gps:
        if i == "App":
            idx2 = -1
        elif line2[idx1] == "\"":
            idx2 = line2.rfind(",\"", 0, idx1)
        else:
            idx2 = line2.rfind(",", 0, idx1)
        gps[i] = line2[idx2 + 1: idx1 + 1].strip("\"")
        if gps[i] == "NaN":
            miss[i] += 1
        idx1 = idx2 - 1
    gpsList.append(gps.copy())
    gpsList2.append(gps.copy()) 
    count += 1
f.close()
"""
f = open("App.csv", "w")
for i in gpsList:
    f.write(i["App"] + "\n")
f.close()
f = open("Category.csv", "w")
for i in gpsList:
    f.write(i["Category"] + "\n")
f.close()
f = open("Rating.csv", "w")
for i in gpsList:
    f.write(i["Rating"] + "\n")
f.close()
f = open("Reviews.csv", "w")
for i in gpsList:
    f.write(i["Reviews"] + "\n")
f.close()
f = open("Size.csv", "w")
for i in gpsList:
    f.write(i["Size"] + "\n")
f.close()
f = open("Installs.csv", "w")
for i in gpsList:
    f.write(i["Installs"] + "\n")
f.close()
f = open("Type.csv", "w")
for i in gpsList:
    f.write(i["Type"] + "\n")
f.close()
f = open("Price.csv", "w")
for i in gpsList:
    f.write(i["Price"] + "\n")
f.close()
f = open("Content_Rating.csv", "w")
for i in gpsList:
    f.write(i["Content_Rating"] + "\n")
f.close()
f = open("Genres.csv", "w")
for i in gpsList:
    f.write(i["Genres"] + "\n")
f.close()
f = open("Last_Updated.csv", "w")
for i in gpsList:
    f.write(i["Last_Updated"] + "\n")
f.close()
f = open("Current_Ver.csv", "w")
for i in gpsList:
    f.write(i["Current_Ver"] + "\n")
f.close()
f = open("Android_Ver.csv", "w")
for i in gpsList:
    f.write(i["Android_Ver"] + "\n")
f.close()
"""
#count and miss rate in con_f and cat_f
for i in con_f:
    con_f[i]["Count"] = count
for i in cat_f:
    cat_f[i]["Count"] = count
for i in con_f:
    con_f[i]["Miss%"] = round(miss[i] * 100 / count, 2)
for i in cat_f:
    cat_f[i]["Miss%"] = round(miss[i] * 100 / count, 2)
count_varies = 0
for element in gpsList:
    for i in con_f:
        if i == "Size":
            if element[i] != "NaN":
                if element[i][len(element[i]) - 1] == "k":
                    con_f[i]["Mean"] += (float(element[i].strip("k")) / 1000)
                elif element[i][len(element[i]) - 1] == "M":
                    con_f[i]["Mean"] += float(element[i].strip("M"))
                elif element[i] == "Varies with device":
                    count_varies += 1
        elif i == "Installs":
            if element[i] != "NaN":
                con_f[i]["Mean"] += float(element[i].strip("+").replace(",", ""))
        elif i == "Price":
            if element[i] != "NaN":
                con_f[i]["Mean"] += float(element[i].strip("$"))
        else:
            if element[i] != "NaN":
                con_f[i]["Mean"] += float(element[i])
#mean in con_f
for i in con_f:
    if i == "Size":
        con_f[i]["Mean"] = round(con_f[i]["Mean"] / (count - miss[i] - count_varies), 2)
    else:
        con_f[i]["Mean"] = round(con_f[i]["Mean"] / (count - miss[i]), 2)

for element in gpsList:
    for i in con_f:
        if i == "Size":
            if element[i] != "NaN":
                if element[i][len(element[i]) - 1] == "k":
                    con_f[i]["Std_Dev"] += pow((float(element[i].strip("k")) / 1000) - con_f[i]["Mean"], 2)
                elif element[i][len(element[i]) - 1] == "M":
                    con_f[i]["Std_Dev"] += pow(float(element[i].strip("M")) - con_f[i]["Mean"], 2)
        elif i == "Installs":
            if element[i] != "NaN":
                con_f[i]["Std_Dev"] += pow(float(element[i].strip("+").replace(",", "")) - con_f[i]["Mean"], 2)
        elif i == "Price":
            if element[i] != "NaN":
                con_f[i]["Std_Dev"] += pow(float(element[i].strip("$")) - con_f[i]["Mean"], 2)
        else:
            if element[i] != "NaN":
                con_f[i]["Std_Dev"] += pow(float(element[i]) - con_f[i]["Mean"], 2)

#standard deviation in con_f
for i in con_f:
    if i == "Size":
        con_f[i]["Std_Dev"] = round(pow(con_f[i]["Std_Dev"] / (count - miss[i] - count_varies), 0.5), 2)
    else:
        con_f[i]["Std_Dev"] = round(pow(con_f[i]["Std_Dev"] / (count - miss[i]), 0.5), 2)
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of Rating in con_f
gpsList.sort(key = RatingFunc)
con_f["Rating"]["Min"] = round(float(gpsList[0]["Rating"]), 2) 
con_f["Rating"]["1stQrt"] = Q(gpsList, "Rating", count - miss["Rating"], 1)
con_f["Rating"]["Median"] = Q(gpsList, "Rating", count - miss["Rating"], 2)
con_f["Rating"]["3rdQrt"] = Q(gpsList, "Rating", count - miss["Rating"], 3)
con_f["Rating"]["Max"] = round(float(gpsList[count - miss["Rating"] - 1]["Rating"]), 2)
buf = []
buf.append(gpsList[0]["Rating"])
j = 0
i = 1
while i < count - miss["Rating"]:
    if gpsList[i]["Rating"] != buf[j]:
        buf.append(gpsList[i]["Rating"])
        j += 1
    i += 1
con_f["Rating"]["Card"] = len(buf)
buf.clear()
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of Reviews in con_f
gpsList.sort(key = ReviewsFunc)
con_f["Reviews"]["Min"] = round(float(gpsList[0]["Reviews"]), 2) 
con_f["Reviews"]["1stQrt"] = Q(gpsList, "Reviews", count - miss["Reviews"], 1)
con_f["Reviews"]["Median"] = Q(gpsList, "Reviews", count - miss["Reviews"], 2)
con_f["Reviews"]["3rdQrt"] = Q(gpsList, "Reviews", count - miss["Reviews"], 3)
con_f["Reviews"]["Max"] = round(float(gpsList[count - miss["Reviews"] - 1]["Reviews"]), 2)
buf = []
buf.append(gpsList[0]["Reviews"])
j = 0
i = 1
while i < count - miss["Reviews"]:
    if gpsList[i]["Reviews"] != buf[j]:
        buf.append(gpsList[i]["Reviews"])
        j += 1
    i += 1
con_f["Reviews"]["Card"] = len(buf)
buf.clear()
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of Size in con_f
gpsList.sort(key = SizeFunc)
if gpsList[0]["Size"][len(gpsList[0]["Size"]) - 1] == "k":
    con_f["Size"]["Min"] = round(float(gpsList[0]["Size"].strip("k")) / 1000, 2) 
elif gpsList[0]["Size"][len(gpsList[0]["Size"]) - 1] == "M":
    con_f["Size"]["Min"] = round(float(gpsList[0]["Size"].strip("M")), 2) 
else: 
    print("Error: No option")
con_f["Size"]["1stQrt"] = Q(gpsList, "Size", count - miss["Size"] - count_varies, 1)
con_f["Size"]["Median"] = Q(gpsList, "Size", count - miss["Size"] - count_varies, 2)
con_f["Size"]["3rdQrt"] = Q(gpsList, "Size", count - miss["Size"] - count_varies, 3)
if gpsList[count - miss["Size"] - count_varies - 1]["Size"][len(gpsList[count - miss["Size"] - count_varies - 1]["Size"]) - 1] == "k":
    con_f["Size"]["Max"] = round(float(gpsList[count - miss["Size"] - count_varies - 1]["Size"].strip("k")) / 1000, 2) 
elif gpsList[count - miss["Size"] - count_varies - 1]["Size"][len(gpsList[count - miss["Size"] - count_varies - 1]["Size"]) - 1] == "M":
    con_f["Size"]["Max"] = round(float(gpsList[count - miss["Size"] - count_varies - 1]["Size"].strip("M")), 2)
else: 
    print("Error: No option!")
buf = []
buf.append(gpsList[0]["Size"])
j = 0
i = 1
while i < count - miss["Size"]:
    if gpsList[i]["Size"] != buf[j]:
        buf.append(gpsList[i]["Size"])
        j += 1
    i += 1
con_f["Size"]["Card"] = len(buf)
buf.clear()
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of Installs in con_f
gpsList.sort(key = InstallsFunc)
con_f["Installs"]["Min"] = round(float(gpsList[0]["Installs"].strip("+").replace(",", "")), 2) 
con_f["Installs"]["1stQrt"] = Q(gpsList, "Installs", count - miss["Installs"], 1)
con_f["Installs"]["Median"] = Q(gpsList, "Installs", count - miss["Installs"], 2)
con_f["Installs"]["3rdQrt"] = Q(gpsList, "Installs", count - miss["Installs"], 3)
con_f["Installs"]["Max"] = round(float(gpsList[count - miss["Installs"] - 1]["Installs"].strip("+").replace(",", "")), 2)
buf = []
buf.append(gpsList[0]["Installs"])
j = 0
i = 1
while i < count - miss["Installs"]:
    if gpsList[i]["Installs"] != buf[j]:
        buf.append(gpsList[i]["Installs"])
        j += 1
    i += 1
con_f["Installs"]["Card"] = len(buf)
buf.clear()
#Min, 1stQrt, Median, 3rdQrt, Max and Cardinality of Price in con_f
gpsList.sort(key = PriceFunc)
con_f["Price"]["Min"] = round(float(gpsList[0]["Price"].strip("$")), 2) 
con_f["Price"]["1stQrt"] = Q(gpsList, "Price", count - miss["Price"], 1)
con_f["Price"]["Median"] = Q(gpsList, "Price", count - miss["Price"], 2)
con_f["Price"]["3rdQrt"] = Q(gpsList, "Price", count - miss["Price"], 3)
con_f["Price"]["Max"] = round(float(gpsList[count - miss["Price"] - 1]["Price"].strip("$")), 2)
buf = []
buf.append(gpsList[0]["Price"])
j = 0
i = 1
while i < count - miss["Price"]:
    if gpsList[i]["Price"] != buf[j]:
        buf.append(gpsList[i]["Price"])
        j += 1
    i += 1
con_f["Price"]["Card"] = len(buf)
buf.clear()
#Cardinality of App in cat_f
gpsList.sort(key = AppFunc)
buf.append(gpsList[0]["App"])
j = 0
i = 1
while i < count:
    if gpsList[i]["App"] != buf[j]:
        buf.append(gpsList[i]["App"])
        j += 1
    i += 1
if (miss["App"]) > 0:
    buf.remove("NaN")
cat_f["App"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each App
i = 0
j = 0
times = 0
while i < count:
    if gpsList[i]["App"] == buf[j]:
        times += 1
    elif gpsList[i]["App"] != "NaN": 
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
#Mode, Mode_Freq, Mode% of App in cat_f
cat_f["App"]["Mode"] = buf[maxIndex]
cat_f["App"]["Mode_Freq"] = buf2[maxIndex]
cat_f["App"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["App"]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of App in cat_f
cat_f["App"]["2nd_Mode"] = buf[maxIndex]
cat_f["App"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["App"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["App"]), 2)
buf.clear()
buf2.clear()
#Cardinality of Category in cat_f
gpsList.sort(key = CategoryFunc)
buf.append(gpsList[0]["Category"])
j = 0
i = 1
while i < count:
    if gpsList[i]["Category"] != buf[j]:
        buf.append(gpsList[i]["Category"])
        j += 1
    i += 1
if (miss["Category"]) > 0:
    buf.remove("NaN")
cat_f["Category"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each Category
i = 0
j = 0
times = 0
while i < count:
    if gpsList[i]["Category"] == buf[j]:
        times += 1
    elif gpsList[i]["Category"] != "NaN": 
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
#Mode, Mode_Freq, Mode% of Category in cat_f
cat_f["Category"]["Mode"] = buf[maxIndex]
cat_f["Category"]["Mode_Freq"] = buf2[maxIndex]
cat_f["Category"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Category"]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of Category in cat_f
cat_f["Category"]["2nd_Mode"] = buf[maxIndex]
cat_f["Category"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["Category"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Category"]), 2)
buf.clear()
buf2.clear()
#Cardinality of Type in cat_f
gpsList.sort(key = TypeFunc)
buf.append(gpsList[0]["Type"])
j = 0
i = 1
while i < count:
    if gpsList[i]["Type"] != buf[j]:
        buf.append(gpsList[i]["Type"])
        j += 1
    i += 1
if (miss["Type"]) > 0:
    buf.remove("NaN")
cat_f["Type"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each Type
i = 0
j = 0
times = 0
while i < count:
    if gpsList[i]["Type"] == buf[j]:
        times += 1
    elif gpsList[i]["Type"] != "NaN": 
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
#Mode, Mode_Freq, Mode% of Type in cat_f
cat_f["Type"]["Mode"] = buf[maxIndex]
cat_f["Type"]["Mode_Freq"] = buf2[maxIndex]
cat_f["Type"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Type"]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of Type in cat_f
cat_f["Type"]["2nd_Mode"] = buf[maxIndex]
cat_f["Type"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["Type"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Type"]), 2)
buf.clear()
buf2.clear()
#Cardinality of Content_Rating in cat_f
gpsList.sort(key = Content_RatingFunc)
buf.append(gpsList[0]["Content_Rating"])
j = 0
i = 1
while i < count:
    if gpsList[i]["Content_Rating"] != buf[j]:
        buf.append(gpsList[i]["Content_Rating"])
        j += 1
    i += 1
if (miss["Content_Rating"]) > 0:
    buf.remove("NaN")
cat_f["Content_Rating"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each Content_Rating
i = 0
j = 0
times = 0
while i < count:
    if gpsList[i]["Content_Rating"] == buf[j]:
        times += 1
    elif gpsList[i]["Content_Rating"] != "NaN": 
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
#Mode, Mode_Freq, Mode% of Content_Rating in cat_f
cat_f["Content_Rating"]["Mode"] = buf[maxIndex]
cat_f["Content_Rating"]["Mode_Freq"] = buf2[maxIndex]
cat_f["Content_Rating"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Content_Rating"]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of Content_Rating in cat_f
cat_f["Content_Rating"]["2nd_Mode"] = buf[maxIndex]
cat_f["Content_Rating"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["Content_Rating"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Content_Rating"]), 2)
buf.clear()
buf2.clear()
#Cardinality of Genres in cat_f
gpsList.sort(key = GenresFunc)
buf.append(gpsList[0]["Genres"])
j = 0
i = 1
while i < count:
    if gpsList[i]["Genres"] != buf[j]:
        buf.append(gpsList[i]["Genres"])
        j += 1
    i += 1
if (miss["Genres"]) > 0:
    buf.remove("NaN")
cat_f["Genres"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each Genres
i = 0
j = 0
times = 0
while i < count:
    if gpsList[i]["Genres"] == buf[j]:
        times += 1
    elif gpsList[i]["Genres"] != "NaN": 
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
#Mode, Mode_Freq, Mode% of Genres in cat_f
cat_f["Genres"]["Mode"] = buf[maxIndex]
cat_f["Genres"]["Mode_Freq"] = buf2[maxIndex]
cat_f["Genres"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Genres"]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of Genres in cat_f
cat_f["Genres"]["2nd_Mode"] = buf[maxIndex]
cat_f["Genres"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["Genres"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Genres"]), 2)
buf.clear()
buf2.clear()
#Cardinality of Last_Updated in cat_f
gpsList.sort(key = Last_UpdatedFunc)
buf.append(gpsList[0]["Last_Updated"])
j = 0
i = 1
while i < count:
    if gpsList[i]["Last_Updated"] != buf[j]:
        buf.append(gpsList[i]["Last_Updated"])
        j += 1
    i += 1
if (miss["Last_Updated"]) > 0:
    buf.remove("NaN")
cat_f["Last_Updated"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each Last_Updated
i = 0
j = 0
times = 0
while i < count:
    if gpsList[i]["Last_Updated"] == buf[j]:
        times += 1
    elif gpsList[i]["Last_Updated"] != "NaN": 
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
#Mode, Mode_Freq, Mode% of Last_Updated in cat_f
cat_f["Last_Updated"]["Mode"] = buf[maxIndex]
cat_f["Last_Updated"]["Mode_Freq"] = buf2[maxIndex]
cat_f["Last_Updated"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Last_Updated"]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of Last_Updated in cat_f
cat_f["Last_Updated"]["2nd_Mode"] = buf[maxIndex]
cat_f["Last_Updated"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["Last_Updated"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Last_Updated"]), 2)
buf.clear()
buf2.clear()
#Cardinality of Current_Ver in cat_f
gpsList.sort(key = Current_VerFunc)
buf.append(gpsList[0]["Current_Ver"])
j = 0
i = 1
while i < count:
    if gpsList[i]["Current_Ver"] != buf[j]:
        buf.append(gpsList[i]["Current_Ver"])
        j += 1
    i += 1
if (miss["Current_Ver"]) > 0:
    buf.remove("NaN")
cat_f["Current_Ver"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each Current_Ver
i = 0
j = 0
times = 0
while i < count:
    if gpsList[i]["Current_Ver"] == buf[j]:
        times += 1
    elif gpsList[i]["Current_Ver"] != "NaN": 
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
#Mode, Mode_Freq, Mode% of Current_Ver in cat_f
cat_f["Current_Ver"]["Mode"] = buf[maxIndex]
cat_f["Current_Ver"]["Mode_Freq"] = buf2[maxIndex]
cat_f["Current_Ver"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Current_Ver"]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of Current_Ver in cat_f
cat_f["Current_Ver"]["2nd_Mode"] = buf[maxIndex]
cat_f["Current_Ver"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["Current_Ver"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Current_Ver"]), 2)
buf.clear()
buf2.clear()
#Cardinality of Android_Ver in cat_f
gpsList.sort(key = Android_VerFunc)
buf.append(gpsList[0]["Android_Ver"])
j = 0
i = 1
while i < count:
    if gpsList[i]["Android_Ver"] != buf[j]:
        buf.append(gpsList[i]["Android_Ver"])
        j += 1
    i += 1
if (miss["Android_Ver"]) > 0:
    buf.remove("NaN")
cat_f["Android_Ver"]["Card"] = len(buf)
buf2 = [] #calculate times of the appearance of each Android_Ver
i = 0
j = 0
times = 0
while i < count:
    if gpsList[i]["Android_Ver"] == buf[j]:
        times += 1
    elif gpsList[i]["Android_Ver"] != "NaN": 
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
#Mode, Mode_Freq, Mode% of Android_Ver in cat_f
cat_f["Android_Ver"]["Mode"] = buf[maxIndex]
cat_f["Android_Ver"]["Mode_Freq"] = buf2[maxIndex]
cat_f["Android_Ver"]["Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Android_Ver"]), 2)
#find the second most appearances
buf2[maxIndex] = 0
maxIndex = 0 
i = 0
while i < len(buf):
    if (buf2[i] > buf2[maxIndex]):
        maxIndex = i
    i += 1
#2nd_Mode, 2nd_Mode_Freq, 2nd_Mode% of Android_Ver in cat_f
cat_f["Android_Ver"]["2nd_Mode"] = buf[maxIndex]
cat_f["Android_Ver"]["2nd_Mode_Freq"] = buf2[maxIndex]
cat_f["Android_Ver"]["2nd_Mode%"] = round(buf2[maxIndex] * 100 / (count - miss["Android_Ver"]), 2)
buf.clear()
buf2.clear()

#print the result to a file
f = open("googleplaystore_data_quality_report.csv", "w", encoding = "UTF-8")
f.write("Continuous Features\n")
f.write("Feature,Count,Miss%,Card,Min,1st Qrt,Mean,Median,3rd Qrt,Max,Std Dev\n")
f.write("Rating," + str(con_f["Rating"]["Count"]) + "," + str(con_f["Rating"]["Miss%"]) + "," + str(con_f["Rating"]["Card"]) + "," + str(con_f["Rating"]["Min"]) + "," + str(con_f["Rating"]["1stQrt"]) + "," + str(con_f["Rating"]["Mean"]) + "," + str(con_f["Rating"]["Median"]) + "," + str(con_f["Rating"]["3rdQrt"]) + "," + str(con_f["Rating"]["Max"]) + "," + str(con_f["Rating"]["Std_Dev"]) + "\n")
f.write("Reviews," + str(con_f["Reviews"]["Count"]) + "," + str(con_f["Reviews"]["Miss%"]) + "," + str(con_f["Reviews"]["Card"]) + "," + str(con_f["Reviews"]["Min"]) + "," + str(con_f["Reviews"]["1stQrt"]) + "," + str(con_f["Reviews"]["Mean"]) + "," + str(con_f["Reviews"]["Median"]) + "," + str(con_f["Reviews"]["3rdQrt"]) + "," + str(con_f["Reviews"]["Max"]) + "," + str(con_f["Reviews"]["Std_Dev"]) + "\n")
f.write("Size," + str(con_f["Size"]["Count"]) + "," + str(con_f["Size"]["Miss%"]) + "," + str(con_f["Size"]["Card"]) + "," + str(con_f["Size"]["Min"]) + "M" + "," + str(con_f["Size"]["1stQrt"]) + "M" + "," + str(con_f["Size"]["Mean"]) + "M" + "," + str(con_f["Size"]["Median"]) + "M" + "," + str(con_f["Size"]["3rdQrt"]) + "M" + "," + str(con_f["Size"]["Max"]) + "M" + "," + str(con_f["Size"]["Std_Dev"]) + "M" + "\n")
f.write("Installs," + str(con_f["Installs"]["Count"]) + "," + str(con_f["Installs"]["Miss%"]) + "," + str(con_f["Installs"]["Card"]) + "," + str(con_f["Installs"]["Min"]) + "+" + "," + str(con_f["Installs"]["1stQrt"]) + "+" + "," + str(con_f["Installs"]["Mean"]) + "+" + "," + str(con_f["Installs"]["Median"]) + "+" + "," + str(con_f["Installs"]["3rdQrt"]) + "+" + "," + str(con_f["Installs"]["Max"]) + "+" + "," + str(con_f["Installs"]["Std_Dev"]) + "+" + "\n")
f.write("Price," + str(con_f["Price"]["Count"]) + "," + str(con_f["Price"]["Miss%"]) + "," + str(con_f["Price"]["Card"]) + "," + "$" + str(con_f["Price"]["Min"]) + "," + "$" + str(con_f["Price"]["1stQrt"])  + "," + "$" + str(con_f["Price"]["Mean"]) + "," + "$" + str(con_f["Price"]["Median"]) + "," + "$" + str(con_f["Price"]["3rdQrt"]) + "," + "$" + str(con_f["Price"]["Max"]) + "," + "$" + str(con_f["Price"]["Std_Dev"]) + "\n")
f.write("\n")
f.write("Categorical Features\n")
f.write("Feature,Count,Miss%,Card,Mode,Mode Freq,Mode%,2nd Mode,2nd Mode Freq,2nd Mode%\n")
f.write("App name," + str(cat_f["App"]["Count"]) + "," + str(cat_f["App"]["Miss%"]) + "," + str(cat_f["App"]["Card"]) + ",")
if cat_f["App"]["Mode"].find(",") == -1:
    f.write(str(cat_f["App"]["Mode"]))
else:
    f.write("\"" + str(cat_f["App"]["Mode"]) + "\"")
f.write("," + str(cat_f["App"]["Mode_Freq"]) + "," + str(cat_f["App"]["Mode%"]) + ",")
if cat_f["App"]["2nd_Mode"].find(",") == -1:
    f.write(str(cat_f["App"]["2nd_Mode"]))
else:
    f.write("\"" + str(cat_f["App"]["2nd_Mode"]) + "\"")
f.write("," + str(cat_f["App"]["2nd_Mode_Freq"]) + "," + str(cat_f["App"]["2nd_Mode%"]) + "\n")
f.write("Category," + str(cat_f["Category"]["Count"]) + "," + str(cat_f["Category"]["Miss%"]) + "," + str(cat_f["Category"]["Card"]) + "," + str(cat_f["Category"]["Mode"]) + "," + str(cat_f["Category"]["Mode_Freq"]) + "," + str(cat_f["Category"]["Mode%"]) + "," + str(cat_f["Category"]["2nd_Mode"]) + "," + str(cat_f["Category"]["2nd_Mode_Freq"]) + "," + str(cat_f["Category"]["2nd_Mode%"]) + "\n")
f.write("Type," + str(cat_f["Type"]["Count"]) + "," + str(cat_f["Type"]["Miss%"]) + "," + str(cat_f["Type"]["Card"]) + "," + str(cat_f["Type"]["Mode"]) + "," + str(cat_f["Type"]["Mode_Freq"]) + "," + str(cat_f["Type"]["Mode%"]) + "," + str(cat_f["Type"]["2nd_Mode"]) + "," + str(cat_f["Type"]["2nd_Mode_Freq"]) + "," + str(cat_f["Type"]["2nd_Mode%"]) + "\n")
f.write("Content Rating," + str(cat_f["Content_Rating"]["Count"]) + "," + str(cat_f["Content_Rating"]["Miss%"]) + "," + str(cat_f["Content_Rating"]["Card"]) + "," + str(cat_f["Content_Rating"]["Mode"]) + "," + str(cat_f["Content_Rating"]["Mode_Freq"]) + "," + str(cat_f["Content_Rating"]["Mode%"]) + "," + str(cat_f["Content_Rating"]["2nd_Mode"]) + "," + str(cat_f["Content_Rating"]["2nd_Mode_Freq"]) + "," + str(cat_f["Content_Rating"]["2nd_Mode%"]) + "\n")
f.write("Genres," + str(cat_f["Genres"]["Count"]) + "," + str(cat_f["Genres"]["Miss%"]) + "," + str(cat_f["Genres"]["Card"]) + "," + str(cat_f["Genres"]["Mode"]) + "," + str(cat_f["Genres"]["Mode_Freq"]) + "," + str(cat_f["Genres"]["Mode%"]) + "," + str(cat_f["Genres"]["2nd_Mode"]) + "," + str(cat_f["Genres"]["2nd_Mode_Freq"]) + "," + str(cat_f["Genres"]["2nd_Mode%"]) + "\n")
f.write("Last Updated," + str(cat_f["Last_Updated"]["Count"]) + "," + str(cat_f["Last_Updated"]["Miss%"]) + "," + str(cat_f["Last_Updated"]["Card"]) + "," + "\"" + str(cat_f["Last_Updated"]["Mode"]) + "\"" + "," + str(cat_f["Last_Updated"]["Mode_Freq"]) + "," + str(cat_f["Last_Updated"]["Mode%"]) + "," + "\"" +  str(cat_f["Last_Updated"]["2nd_Mode"]) + "\"" + "," + str(cat_f["Last_Updated"]["2nd_Mode_Freq"]) + "," + str(cat_f["Last_Updated"]["2nd_Mode%"]) + "\n")
f.write("Current Ver," + str(cat_f["Current_Ver"]["Count"]) + "," + str(cat_f["Current_Ver"]["Miss%"]) + "," + str(cat_f["Current_Ver"]["Card"]) + ",")
if cat_f["Current_Ver"]["Mode"].find(",") == -1:
    f.write(str(cat_f["Current_Ver"]["Mode"]))
else:
    f.write("\"" + str(cat_f["Current_Ver"]["Mode"]) + "\"")
f.write("," + str(cat_f["Current_Ver"]["Mode_Freq"]) + "," + str(cat_f["Current_Ver"]["Mode%"]) + ",")
if cat_f["Current_Ver"]["2nd_Mode"].find(",") == -1:
    f.write(str(cat_f["Current_Ver"]["2nd_Mode"]))
else:
    f.write("\"" + str(cat_f["Current_Ver"]["2nd_Mode"]) + "\"")
f.write("," + str(cat_f["Current_Ver"]["2nd_Mode_Freq"]) + "," + str(cat_f["Current_Ver"]["2nd_Mode%"]) + "\n")
f.write("Android Ver," + str(cat_f["Android_Ver"]["Count"]) + "," + str(cat_f["Android_Ver"]["Miss%"]) + "," + str(cat_f["Android_Ver"]["Card"]) + "," + "\"" + str(cat_f["Android_Ver"]["Mode"]) + "\"" + "," + str(cat_f["Android_Ver"]["Mode_Freq"]) + "," + str(cat_f["Android_Ver"]["Mode%"]) + "," + "\"" +  str(cat_f["Android_Ver"]["2nd_Mode"]) + "\"" + "," + str(cat_f["Android_Ver"]["2nd_Mode_Freq"]) + "," + str(cat_f["Android_Ver"]["2nd_Mode%"]) + "\n")
f.close()
#generate the preprocessed data 
f = open("preprocessed_googleplaystore.csv", "w", encoding = "UTF-8")
f1 = open("googleplaystore.csv", "r", encoding = "UTF-8")
buf = f1.readline()
f.write(buf)
f1.close()
feature = {"App": "App",
           "Category": "Category",
           "Rating": "Rating",
           "Reviews": "Reviews",
           "Size": "Size",
           "Installs": "Installs",
           "Type": "Type",
           "Price": "Price",
           "Content_Rating": "Content_Rating",
           "Genres": "Genres",
           "Last_Updated": "Last_Updated",
           "Current_Ver": "Current_Ver",
           "Android_Ver": "Android_Ver"
}
for e in gpsList2:
    for i in feature:
        if e[i] == "nan" or e[i] == "NaN" or e[i] == "Varies with device":
            if i in con_f:
                if i == "Size":
                    f.write(str(con_f[feature[i]]["Mean"]) + "M")
                elif i == "Installs":
                    f.write(str(con_f[feature[i]]["Mean"]) + "+")
                elif i == "Price":
                    f.write("$" + str(con_f[feature[i]]["Mean"]))
                else: 
                    f.write(str(con_f[feature[i]]["Mean"]))
            elif i in cat_f:
                if e[i] == "Varies with device":
                    f.write(e[i])
                elif cat_f[feature[i]]["Mode"].find(",") == -1: 
                    f.write(str(cat_f[feature[i]]["Mode"]))
                else:
                    f.write("\"" + str(cat_f[feature[i]]["Mode"]) + "\"")
            else:
                print("Error: Cannot write into the file!")
        else:
            if i == "Installs":
              f.write(e[i].replace(",", ""))
            else:
                if e[i].find(",") == -1: 
                    f.write(e[i])
                else:
                    f.write("\"" + e[i] + "\"")
        if i == "Android_Ver":
            f.write("\n")
        else:
            f.write(",")
f.close()

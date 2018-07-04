#!/bin/sh

# Ask user to enter account:
while [ "$statuscode" != "exit" ]
do
	exec 3>&1;
	account=$(dialog --title "Account" --inputbox "Enter your account" 0 0 2>&1 1>&3);
	exitcode=$?;
	exec 3>&-;

	case $exitcode in
	0)
		if echo $account |grep -Eq '^[0][0-9]{6}$'; then
			statuscode="exit"
		fi
		;;
	1)
		dialog --title "Error" --msgbox 'User canceled' 6 20
		exit
		;;
	255)
		dialog --title "Error" --msgbox '[ESC] Pressed by user' 6 20
		exit
		;;
	esac
done

# Initialize statuscode
statuscode="continue"

# Ask user to enter password:
while [ "$statuscode" != "exit" ]
do
	exec 3>&1;
	password=$(dialog --title "Password" --clear --insecure --passwordbox "Enter your password" 0 0 2>&1 1>&3);
	exitcode=$?;
	exec 3>&-;

	case $exitcode in
	0)
		statuscode="exit"
		;;
	1)
		dialog --title "Error" --msgbox 'User canceled' 6 20
		exit
		;;
	255)
		dialog --title "Error" --msgbox '[ESC] Pressed by user' 6 20
		exit
		;;
	esac
done

# Connect to nctu portal and get captcha
clear
echo "Solving captcha..."
while [ "$len" != 4 ]
do
	curl -s \
		-X GET \
		-H "Content-Type: multipart/form-data" \
		-c cookie.txt \
		-L \
		https://portal.nctu.edu.tw/captcha/pic.php > /dev/null

	curl -s \
		-X GET \
		-b cookie.txt \
		https://portal.nctu.edu.tw/captcha/pitctest/pic.php > image_code.png

	captcha=$(curl -s \
					-H "Content-Type: multipart/form-data" \
					-F "image=@image_code.png" \
					https://nasa.cs.nctu.edu.tw/sap/2017/hw2/captcha-solver/api/)
	
	len=${#captcha}
	oLang=$LANG
	LANG=C
done

echo "Login nctu portal..."
	# Connect to nctu portal and enter account, passwd, captcha
curl -s \
	-H "Content-Type: multipart/form-data; charset=UTF-8" \
	-b cookie.txt \
	-F "username=${account}" \
	-F "password=${password}" \
	-F "seccode=${captcha}" \
	-F "pwdtype=static" \
	-F "Submit2=登入(Login)" \
	-L \
	-c cookie.txt \
	https://portal.nctu.edu.tw/portal/chkpas.php? > /dev/null

formdata=$(
curl -s \
	-b cookie.txt \
	-c cookie.txt \
	-L \
	https://portal.nctu.edu.tw/portal/relay.php?D=cos | 
	node ~/SA_HW/hw2/sa2017-hw2/extractFormdata/extractFormdata.js)

curl -s \
	-X GET \
	-H "Content-Type: multipart/form-data" \
	-b cookie.txt \
	-c cookie.txt \
	-L \
	https://course.nctu.edu.tw/index.asp?${formdata} > /dev/null

echo "Printing course schedule..."
clear
curl -s \
	-H "Content-Type: multipart/form-data" \
	-b cookie.txt \
	-L \
	https://course.nctu.edu.tw/adSchedule.asp | iconv -f big5 -t utf-8 | sed 1,152d  | 
	awk 'function ltrim(s)  { sub(/^[ \t\r\n]+/, "", s); return s }
	function rtrim(s)  { sub(/[ \t\r\n]+$/, "", s); return s }
	function trim(s)   { return rtrim(ltrim(s)); }
	function trimbr(s) { sub(/<br>/, "", s); return s }
	BEGIN {
    	  printf("-- -- Mon Tue Wed Thr Fri Sat Sun \n\r");
	}
	match($0, /<td VALIGN="TOP"/){ getline; t = trim($0); if (t == "") printf("-- "); else printf("%s ", trimbr(t)); }
	match($0, /<\/tr>/) { printf("\n\r"); }' | 
	column -t

# Delete temp files
rm -rf image_code.png
rm -rf cookie.txt

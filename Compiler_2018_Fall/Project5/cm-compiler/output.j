; output.j
.class public output
.super java/lang/Object
.field public static _sc Ljava/util/Scanner;

; ########## main function ##########
.method public static main([Ljava/lang/String;)V
.limit stack 500
.limit locals 500
	new java/util/Scanner
	dup
	getstatic java/lang/System/in Ljava/io/InputStream;
	invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V
	putstatic output/_sc Ljava/util/Scanner;
; === assign global variable's value ===
; ======================================
ldc 1 ; is integer
ldc 0 ; is integer
istore 1
istore 0
Lbegin_0:
iload 0 ; var name: i
ldc 100 ; is integer
isub
ifle Ltrue_1
iconst_0
goto Lfalse_1
Ltrue_1:
iconst_1
Lfalse_1:
ifeq Lexit_0
iload 1 ; var name: answer
iload 0 ; var name: i
iadd
istore 1 ; save to answer 
iload 0 ; var name: i
ldc 1 ; is integer
iadd
istore 0 ; save to i 
goto Lbegin_0
Lexit_0:

;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 0 ; var name: i
invokevirtual java/io/PrintStream/print(I)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc " "
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 1 ; var name: answer
invokevirtual java/io/PrintStream/print(I)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc "
"
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###

ldc 0 ; is integer
return
.end method

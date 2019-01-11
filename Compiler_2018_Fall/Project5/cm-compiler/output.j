; output.j
.class public output
.super java/lang/Object
.field public static _sc Ljava/util/Scanner;
.field public static intvalue I
.field public static floatvalue F

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
;    ### Start read ###
getstatic output/_sc Ljava/util/Scanner;
invokevirtual java/util/Scanner/nextInt()I
istore 1 ; local var num of integer_a is 1
;    ### Start read ###
getstatic output/_sc Ljava/util/Scanner;
invokevirtual java/util/Scanner/nextInt()I
istore 2 ; local var num of integer_b is 2
;    ### Start read ###
getstatic output/_sc Ljava/util/Scanner;
invokevirtual java/util/Scanner/nextInt()I
istore 3 ; local var num of integer_c is 3
;    ### Start read ###
getstatic output/_sc Ljava/util/Scanner;
invokevirtual java/util/Scanner/nextFloat()F
fstore 5 ; local var num of float_e is 5
;    ### Start read ###
getstatic output/_sc Ljava/util/Scanner;
invokevirtual java/util/Scanner/nextFloat()F
fstore 6 ; local var num of float_f is 6

;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc "--------------------
"
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 3 ; var name: integer_c
invokevirtual java/io/PrintStream/print(I)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc " "
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 1 ; var name: integer_a
invokevirtual java/io/PrintStream/print(I)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc " "
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 2 ; var name: integer_b
invokevirtual java/io/PrintStream/print(I)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc " "
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
fload 5 ; var name: float_e
invokevirtual java/io/PrintStream/print(F)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc " "
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
fload 6 ; var name: float_f
invokevirtual java/io/PrintStream/print(F)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc "
"
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###

iload 2 ; var name: integer_b
putstatic output/intvalue I
iload 3 ; var name: integer_c
istore 2 ; save to integer_b 
getstatic output/intvalue I
istore 3 ; save to integer_c 

;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 2 ; var name: integer_b
invokevirtual java/io/PrintStream/print(I)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc " "
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 3 ; var name: integer_c
invokevirtual java/io/PrintStream/print(I)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc "
"
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###

fload 6 ; var name: float_f
putstatic output/floatvalue F
fload 5 ; var name: float_e
fstore 6 ; save to float_f 
getstatic output/floatvalue F
fstore 5 ; save to float_e 

;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
fload 5 ; var name: float_e
invokevirtual java/io/PrintStream/print(F)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc " "
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
fload 6 ; var name: float_f
invokevirtual java/io/PrintStream/print(F)V
;    ### End of print ###


;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc "
"
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###

iconst_1 ; is bool
istore 4  ; save to bool_d 

;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 4 ; var name: bool_d
invokevirtual java/io/PrintStream/print(Z)V
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

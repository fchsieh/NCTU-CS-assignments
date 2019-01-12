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

;    ### Start print ###
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc "hello world!
"
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
;    ### End of print ###

return
.end method

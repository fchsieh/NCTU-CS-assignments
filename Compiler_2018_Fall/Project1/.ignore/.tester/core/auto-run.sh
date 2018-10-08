#!/bin/sh -ev

: ==============================================
:   Downloading from Github...
: ==============================================

git clone --depth=1 https://lctseng@github.com/lctseng/NCTU-Compiler-Hw1-Tester
rm -rf ./NCTU-Compiler-Hw1-Tester/.git
rm -f ./NCTU-Compiler-Hw1-Tester/.gitignore

: ==============================================
:   Download finished!
:   You can change into the 'NCTU-Compiler-Hw1-Tester' directory.
:   To run test, just copy your xxx.l source file into 
:   'NCTU-Compiler-Hw1-Tester' directory, then run './test xxx.l'
: ==============================================


int mySeed;
int INITIALIZE = 755;

void srand(int seed);
int rand();

int main() {
    int i, grade;
    i = 1;

    srand(INITIALIZE);  // print "student (number)    (grade)    pass/fail"

    // print "student (number)    (grade)    pass/fail"
    while (i <= 15) {
        print "student ";
        print i;
        print "\t";
        grade = rand() % 71 + 30;
        if (grade <= 0) {
            grade = grade + 100;
        }
        print grade;
        print "\t";

        if (grade >= 60) {
            print "pass\n";
        }

        else {
            print "fail\n";
        }
        i = i + 1;
    }

    return 0;
}

void srand(int seed) { mySeed = seed; }

int rand() {
    mySeed = mySeed * 234 + 456;
    if (mySeed >= 0) {
        return mySeed;
    }

    else {
        return -mySeed;
    }
    return 0;
}
/*
Input:
No input

Output:
student 1       91      pass
student 2       44      fail
student 3       66      pass
student 4       81      pass
student 5       100     pass
student 6       72      pass
student 7       35      fail
student 8       70      pass
student 9       33      fail
student 10      67      pass
*/

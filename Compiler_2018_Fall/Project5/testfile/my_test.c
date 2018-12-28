int foo();
void bar() { print "in bar\n"; }
int main() {
    foo();
    print "\nhello world!!!!!!!!\n";
    bar();
    print "\nEnd of world\n";
    return 0;
}
int foo() {
    print "in foo\n";
    print 123;
    print "\n";
    print 55;
    return 87;
}

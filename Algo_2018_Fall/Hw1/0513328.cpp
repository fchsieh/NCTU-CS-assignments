#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

#define INF x

int tableau_count = 0;

void read_data()
{
    fstream fin;
    fin.open("input.txt", ios::in);
    fin >> tableau_count;
}

void build_tableau() {}

void extract_min() {}

void insert() {}

int main()
{
    read_data();

    while (tableau_count--)
    {
        cout << "Hello!\n";
    }
}
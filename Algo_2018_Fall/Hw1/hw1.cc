#include <limits.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

void ReadFile();
void OutputResult();
void ExtractMin();

int table_count = 0;

int main() {
    ReadFile();
    return 0;
}

void ReadFile() {
    fstream file("input.txt", ios::in);
    string content;
    string insert;
    int used_method;  // 1 for insert, 2 for extract-min

    // Total iter count
    file >> table_count;

    for (int i = 0; i < table_count; i++) {
        file >> used_method;
        // Eat useless newlines
        file >> ws >> ws;
        // Record insert numbers
        if (used_method == 1) {
            getline(file, insert);
            // TODO: convert insert string to numbers array
        }
        while (getline(file, content)) {
            if (content.length() == 0) break;
            // TODO: save as vector array
        }
    }
}

void OutputResult() {}

void ExtractMin() {}

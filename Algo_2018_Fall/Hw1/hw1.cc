#include <limits.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

//#define DEBUG_BUILDTABLE
#define FINISH_JOB

void ReadFile();
void OutputResult();
int ExtractMin(vector<vector<int>>&);
void Insert(vector<vector<int>>&, const int);
void ReplaceX(string&, const string&, const string&);

int main() {
    ReadFile();
    return 0;
}

void ReadFile() {
    int table_count = 0;
    fstream file("input.txt", ios::in);
    string row_content;
    string insert;
    int used_method;  // 1 for insert, 2 for extract-min
    int minimum = 0;  // If use extract-min method, output minimum

    // Total iter count
    file >> table_count;

#ifdef DEBUG_BUILDTABLE
    cout << "Table count: " << table_count << "\n";
#endif

    for (int i = 0; i < table_count; i++) {
        vector<vector<int>> YoungsTable;
        int row_count = 0;
        int col_count = 0;

#ifdef DEBUG_BUILDTABLE
        cout << "Iter: " << i + 1 << "\n";
#endif
        // ========== If method = 1, Build Insert array =========
        vector<int> insert_arr;
        file >> used_method;
        // Eat useless newlines
        file >> ws >> ws;
        // Record insert numbers
        if (used_method == 1) {
            getline(file, insert);
            stringstream stream(insert);
            int n;
            while (stream >> n) {
                insert_arr.push_back(n);
            }
        }
        // =======================================================
#ifdef DEBUG_BUILDTABLE
        cout << "Use method: " << used_method << "\n";
        cout << "Insert arr: ";
        for (int i = 0; i < insert_arr.size(); i++)
            cout << insert_arr[i] << " ";
        cout << "\n";
#endif

        while (getline(file, row_content)) {
            row_count++;
            // ============== Convert x to INT_MAX ===========
            stringstream ss;
            ss.clear();
            ss << INT_MAX;
            string STR_INTMAX = ss.str();
            ReplaceX(row_content, "x", STR_INTMAX);
            // ===============================================
            if (row_content.length() == 0) break;
            vector<int> v;
            stringstream stream(row_content);
            int n;
            while (stream >> n) {
                v.push_back(n);
            }
            YoungsTable.push_back(v);
            col_count = v.size();
        }
        row_count -= 1;

#ifdef DEBUG_BUILDTABLE
        for (int i = 0; i < row_count; i++) {
            for (int j = 0; j < col_count; j++) {
                cout << YoungsTable[i][j] << " ";
            }
            cout << "\n";
        }
        cout << "\n";
#endif

        if (used_method == 1) {
            // Insert mode
            for (int i = 0; i < insert_arr.size(); i++)
                Insert(YoungsTable, insert_arr[i]);
        } else if (used_method == 2) {
            // Extract mode
            minimum = ExtractMin(YoungsTable);
        }
// OutputResult();
#ifdef FINISH_JOB
        if (used_method == 1) {
            cout << "Insert ";
            for (int i = 0; i < insert_arr.size(); i++)
                cout << insert_arr[i] << " ";
            cout << "\n";
        } else if (used_method == 2) {
            cout << "Extract-min " << minimum << "\n";
        }
        for (int i = 0; i < row_count; i++) {
            for (int j = 0; j < col_count; j++) {
                if (YoungsTable[i][j] == INT_MAX)
                    cout << "x";
                else
                    cout << YoungsTable[i][j];
                cout << " ";
            }
            cout << "\n";
        }
        cout << "\n";
#endif
    }
}

void OutputResult() {}

int ExtractMin(vector<vector<int>>& young) {
    int minimum = young[0][0];
    int row = 0;
    int col = 0;

    while (row < young.size() && col < young[0].size()) {
        int cur = young[row][col];
        int original_row = row;
        int original_col = col;
        int right = INT_MAX;
        int down = INT_MAX;

        if (row < young.size() - 1) down = young[row + 1][col];
        if (col < young[0].size() - 1) right = young[row][col + 1];

        if (right == INT_MAX && down == INT_MAX) {
            young[row][col] = INT_MAX;
            break;
        } else if (down <= right)
            row++;
        else
            col++;

        swap(young[row][col], young[original_row][original_col]);
    }
    return minimum;
}

void Insert(vector<vector<int>>& young, const int insert) {
    // Compare with left/up elem, if larger than insert,
    // Exchange insert with the elem position
    int row = young.size() - 1;
    int col = young[0].size() - 1;
    young[row][col] = insert;

    while (true) {
        if (row > 0 && col > 0) {
            if (young[row - 1][col] > young[row][col - 1] &&
                young[row - 1][col] > insert) {
                swap(young[row][col], young[row - 1][col]);
                row -= 1;
            } else if (young[row][col - 1] > young[row - 1][col] &&
                       young[row][col - 1] > insert) {
                swap(young[row][col], young[row][col - 1]);
                col -= 1;
            } else if (young[row - 1][col] == young[row][col - 1] &&
                       young[row - 1][col] > insert) {
                swap(young[row][col], young[row - 1][col]);
                row -= 1;
            } else
                break;
        } else if (row == 0 && col > 0) {
            if (young[row][col - 1] > insert) {
                swap(young[row][col], young[row][col - 1]);
                col -= 1;
            } else
                break;
        } else if (col == 0 && row > 0) {
            if (young[row - 1][col] > insert) {
                swap(young[row][col], young[row - 1][col]);
                row -= 1;
            } else
                break;
        } else {
            break;
        }
    }
}

void ReplaceX(string& str, const string& replace, const string& to) {
    // Replace x in input file
    // To INT_MAX (as inf)
    if (replace.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(replace, start_pos)) != string::npos) {
        str.replace(start_pos, replace.length(), to);
        start_pos += to.length();
    }
}
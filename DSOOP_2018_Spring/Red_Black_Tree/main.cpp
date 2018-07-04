#include <iostream>
#include <time.h>
#include <string>
#include <iomanip>
#include <set>
#include <fstream>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <vector>
//////////////////////////////////////////////////////////////////////////////////////////////////////
// Please implement RBTree class in separate header and cpp file (RBTree.h and RBTree.cpp)          //
// And you should not modify any codes in main.cpp file.                                                //
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include "RBTree.h"

using namespace std;

/**
class to evaluate time cost
*/
template <typename Timer = std::chrono::milliseconds>
struct measure
{
    template <typename F, typename... Args>
    static typename Timer::rep execution(F func, Args &&... args)
    {
        auto start = std::chrono::high_resolution_clock::now();
        func(std::forward<Args>(args)...);
        auto duration = std::chrono::duration_cast<Timer>(std::chrono::high_resolution_clock::now() - start);
        return duration.count();
    }
};

/**
function to be evaluated
*/
void EvaluateFunc(RBTree rbTree, vector<int> testCases)
{
    string gender;
    int height, weight;
    for (int i = 0; i < testCases.size(); i++)
    {
        gender = rbTree[testCases.back()].getGender();
        height = rbTree[testCases.back()].getHeight();
        weight = rbTree[testCases.back()].getWeight();
        testCases.pop_back();
    }
}

int rand31()
{
    return ((rand() << 16) + rand());
}

int main()
{
    /** 
    RBTree is the class implemented by yourself, 2 member functions you need to implement as belows:
        1. insert(key, gender, height, weight) : add data into your RBTree and return bool
            Returned bool:
                The element is set to true if a new element was inserted or false if an equivalent element already existed.
        2. operator "[]"    : return node by selected key
    For each node, 3 member functions you need to implement as belows:
        3. getGender()      : return gender by node
        4. getHeight()      : return height by node
        5. getWeight()      : return weight by node
    */

    // for evaluation
    vector<int> testCases;
    // Your RBTree
    RBTree rbTree;

    // you can test your implementation by this segment of code to avoid collision happened.
    srand(time(NULL));
    int repeat = 0;
    bool ret;

    for (int i = 0; i < 5000000; i++)
    {
        // generate test cases
        int key = rand31() % 100000000 + 1;
        int gender = rand() % 2;
        int height = 150 + rand() % 30;
        int weight = 40 + rand() % 50;
        string sex = (gender == 0) ? "female" : "male";

        // insert
        ret = rbTree.insert(key, sex, height, weight);

        // collision happen
        if (ret == false)
            repeat++;
        else
            // collect evaluated test cases
            testCases.push_back(key);
    }
    // (updated) repeat will > 0 because of collisions
    cout << repeat << endl;

    // Evauluate the speed of your RBTree
    auto totalCost = measure<std::chrono::nanoseconds>::execution(EvaluateFunc, rbTree, testCases);
    cout << "Mean: " << totalCost / testCases.size() << " ns" << endl;

    return 0;
}

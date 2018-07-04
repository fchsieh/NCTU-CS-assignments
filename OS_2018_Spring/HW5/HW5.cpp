#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <list>
#include <cstring>
#include <sys/time.h>
using namespace std;

typedef struct CNODE
{
    string key;
    CNODE(string k) : key(k){}
} CacheNode;

void sim_FIFO(string FILE, int frame)
{
    long long hit_count = 0;
    long long miss_count = 0;
    double fault_ratio = 0;

    ifstream input(FILE);

    // Store pages in order, for recording oldest page
    string check[frame];
    int oldest = 0; // oldest: [0, frame)
    int i = 0;  // For inserting free space
    // Use map to implement finding page with hash function 
    unordered_map<string, int> umap;

    string page;
    while(input >> page)
    {
        if(umap.find(page) == umap.end())
        {
            miss_count++;
            // Cache is not full, insert page directly
            if(umap.size() < frame)
            {
                // Just insert a random number
                umap[page] = 1;
                check[i++] = page;
            }
            // Cache is full, need replacement
            else
            {
                umap.erase(umap.find(check[oldest]));
                umap[page] = 1;
                check[oldest] = page;
                oldest = (oldest + 1) % frame;
            }
        }
        else
        {
            hit_count++;
        }
    }
    
    input.close();
    umap.clear();

    fault_ratio = miss_count /(double)(hit_count + miss_count);
    cout << left << setw(8) << frame << left << setw(15) << miss_count << left << setw(15) << hit_count << fixed << setprecision(9) << fault_ratio << "\n";
} 


void sim_LRU(string FILE, int frame)
{
    /*
        In fact you can find this code problem at leetcode: 146. LRU Cache
    */
    long long hit_count = 0;
    long long miss_count = 0;
    double fault_ratio = 0;

    ifstream input(FILE);

    list<CacheNode> cacheList;
    unordered_map<string, list<CacheNode>::iterator> cacheMap;

    string page;
    while(input >> page)
    {
        // Not found
        if(cacheMap.find(page) == cacheMap.end())
        {
            miss_count++;
            if(cacheList.size() == frame)
            {
                cacheMap.erase(cacheList.back().key);
                cacheList.pop_back();
            }
            cacheList.push_front(CacheNode(page));
            cacheMap[page] = cacheList.begin();
        }
        // Found
        else
        {
            hit_count++;
            auto it = cacheMap[page];
            cacheList.splice(cacheList.begin(), cacheList, it);
            cacheMap[page] = cacheList.begin();
        }
    }

    fault_ratio = miss_count /(double)(hit_count + miss_count);
    cout << left << setw(8) << frame << left << setw(15) << miss_count << left << setw(15) << hit_count << fixed << setprecision(9) << fault_ratio << "\n";
}


int main()
{
    string FILENAME;
    cout << "Input file name: ";
    cin >> FILENAME;

    struct timeval start, end;

    int frame;

    // Start!
    gettimeofday(&start, 0);
    // Do FIFO simulate
    cout << "FIFO---\n";
    cout << left << setw(8) << "size" << left << setw(15) << "miss" << left << setw(15) << "hit" << left << setw(20) << "page fault ratio" << "\n";
    for(frame = 128; frame <= 1024; frame *= 2)
    {
        sim_FIFO(FILENAME, frame);
    }

    // Do LRU simulate
    cout << "\nLRU---\n";
    cout << left << setw(8) << "size" << left << setw(15) << "miss" << left << setw(15) << "hit" << left << setw(20) << "page fault ratio" << "\n";
    for(frame = 128; frame <= 1024; frame *= 2)
    {
        sim_LRU(FILENAME, frame);
    }
    gettimeofday(&end, 0);

    int sec = end.tv_sec - start.tv_sec;
    int usec = end.tv_usec - start.tv_usec;

    cout << "\nTotal elapsed: " << sec + (usec/1000000.0) << " sec" <<  "\n\n";

    return 0;
}
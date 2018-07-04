#include <iostream>
#include <cstdio>
#include <climits>
#include <cmath>
#include <iomanip>

using namespace std;

#define KB 1024
#define block_size 64

string filename = "";

struct cache_content
{
    bool valid;
    unsigned int tag;
    unsigned int timestamp;
};

int bit_required(int cache_size, int way)
{
    cache_size /= way;
    int offset_bit = (int)log2(block_size);
    int index_bit = (int)log2(cache_size / block_size);
    int line = cache_size >> (offset_bit);

    int tagbit = 32 - (index_bit + offset_bit);

    return line * way * (1 + tagbit + block_size * 8);
}

void sim_LRU(int cache_size, int way)
{
    cache_size /= way;
    int offset_bit = (int)log2(block_size);
    int index_bit = (int)log2(cache_size / block_size);
    int line = cache_size >> (offset_bit);

    cache_content **cache = new cache_content *[line];

    for (int i = 0; i < line; i++)
    {
        cache[i] = new cache_content[way];
        for (int j = 0; j < way; j++)
        {
            cache[i][j].valid = false;
            cache[i][j].timestamp = 0;
        }
    }

    unsigned int hit = 0;
    unsigned int miss = 0;
    double fault_ratio = 0;
    unsigned int tag, index, x;
    unsigned int timestamp = 0;

    FILE *fp = fopen(filename.c_str(), "r"); // read file
    while (fscanf(fp, "%x", &x) != EOF)
    {
        timestamp++;
        index = (x >> offset_bit) & (line - 1);
        tag = x >> (index_bit + offset_bit);
        bool is_hit = false;
        unsigned int min_timestamp = UINT_MAX;
        int min_timestamp_at = -1;
        for (int i = 0; i < way; i++)
        {
            if (cache[index][i].valid && cache[index][i].tag == tag)
            {
                hit++;
                cache[index][i].timestamp = timestamp;
                is_hit = true;
                break;
            }
        }
        if (!is_hit)
        {
            for (int i = 0; i < way; i++)
            {
                if (cache[index][i].timestamp < min_timestamp)
                {
                    min_timestamp = cache[index][i].timestamp;
                    min_timestamp_at = i;
                }
            }
            miss++;
            cache[index][min_timestamp_at].timestamp = timestamp;
            cache[index][min_timestamp_at].valid = true;
            cache[index][min_timestamp_at].tag = tag;
        }
    }

    fclose(fp);

    for (int i = 0; i < line; i++)
        delete[] cache[i];

    delete[] cache;
    fault_ratio = (double)miss / (hit + miss) * 100;
    printf("Miss rate: %.2f%%\n", fault_ratio);
}

int main()
{
    // start LU trace
    cout << "Traceing: LU.txt---\n";
    filename = "LU.txt";
    for (int i = 1; i <= 32; i *= 2) // cache size
    {
        cout << "Cache size: " << i << "\n";
        for (int j = 1; j <= 8; j *= 2) // j-way
        {
            cout << "Associativity: " << j << " ways\t";
            sim_LRU(i * KB, j);
        }
        cout << "\n";
    }

    // start RADIX trace
    cout << "Tracing: RADIX.txt---\n";
    filename = "RADIX.txt";
    for (int i = 1; i <= 32; i *= 2) // cache size
    {
        cout << "Cache size: " << i << "\n";
        for (int j = 1; j <= 8; j *= 2) // j-way
        {
            cout << "Associativity: " << j << " ways\t";
            sim_LRU(i * KB, j);
        }
        cout << "\n";
    }

    // get required bitcount
    unsigned int cache_size = 1 * KB;

    cout << "Bits required:\n";
    cout << "   ";
    for (int i = 1; i <= 8; i *= 2)
        cout << setw(7) << i;
    cout << "\n";

    for (int i = 1; i <= 32; i *= 2)
    {
        cout << setw(2) << i << "K";
        for (int j = 1; j <= 8; j *= 2)
        {
            cout << setw(7) << bit_required(i * KB, j);
        }
        cout << "\n";
    }

    return 0;
}
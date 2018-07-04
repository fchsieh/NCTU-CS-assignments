#include <iostream>
#include <fstream>
#include <cmath>
#include <climits>
#include <cstring>
#include <vector>
using namespace std;

//#define DEBUG_PRINT_MAT

string in_file = "";
string out_file = "";
unsigned int addr0 = 0;
unsigned int addr1 = 0;
unsigned int addr2 = 0;
int size_m = 0;
int size_n = 0;
int size_p = 0;
int **matrixA;
int **matrixB;
int **matrixC;
int exec_cycle = 0;
int stall_one = 0;
int stall_wider = 0;
int stall_two = 0;

// Functions
void allocate_memory();
void output_result();
void release_memory();
void simulate();
void stall_simulate_one();
void stall_simulate_wider();
void stall_simulate_two();
void matrix_calculate();

// Cache
struct cache_line
{
    bool valid;
    int tag;
    unsigned int timestamp;
};

struct cache_content
{
    cache_line line[8];
};

class cache
{
  public:
    cache(int cache_size, int block_size)
    {
        way = 8;
        cache_size /= way;
        timestamp = 0;
        offset_bit = (int)log2(block_size);
        index_bit = (int)log2(cache_size / block_size);
        line = cache_size >> (offset_bit);

        mem_cache = new cache_content[line];

        for (int i = 0; i < line; i++)
            for (int j = 0; j < way; j++)
            {
                mem_cache[i].line[j].valid = false;
                mem_cache[i].line[j].tag = 0;
                mem_cache[i].line[j].timestamp = 0;
            }
    }
    ~cache()
    {
        delete[] mem_cache;
    }
    bool access(unsigned int address)
    {
        int index = (address >> offset_bit) & (line - 1);
        int tag = address >> (index_bit + offset_bit);
        bool is_hit = false;
        timestamp++;

        for (int i = 0; i < way; i++)
            if (mem_cache[index].line[i].valid && mem_cache[index].line[i].tag == tag)
            {
                is_hit = true;
                mem_cache[index].line[i].timestamp = timestamp;
                break;
            }

        if (!is_hit)
        {
            double min_timestamp = timestamp;
            int min_timestamp_at = 0;

            for (int i = 0; i < way; i++)
                if (mem_cache[index].line[i].timestamp < min_timestamp)
                {
                    min_timestamp = mem_cache[index].line[i].timestamp;
                    min_timestamp_at = i;
                }

            mem_cache[index].line[min_timestamp_at].timestamp = timestamp;
            mem_cache[index].line[min_timestamp_at].valid = true;
            mem_cache[index].line[min_timestamp_at].tag = tag;
            return false;
        }
        else
            return true;
    }

  private:
    int way;
    // For LRU cache
    unsigned int timestamp;
    int offset_bit;
    int index_bit;
    int line;
    cache_content *mem_cache;
};

void allocate_memory()
{
    fstream file;
    file.open(in_file, ios::in);

    file >> hex >> addr0 >> addr1 >> addr2;
    file >> dec >> size_m >> size_n >> size_p;

    matrixA = new int *[size_m]; // A m*n
    for (int i = 0; i < size_m; i++)
    {
        matrixA[i] = new int[size_n];
        for (int j = 0; j < size_n; j++)
            file >> matrixA[i][j];
    }
    matrixB = new int *[size_n]; // B n*p
    for (int i = 0; i < size_n; i++)
    {
        matrixB[i] = new int[size_p];
        for (int j = 0; j < size_p; j++)
            file >> matrixB[i][j];
    }
    file.close();

    matrixC = new int *[size_m]; // C m*p
    for (int i = 0; i < size_m; i++)
        matrixC[i] = new int[size_p];

#ifdef DEBUG_PRINT_MAT
    cout << addr0 << " " << addr1 << " " << addr2 << "\n";
    cout << size_m << " " << size_n << " " << size_p << "\n";
    for (int i = 0; i < size_m; i++)
    {
        for (int j = 0; j < size_p; j++)
            cout << matrixA[i][j] << " ";
        cout << "\n";
    }
#endif
}

void output_result()
{
    // Print result to output file
    fstream file;
    file.open(out_file, ios::out);

    for (int i = 0; i < size_m; i++)
    {
        for (int j = 0; j < size_p; j++)
        {
            file << matrixC[i][j] << " ";
            if (j == size_p - 1)
                file << "\n";
        }
    }
    file << exec_cycle << " " << stall_one << " " << stall_wider << " " << stall_two << "\n";
    file.close();
}

void release_memory()
{
    for (int i = 0; i < size_m; i++)
    {
        delete[] matrixA[i];
        delete[] matrixC[i];
    }
    delete[] matrixA;
    delete[] matrixC;
    for (int i = 0; i < size_n; i++)
        delete[] matrixB[i];
    delete[] matrixB;
}

void stall_simulate_one()
{
    // Simulate One-word-wide memory 
    cache one_word_wide(512, 8 * 4);
    stall_one = 0;
    for (int i = 0; i < size_m; i++)
    {
        for (int j = 0; j < size_p; j++)
        {
            for (int k = 0; k < size_n; k++)
            {
                vector<unsigned int> addr_arr;
                addr_arr.clear();
                unsigned int addr_tmp_1 = 4 * (i * size_p + j) + addr2; //	temp1 = 4(i*p+j) + C[]base
                unsigned int addr_tmp_2 = 4 * (i * size_n + k) + addr0; //	temp2 = 4(i*n+k) + A[]base
                unsigned int addr_tmp_3 = 4 * (k * size_p + j) + addr1; //	temp3 = 4(k*p+j) + B[]base
                unsigned int addr_tmp_4 = 4 * (i * size_p + j) + addr2; //	sw
                addr_arr.push_back(addr_tmp_1);
                addr_arr.push_back(addr_tmp_2);
                addr_arr.push_back(addr_tmp_3);
                addr_arr.push_back(addr_tmp_4);
                for (int i = 0; i < addr_arr.size(); i++)
                {
                    if (one_word_wide.access(addr_arr[i]))
                        stall_one += (1 + 2 + 1);
                    else
                        stall_one += (1 + 2 + 8 * (1 + 100 + 1 + 2) + 1);
                }
            }
        }
    }
}

void stall_simulate_wider()
{
    // Simulate Wider-memory 
    cache wider(512, 8 * 4);
    stall_wider = 0;
    for (int i = 0; i < size_m; i++)
    {
        for (int j = 0; j < size_p; j++)
        {
            for (int k = 0; k < size_n; k++)
            {
                vector<unsigned int> addr_arr;
                addr_arr.clear();
                unsigned int addr_tmp_1 = 4 * (i * size_p + j) + addr2; //	temp1 = 4(i*p+j) + C[]base
                unsigned int addr_tmp_2 = 4 * (i * size_n + k) + addr0; //	temp2 = 4(i*n+k) + A[]base
                unsigned int addr_tmp_3 = 4 * (k * size_p + j) + addr1; //	temp3 = 4(k*p+j) + B[]base
                unsigned int addr_tmp_4 = 4 * (i * size_p + j) + addr2; //	sw
                addr_arr.push_back(addr_tmp_1);
                addr_arr.push_back(addr_tmp_2);
                addr_arr.push_back(addr_tmp_3);
                addr_arr.push_back(addr_tmp_4);
                for (int i = 0; i < addr_arr.size(); i++)
                {
                    if (wider.access(addr_arr[i]))
                        stall_wider += (1 + 2 + 1);
                    else
                        stall_wider += (1 + 2 + 1 + 100 + 1 + 2 + 1);
                }
            }
        }
    }
}

void stall_simulate_two()
{
    // Simulate Two-level-memory
    cache L1(128, 4 * 4);
    cache L2(4096, 32 * 4);
    stall_two = 0;
    for (int i = 0; i < size_m; i++)
    {
        for (int j = 0; j < size_p; j++)
        {
            for (int k = 0; k < size_n; k++)
            {
                vector<unsigned int> addr_arr;
                addr_arr.clear();
                // do matrix calculate
                //matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
                unsigned int addr_tmp_1 = 4 * (i * size_p + j) + addr2; //	temp1 = 4(i*p+j) + C[]base
                unsigned int addr_tmp_2 = 4 * (i * size_n + k) + addr0; //	temp2 = 4(i*n+k) + A[]base
                unsigned int addr_tmp_3 = 4 * (k * size_p + j) + addr1; //	temp3 = 4(k*p+j) + B[]base
                unsigned int addr_tmp_4 = 4 * (i * size_p + j) + addr2; //	sw
                addr_arr.push_back(addr_tmp_1);
                addr_arr.push_back(addr_tmp_2);
                addr_arr.push_back(addr_tmp_3);
                addr_arr.push_back(addr_tmp_4);
                // Not sure if the calculation is correct
                for (int arr = 0; arr < addr_arr.size(); arr++)
                {
                    if (L1.access(addr_arr[arr]))
                        stall_two += (1 + 1 + 1);
                    else
                    {
                        if (L2.access(addr_arr[arr]))
                            stall_two += (1 + 1 + 4 * (1 + 10 + 1 + 1) + 1);
                        else
                            stall_two += (1 + 32 * (1 + 100 + 1 + 10) + 4 * (1 + 10 + 1 + 1) + 1 + 1);
                    }
                }
            }
        }
    }
}

void matrix_calculate()
{
    // Calculate matrix C and calculate the program execution cycle at the same time
    exec_cycle = 0;
    exec_cycle += 3;
    for (int i = 0; i < size_m; i++)
    {
        exec_cycle += 3;
        for (int j = 0; j < size_p; j++)
        {
            matrixC[i][j] = 0;
            exec_cycle += 3;
            for (int k = 0; k < size_n; k++)
            {
                exec_cycle += 22;
                matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
            }
            exec_cycle += 2;
            exec_cycle += 2;
        }
        exec_cycle += 2;
        exec_cycle += 2;
    }
    exec_cycle += 2;
}

void simulate()
{
    // Do simulation
    matrix_calculate();
    stall_simulate_one();
    stall_simulate_wider();
    stall_simulate_two();
}

int main(int argc, char *argv[])
{
    // Usage: ./simulate_caches INFILE OUTFILE
    in_file = argv[1];
    out_file = argv[2];

    allocate_memory();
    simulate();
    output_result();
    release_memory();

    return 0;
}
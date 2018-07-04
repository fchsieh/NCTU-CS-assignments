#include <iostream>
#include <cstdio>
#include <math.h>
#include <cstring>

using namespace std;

struct cache_content
{
	bool v;
	unsigned int tag;
	// unsigned int	data[16];
};

const int K = 1024;
string filename = "";

double log2(double n)
{
	// log(n) / log(2) is log2.
	return log(n) / log(double(2));
}

void simulate(int cache_size, int block_size)
{
	unsigned int tag, index, x;

	int offset_bit = (int)log2(block_size);
	int index_bit = (int)log2(cache_size / block_size);
	int line = cache_size >> (offset_bit);

	cache_content *cache = new cache_content[line];

	//cout << "cache line: " << line << endl;

	for (int j = 0; j < line; j++)
		cache[j].v = false;

	FILE *fp = fopen(filename.c_str(), "r"); // read file

	unsigned int miss = 0, hit = 0;
	while (fscanf(fp, "%x", &x) != EOF)
	{
		//cout << hex << x << " ";
		index = (x >> offset_bit) & (line - 1);
		tag = x >> (index_bit + offset_bit);
		if (cache[index].v && cache[index].tag == tag)
		{
			// hit
			hit++;
			cache[index].v = true;
		}
		else
		{
			// miss
			miss++;
			cache[index].v = true;
			cache[index].tag = tag;
		}
	}
	fclose(fp);

	delete[] cache;
	double miss_rate = (double)miss / (hit + miss);
	printf("Miss rate = %.2f%%\n", miss_rate * 100);
}

int main()
{
	// start ICACHE simulate
	filename = "ICACHE.txt";
	printf("Tracing: ICACHE.txt---\n");
	for (int i = 4; i <= 256; i *= 4) // cache size
	{
		printf("Cache size: %d KB\n", i);
		for (int j = 16; j <= 256; j *= 2) // block size
		{
			printf("Block size: %d B,\t", j);
			simulate(i * K, j);
		}
		cout << "\n";
	}

	// start DCACHE simulate
	filename = "DCACHE.txt";
	printf("\n\nTracing: DCACHE.txt---\n");
	for (int i = 4; i <= 256; i *= 4) // cache size
	{
		printf("Cache size: %d KB\n", i);
		for (int j = 16; j <= 256; j *= 2) // block size
		{
			printf("Block size: %d B,\t", j);
			simulate(i * K, j);
		}
		cout << "\n";
	}
	return 0;
}

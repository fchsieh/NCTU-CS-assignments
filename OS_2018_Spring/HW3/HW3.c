#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <sys/time.h>

//#define DEBUG

int num_count;
int *mt_arr;
int *st_arr;
sem_t T[16], done[16], thread_finished;
sem_t single, single_done;

typedef struct
{
    int start;
    int end;
} slice;
// part[15] for partitions in mt_arr
// part2[15] for partitions in st_arr
slice part[15], part2[15];

void *task(void *in);
void *single_sort(void *in);
void bubble_sort(int arr[], int start, int end);
int partition(int arr[], int start, int end);
void swap(int *a, int *b);

int main()
{
    // Input file name
    char filename[50];
    printf("Input file name: ");
    scanf("%s", filename);
    filename[strlen(filename)] = '\0';

    // Read file and save numbers into array
    if (freopen(filename, "r", stdin) == NULL)
    {
        perror("freopen error");
        exit(1);
    }
    scanf("%d", &num_count);
    mt_arr = (int *)malloc(sizeof(int) * num_count); // for multithread
    st_arr = (int *)malloc(sizeof(int) * num_count); // for singlethread
    for (int i = 0; i < num_count; i++)
    {
        scanf("%d", &mt_arr[i]);
        st_arr[i] = mt_arr[i];
    }
    fclose(stdin);

    // Create semaphores
    // semaphore of T1 = T[0]...
    for (int i = 0; i < 16; i++)
    {
        sem_init(&T[i], 0, 0);
        sem_init(&done[i], 0, 0);
    }
    // after finish all threads, call thread_finished
    sem_init(&thread_finished, 0, 0);
    // semaphore for singlethread
    sem_init(&single, 0, 0);
    sem_init(&single_done, 0, 0);

    // Create threads
    // T1 = tid[0], T2 = tid[1]...
    pthread_t tid[15];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int thread_num[15];
    for (int i = 0; i < 15; i++)
    {
        thread_num[i] = i + 1;
        if (pthread_create(&tid[i], &attr, task, &thread_num[i]) != 0)
        {
            perror("pthread create");
            exit(1);
        }
    }
    // for singlethread
    pthread_t spid;
    if (pthread_create(&spid, NULL, single_sort, NULL) != 0)
    {
        perror("pthread create");
        exit(1);
    }

    // Start Multithread sorting
    struct timeval start_mt, end_mt;
    gettimeofday(&start_mt, 0);

#ifdef DEBUG
    printf("Start MT\n");
#endif

    // Call T1
    printf("\nStart Multi-thread\n");
    part[0].start = 0;
    part[0].end = num_count - 1;
    sem_post(T + 0);
    // wait until all threads are done
    sem_wait(&thread_finished);

#ifdef DEBUG
    printf("MT done\n");
#endif

    gettimeofday(&end_mt, 0);
    int sec_mt = end_mt.tv_sec - start_mt.tv_sec;
    int usec_mt = end_mt.tv_usec - start_mt.tv_usec;

    // Output result to output1.txt and print elapsed time
    printf("Multi-thread elapsed: %f sec\n", sec_mt + (usec_mt / 1000000.0));
    FILE *out1;
    if ((out1 = fopen("output1.txt", "w")) == NULL)
    {
        perror("fopen error");
        exit(1);
    }
    for (int i = 0; i < num_count; i++)
        fprintf(out1, "%d ", mt_arr[i]);
    fclose(out1);
    free(mt_arr); // release memory

    // Start singlethread sorting
    struct timeval start_st, end_st;
    gettimeofday(&start_st, 0);
#ifdef DEBUG
    printf("Start ST\n");
#endif
    printf("Start Single-thread\n");
    part2[0].start = 0;
    part2[0].end = num_count - 1;
    sem_post(&single);
    sem_wait(&single_done);
#ifdef DEBUG
    printf("ST done\n");
#endif
    gettimeofday(&end_st, 0);
    int sec_st = end_st.tv_sec - start_st.tv_sec;
    int usec_st = end_st.tv_usec - start_st.tv_usec;

    // Output result to output2.txt and print elapsed time
    printf("Single-thread elapsed: %f sec\n", sec_st + (usec_st / 1000000.0));
    FILE *out2;
    if ((out2 = fopen("output2.txt", "w")) == NULL)
    {
        perror("fopen error");
        exit(1);
    }
    for (int i = 0; i < num_count; i++)
        fprintf(out2, "%d ", st_arr[i]);
    fclose(out2);
    free(st_arr); // release memory

    // delete semaphores
    for (int i = 0; i < 16; i++)
    {
        sem_destroy(&T[i]);
        sem_destroy(&done[i]);
    }
    sem_destroy(&thread_finished);
    sem_destroy(&single);
    sem_destroy(&single_done);

    return 0;
}

int partition(int arr[], int start, int end)
{
    int original_start = start;
    if (start >= end)
        return start;
    int pivot = arr[start];
    while (start < end)
    {
        while (arr[start] <= pivot)
            start++;
        while (arr[end] > pivot)
            end--;
        swap(&arr[start], &arr[end]);
    }
    if (start > original_start && arr[start] > pivot)
        return start - 1;
    else
        return start;
}

void *task(void *in)
{
    int thread_id = *((int *)in);
    sem_wait(T + thread_id - 1);

#ifdef DEBUG
    printf("thread start: %d\n", thread_id);
#endif

    if (thread_id >= 1 && thread_id <= 7)
    {
        int pivot = partition(mt_arr, part[thread_id - 1].start, part[thread_id - 1].end);
        part[thread_id * 2 - 1].start = part[thread_id - 1].start;
        part[thread_id * 2 - 1].end = pivot;
        sem_post(T + thread_id * 2 - 1);
        part[thread_id * 2].start = pivot + 1;
        part[thread_id * 2].end = part[thread_id - 1].end;
        sem_post(T + thread_id * 2);
    }
    else if (thread_id >= 8 && thread_id <= 15)
    {
        bubble_sort(mt_arr, part[thread_id - 1].start, part[thread_id - 1].end);
        sem_post(&done[thread_id]);
    }
    if (thread_id == 1)
    {
        for (int i = 8; i <= 15; i++)
            sem_wait(&done[i]);
        sem_post(&thread_finished);
    }

#ifdef DEBUG
    printf("thread %d done\n", thread_id);
#endif

    pthread_exit((void *)0);
}

void bubble_sort(int arr[], int start, int end)
{
    for (int i = start; i <= end; i++)
    {
        for (int j = end; j > i; j--)
        {
            if (arr[i] > arr[j])
                swap(&arr[j], &arr[i]);
        }
    }
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void *single_sort(void *in)
{
    sem_wait(&single);
    for (int i = 1; i < 8; i++)
    {
        int pivot = partition(st_arr, part2[i - 1].start, part2[i - 1].end);
        part2[i * 2].start = pivot + 1;
        part2[i * 2].end = part2[i - 1].end;
        part2[i * 2 - 1].start = part2[i - 1].start;
        part2[i * 2 - 1].end = pivot;
    }
    for (int i = 8; i <= 15; i++)
    {
        bubble_sort(st_arr, part2[i - 1].start, part2[i - 1].end);
    }

    sem_post(&single_done);
    pthread_exit((void *)0);
}
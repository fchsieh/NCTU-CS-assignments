#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <stdlib.h>

typedef struct thread_data
{
    int tid;
    sem_t start;
    pthread_t pid;
} thread_data;

thread_data *thread_pool[8];
thread_data thread[8];

typedef struct slice
{
    int start;
    int end;
} slice;

slice part[15];

// semaphores
sem_t T[15], sort_done, job_list_mutex, job_list_flag;

// array to sort
int *src;
int *sort_arr;
int num_count;

// job list
int job_rear = 0;
int job_front = 0;

void *task(void *in);
void push(thread_data *th);
thread_data *pop();
void swap(int *a, int *b);
void bubble_sort(int start, int end);
int partition(int start, int end);
void read_file();
void output_file(int thread_count);

int main()
{
    // read input and save numbers to src array
    read_file();

    // init semaphores
    for (int i = 0; i < 15; i++)
        sem_init(&(T[i]), 0, 0);
    sem_init(&sort_done, 0, 0);
    sem_init(&job_list_mutex, 0, 1);
    sem_init(&job_list_flag, 0, 0);

    // start sorting
    printf("Start sorting...\n");
    for (int thread_count = 0; thread_count < 8; thread_count++)
    {
        // init numbers array
        for (int i = 0; i < num_count; i++)
            sort_arr[i] = src[i];

        // create thread
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        if ((pthread_create(&thread[thread_count].pid, &attr, task, (void *)&thread[thread_count]) != 0))
        {
            perror("pthread create");
            exit(1);
        }
        push(&thread[thread_count]);
        part[0].start = 0;
        part[0].end = num_count - 1;

        // start sorting
        struct timeval start, end;
        gettimeofday(&start, 0);
        sem_post(&T[0]);
        for (int i = 0; i < 15; i++)
        {
            thread_data *new_thread = pop();
            new_thread->tid = i;
            sem_post(&new_thread->start);
        }

        // wait for all threads are done
        for (int i = 0; i < 8; i++)
            sem_wait(&sort_done);

        gettimeofday(&end, 0);
        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;

        printf("Thread count: %d, elapsed time: %f sec\n", thread_count + 1, sec + (usec / 1000000.0));
        output_file(thread_count + 1);
    }
    // Release memory and delete sems
    free(src);
    free(sort_arr);
    sem_destroy(&sort_done);
    sem_destroy(&job_list_mutex);
    sem_destroy(&job_list_flag);
    for (int i = 0; i < 15; i++)
        sem_destroy(&T[i]);
    return 0;
}

void *task(void *in)
{
    thread_data *tmp = (thread_data *)in;
    while (1)
    {
        sem_wait(&tmp->start);

        int id = tmp->tid;
        int start = part[id].start;
        int end = part[id].end;
        sem_wait(&T[id]);

        if (id >= 0 && id < 7)
        {
            int pivot = partition(start, end);
            part[id * 2 + 1].start = start;
            part[id * 2 + 1].end = pivot - 1;
            sem_post(&T[id * 2 + 1]);
            part[id * 2 + 2].start = pivot + 1;
            part[id * 2 + 2].end = end;
            sem_post(&T[id * 2 + 2]);
            push(tmp);
        }
        else if (id >= 7 && id < 15)
        {
            bubble_sort(start, end);
            sem_post(&sort_done);
            push(tmp);
        }
    }
}

void push(thread_data *th)
{
    sem_wait(&job_list_mutex);
    thread_pool[job_rear] = th;
    job_rear++;
    if (job_rear == 8)
        job_rear = 0;
    sem_post(&job_list_mutex);
    sem_post(&job_list_flag);
}

thread_data *pop()
{
    sem_wait(&job_list_flag);
    sem_wait(&job_list_mutex);
    int tmp = job_front;
    job_front++;
    if (job_front == 8)
        job_front = 0;
    sem_post(&job_list_mutex);
    return thread_pool[tmp];
}

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void bubble_sort(int start, int end)
{
    for (int i = start; i < end; i++)
    {
        for (int j = i + 1; j <= end; j++)
        {
            if (sort_arr[i] > sort_arr[j])
            {
                swap(&sort_arr[i], &sort_arr[j]);
            }
        }
    }
}

int partition(int start, int end)
{
    int pivot = start;
    start++;
    while (start <= end)
    {
        if (sort_arr[start] <= sort_arr[pivot])
            start++;
        else if (sort_arr[end] > sort_arr[pivot])
            end--;
        else
            swap(&sort_arr[start], &sort_arr[end]);
    }
    if (end != pivot)
        swap(&sort_arr[pivot], &sort_arr[end]);
    return end;
}

void read_file()
{
    // Input file name
    char file_name[] = "input.txt";

    // Read file and save numbers into src arr
    if (freopen(file_name, "r", stdin) == NULL)
    {
        perror("Read input file error");
        exit(1);
    }
    scanf("%d", &num_count);
    src = (int *)malloc(num_count * sizeof(int));
    // sort_arr is for sorting
    sort_arr = (int *)malloc(num_count * sizeof(int));
    // src sort_arr should not be changed
    for (int i = 0; i < num_count; i++)
        scanf("%d", &src[i]);
    fclose(stdin);
}

void output_file(int thread_count)
{
    char out_name[15];
    snprintf(out_name, 15, "output_%d.txt", thread_count);
    FILE *out;
    if ((out = fopen(out_name, "w")) == NULL)
    {
        perror("Create output file error");
        exit(1);
    }
    for (int i = 0; i < num_count; i++)
        fprintf(out, "%d ", sort_arr[i]);
    fclose(out);
}

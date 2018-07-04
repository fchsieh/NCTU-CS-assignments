#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

int main()
{
    int dim;
    pid_t pid[16];

    printf("Input the matrix dimension: ");
    scanf("%d", &dim);
    printf("\n");

    for (int proc_count = 1; proc_count <= 16; ++proc_count)
    {
        if (proc_count == 1)
            printf("Multiplying matrices using %d process\n", proc_count);
        else
            printf("Multiplying matrices using %d processes\n", proc_count);

        /* get shared memory space and attach */
        int shmflag = (IPC_CREAT | 0666);
        int shmidCalc = shmget(IPC_PRIVATE, sizeof(unsigned int) * dim * dim, shmflag);
        int shmidRes = shmget(IPC_PRIVATE, sizeof(unsigned int) * dim * dim, shmflag);
        if (shmidCalc == -1 || shmidRes == -1)
        {
            fprintf(stderr, "shmget failed\n");
            exit(1);
        }
        unsigned int *MATRIX_CALC = (unsigned int *)shmat(shmidCalc, NULL, 0);
        unsigned int *MATRIX_RESULT = (unsigned int *)shmat(shmidRes, NULL, 0);
        for (int i = 0; i < dim * dim; ++i)
        {
            MATRIX_CALC[i] = i;
            MATRIX_RESULT[i] = 0;
        }
        /* get shared memory space and attach */

        int part = dim / proc_count;
        int nextPart = part + 1;
        int nextPartNum = dim % proc_count;
        struct timeval start, end;

        /* main calc function */
        gettimeofday(&start, 0);
        for (int i = 0; i < proc_count; ++i)
        {
            if ((pid[i] = fork()) == -1)
            {
                fprintf(stderr, "fork failed\n");
                exit(1);
            }
            else if (pid[i] == 0)
            {
                if (i < nextPartNum)
                {
                    for (int a = 0; a < dim; ++a)
                    {
                        for (int b = i * nextPart; b < nextPart * (i + 1); ++b)
                        {
                            for (int c = 0; c < dim; ++c)
                                MATRIX_RESULT[(dim * b) + a] += (MATRIX_CALC[c + (dim * b)] * MATRIX_CALC[a + (dim * c)]);
                        }
                    }
                }
                else
                {
                    for (int a = 0; a < dim; ++a)
                    {
                        for (int b = part * (i - nextPartNum) + nextPart * nextPartNum; b < part * (i + 1 - nextPartNum) + nextPart * nextPartNum; ++b)
                        {
                            for (int c = 0; c < dim; ++c)
                                MATRIX_RESULT[(dim * b) + a] += (MATRIX_CALC[c + (dim * b)] * MATRIX_CALC[a + (dim * c)]);
                        }
                    }
                }
                exit(0);
            }
            else
            {
                continue;
            }
        }
        for (int i = 0; i < proc_count; ++i)
            wait(NULL);

        unsigned int checksum = 0;
        for (int i = 0; i < dim; ++i)
            for (int j = 0; j < dim; ++j)
                checksum += MATRIX_RESULT[j + (dim * i)];

        gettimeofday(&end, 0);
        /* main calc function */

        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;
        printf("Elapsed time: %f sec, Checksum: %u\n", sec + (usec / 1000000.0), checksum);

        /* release memory */
        shmdt(MATRIX_CALC);
        shmdt(MATRIX_RESULT);
        if (shmctl(shmidCalc, IPC_RMID, 0) == -1 || shmctl(shmidRes, IPC_RMID, 0) == -1)
        {
            fprintf(stderr, "shmctl failed\n");
            exit(1);
        }
        /* release memory */
    }

    return 0;
}
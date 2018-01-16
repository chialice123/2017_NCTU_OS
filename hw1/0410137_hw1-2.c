#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#define UNT unsigned int

void singleProcess(UNT* A, UNT* B, UNT* C, int dim);
void fourProcess(UNT* A, UNT* B, UNT* C, int dim);
UNT* SHM(int dim, int* shmid);
int main()
{
    int dim;
    printf("Dimension: ");
    scanf("%d", &dim);

    int shmid_arr[3]={0,0,0};

    UNT* arrA = SHM(dim, &shmid_arr[0]);
    UNT* arrB = SHM(dim, &shmid_arr[1]);
    UNT* arrC = SHM(dim, &shmid_arr[2]);

    for(int row=0; row<dim; row++){
        for (int col=0; col<dim; col++){
            arrA[row*dim+col] = row*dim+col;
            arrB[row*dim+col] = row*dim+col;
        }
    }
    singleProcess(arrA, arrB, arrC, dim);
    fourProcess(arrA, arrB, arrC, dim);

    shmdt(arrA);
    shmdt(arrB);
    shmdt(arrC);
    for (int i=0; i<3; i++)
        shmctl(shmid_arr[i], IPC_RMID, NULL);
    //system("ipcs -m");
    //system("ps aux | grep 'Z'");

    return 0;
}

UNT* SHM(int dim, int* shmid)
{
    *shmid = shmget(0, sizeof(UNT)*dim*dim, IPC_CREAT | 0666); //rwx-rwx-rwx (me-group-other)
    if(*shmid<0){
        printf("Get SHM failed.\n");
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    UNT* shmaddr = shmat(*shmid, NULL, 0);
    if(shmaddr<0){
        printf("Attach SHM failed.\n");
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    return shmaddr;
}

void singleProcess(UNT* A, UNT* B, UNT* C, int dim)
{
    UNT checksum=0;
    struct timeval start, end;

    //initialized C with 0s
    for(int row=0; row<dim; row++){
        for (int col=0; col<dim; col++){
            C[row*dim+col] = 0;
        }
    }

    gettimeofday(&start, 0);

    //multiplication
    for (int i=0; i<dim; ++i){
        for (int j=0; j<dim; ++j){
            for (int k=0; k<dim; ++k){
                C[i*dim+j] += (A[i*dim+k] * B[k*dim+j]);
            }
            checksum += C[i*dim+j];
        }
    }

    gettimeofday(&end, 0);
    int sec = end.tv_sec - start.tv_sec;
    int usec = end.tv_usec - start.tv_usec;

    printf("1-process, checksum = %u\n", checksum);
    printf("elapsed %f ms\n", sec*1000+(usec/1000.0));
}

void fourProcess(UNT* A, UNT* B, UNT* C, int dim)
{
    pid_t pid1, pid2, pid3;
    int p1, p2, p3, p4;

    UNT checksum=0;
    struct timeval start, end;

    switch(dim%4){
        case 0:
            p1 = p2 = p3 = p4 = dim/4;
            break;
        case 1:
            p1 = dim/4 + 1;
            p2 = p3 = p4 = dim/4;
            break;
        case 2:
            p1 = p2 = dim/4 + 1;
            p3 = p4 = dim/4;
            break;
        case 3:
            p1 = p2 = p3 = dim/4 + 1;
            p4 = dim;
            break;
    }

    //initialized C with 0s
    for(int row=0; row<dim; row++){
        for (int col=0; col<dim; col++){
            C[row*dim+col] = 0;
        }
    }

    gettimeofday(&start, 0);

    pid1 = fork();
    if (pid1<0){
    	fprintf(stderr, "Fork Failed\n");
		exit(EXIT_FAILURE);
    } else if (pid1==0){ //child-1 process
        pid2 = fork();
        if(pid2<0){
            fprintf(stderr, "Fork Failed\n");
            exit(EXIT_FAILURE);
        } else if (pid2==0){ //child-2 process
            //execute multiplication
            for (int i=0; i<p1; ++i){
                for (int j=0; j<dim; ++j){
                    for (int k=0; k<dim; ++k){
                        C[i*dim+j] += (A[i*dim+k] * B[k*dim+j]);
                    }
                }
            }
            exit(EXIT_SUCCESS);
        } else{ //parent-1 process
            //execute multiplication
            for (int i=p1; i<p1+p2; ++i){
                for (int j=0; j<dim; ++j){
                    for (int k=0; k<dim; ++k){
                        C[i*dim+j] += (A[i*dim+k] * B[k*dim+j]);
                    }
                }
            }
            wait(NULL);
        }
        exit(EXIT_SUCCESS);
    } else{ //parent-main process
        pid3 = fork();
        if(pid3<0){
            fprintf(stderr, "Fork Failed\n");
            exit(EXIT_FAILURE);
        } else if (pid3==0){ //child-3 process
            //execute multiplication
            for (int i=p1+p2; i<p1+p2+p3; ++i){
                for (int j=0; j<dim; ++j){
                    for (int k=0; k<dim; ++k){
                        C[i*dim+j] += (A[i*dim+k] * B[k*dim+j]);
                    }
                }
            }
            exit(EXIT_SUCCESS);
        } else{ //parent-main process
            //execute multiplication
            for (int i=p1+p2+p3; i<dim; ++i){
                for (int j=0; j<dim; ++j){
                    for (int k=0; k<dim; ++k){
                        C[i*dim+j] += (A[i*dim+k] * B[k*dim+j]);
                    }
                }
            }
            wait(NULL);
        }
        wait(NULL);
    }

    //checksum
    for (int i=0; i<dim; ++i){
        for (int j=0; j<dim; ++j){
            checksum += C[i*dim+j];
        }
    }

    gettimeofday(&end, 0);
    int sec = end.tv_sec - start.tv_sec;
    int usec = end.tv_usec - start.tv_usec;

    printf("4-process, checksum = %u\n", checksum);
    printf("elapsed %f ms\n", sec*1000+(usec/1000.0));
}

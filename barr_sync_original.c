#include "mpi.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void matmul(int my_size){
    
    int    i, j, k;
    double **a, **b, **c;
    double *a_block, *b_block, *c_block;
    int NRA, NCA, NCB;
    
    
    NRA = my_size; NCA = my_size; NCB = my_size;
    
    a = (double **) malloc(NRA*sizeof(double *)); /* matrix a to be multiplied */
    b = (double **) malloc(NCA*sizeof(double *)); /* matrix b to be multiplied */
    c = (double **) malloc(NRA*sizeof(double *)); /* result matrix c */
    
    a_block = (double *) malloc(NRA*NCA*sizeof(double)); /* Storage for matrices */
    b_block = (double *) malloc(NCA*NCB*sizeof(double));
    c_block = (double *) malloc(NRA*NCB*sizeof(double));
    
    for (i=0; i<NRA; i++)   /* Initialize pointers to a */
        a[i] = a_block+i*NRA;
    
    for (i=0; i<NCA; i++)   /* Initialize pointers to b */
        b[i] = b_block+i*NCA;
    
    for (i=0; i<NRA; i++)   /* Initialize pointers to c */
        c[i] = c_block+i*NRA;
    
    for (i=0; i<NRA; i++) /* last matrix has been initialized */
        for (j=0; j<NCA; j++)
            a[i][j]= (double) (i+j);
    for (i=0; i<NCA; i++)
        for (j=0; j<NCB; j++)
            b[i][j]= (double) (i*j);
    for (i=0; i<NRA; i++)
        for (j=0; j<NCB; j++)
            c[i][j]= 0.0;
    
    for (i=0; i<NRA; i++) {
        for(j=0; j<NCB; j++) {
            for (k=0; k<NCA; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    
}


int main (int argc, char *argv[]) {
    
    int MyProc, tag=1, i, process_size, j;
    MPI_Status *status;
    if(argc < 5){
        printf("The four arguments are mandatory (SIZE, ITERS, IMBALANCE,IMBALANCE_BASE)");
        return -1;
    }
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &MyProc);
    MPI_Comm_size(MPI_COMM_WORLD, &process_size);
    
    int SIZE = atoi(argv[1]);
    int ITERS = atoi(argv[2]);
    int IMBALANCE =  atoi(argv[3]);
    int IMBALANCE_BASE = atoi(argv[4]);
    
    time_t local_start, local_stop;
    double global_time;
    double local_elapsed_time, reduce_time;
    double total_transcurred_time = 0;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int local_processed = 0;
    int local_i_index_count = 0;
    int name_len;
    
    
    local_elapsed_time = 0;
    for(i=0; i<ITERS; i++){
        local_start = time(NULL);
        for(j=0; j<IMBALANCE_BASE+IMBALANCE*MyProc; j++){
            matmul(SIZE);
            local_processed++;
        }
        local_stop = time(NULL);
        local_elapsed_time +=difftime(local_stop,local_start);
        MPI_Reduce(&local_elapsed_time,&global_time,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        if(MyProc == 0){
            printf("Iteration %d done!\n", i);
        }
    }
    MPI_Get_processor_name(processor_name, &name_len);
    MPI_Send(processor_name,strlen(processor_name)+1,MPI_CHAR,0,0,MPI_COMM_WORLD);
    MPI_Send(&local_elapsed_time,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
    MPI_Send(&local_processed,1,MPI_INT,0,0,MPI_COMM_WORLD);
    
    
    if(MyProc == 0){
        printf("Total process: %d\n",process_size);
        printf("Process number %d processed %d and its time was  %.02f in node %s\n", MyProc,local_processed,local_elapsed_time,processor_name);
        int q ;
        double recived_time;
        int count_by_proced_recived;
        int proces_id_recived;
        int recived_i_index_count;
        char recived_processor_name[MPI_MAX_PROCESSOR_NAME];
        for(q=1;q<process_size;q++){
            MPI_Recv(recived_processor_name,1024,MPI_CHAR,q,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&recived_time,1,MPI_DOUBLE,q,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&count_by_proced_recived,1,MPI_INT,q,0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process number %d processed %d and its time was  %.02f in node %s\n", q,count_by_proced_recived, recived_time,recived_processor_name);
        }
        printf("Total reduced  time elapsed was %.02f\n", global_time);
        
    }
    MPI_Finalize();
    if(MyProc == 0){
        printf("Done.\n");
    }
    
    
    exit(0);
}


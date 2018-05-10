#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <mpi.h>


#define SIZE 36


void merge(int* array, int start, int end)
{
    int middle = (start + end) / 2;
    int temp_index = 0;

    /* create a temporary array */
    int* temp = malloc(sizeof(int) * (end - start + 1));

    /* merge in sorted data from the 2 halves */
    int left = start;
    int right = middle + 1;

    /* while both halves have data */
    while((left <= middle) && (right <= end)) {
        /* if the left half value is less than right */
        if (array[left] < array[right]) {
            /* take from left */
            temp[temp_index] = array[left];
            temp_index++;
            left++;
        }
        else {
            /* take from right */
            temp[temp_index] = array[right];
            temp_index++;
            right++;
        }
    }

    /* add the remaining elements from the left half */
    while(left <= middle) {
        temp[temp_index] = array[left];
        temp_index++;
        left++;
    }

    /* add the remaining elements from the right half */
    while(right <= end) {
        temp[temp_index] = array[right];
        temp_index++;
        right++;
    }

    /* move from temp array to the original array */
    int i;
    for(i = start; i <= end; i++) {
        array[i] = temp[i - start];
    }

    /* free the temporary array */
    free(temp);
}

void mergeSort(int* array, int start, int end) {
    
    if(start < end) {
        int middle = (start + end) / 2;

        
        /* sort left half */
        mergeSort(array, start, middle);

        /* sort right half */
        mergeSort(array, middle + 1, end);

        /* merge the two halves */
        merge(array, start, end);
    }
}


void l2g(int a[], int b[], int size){
    int i;
    for(i=0;i<size;i++)
    a[i]=b[i];
}

void p2a(int a [], int* b, int size){
    int i;
    for(i=0; i<size; i++){
        a[i] = b[i];
    }
}


int* mergeArrays(int a[], int b[], int n, int m){
        int* c;
        int size = n+m;
        c = malloc(size*sizeof(int));
        int i=0, j=0, k=0;
        while(i <= n-1 && j <= m-1){
        if(a[i] <= b[j]){
        c[k++] = a[i++];
        }
        else{
        c[k++] = b[j++];
        }
        }
        while (i <= n-1){
        c[k++] = a[i++];
        }
        while (j <= m-1){
        c[k++] = b[j++];
        }
        return c;
}


int main(int argc, char *argv[]) {
    int global_array[SIZE]; 
    int numm_array[SIZE];
    int*comp;
  
    int i;
   
    int  current_id, total_process;
    
        /* initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &current_id);
    MPI_Comm_size(MPI_COMM_WORLD, &total_process);
    if(current_id == 0) {
        
      /* put in random numbers */
        for(i = 0; i < SIZE; i++) {
           numm_array[i] = rand( ) % 100;
        }
        /* print them */
        printf("Unsorted numbers: ");
        for(i = 0; i < SIZE; i++) {
            printf("%d ", numm_array[i]);
        }
        
    }

    int divided_size = SIZE/total_process;
    int local_array[divided_size];
    MPI_Scatter(numm_array, divided_size, MPI_INT, local_array, divided_size, MPI_INT, 0, MPI_COMM_WORLD);
    if(current_id==0){
        mergeSort(local_array,0,divided_size-1);
        l2g(global_array, local_array, divided_size);
        int j, recv_size = divided_size;
        int buff[recv_size];
        for(j=0; j<total_process-1; j++){
             MPI_Recv(buff, recv_size,MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
             comp = mergeArrays(global_array, buff,divided_size, recv_size);
             divided_size += recv_size;
             p2a(global_array, comp, divided_size);
        }
    }else{
        mergeSort(local_array, 0, divided_size-1);
        MPI_Send(local_array, divided_size, MPI_INT, 0, 0,MPI_COMM_WORLD);
    }

   MPI_Finalize();

     if(current_id == 0) {
        printf("\nSorted numbers: ");
        for(i = 0; i < SIZE; i++) {
          printf("%d ", global_array[i]);
        }
     }
    return 0;
}

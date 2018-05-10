#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

#define SIZE 50000000

#define THRESHOLD 1000


void merge(int* array, int start, int end) {
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



void serialMergeSort(int* array, int start, int end) {
    if(start < end) {
        int middle = (start + end) / 2;

        /* sort left half */
        serialMergeSort(array, start, middle);

        /* sort right half */
        serialMergeSort(array, middle + 1, end);

        /* merge the two halves */
        merge(array, start, end);
    }
}


void mergeSort(int* array, int start, int end) {
    
    if(THRESHOLD>=(end-start)){

        serialMergeSort(array, start, end); 

    }else{

        if(start < end) {
        int middle = (start + end) / 2;

        /* sort both halves in parallel */
    
            #pragma omp parallel
            #pragma omp single nowait
            {

                #pragma omp task
                mergeSort(array, start, middle);
            
                mergeSort(array, middle + 1, end);
            
                /* merge the two halves */
                #pragma omp taskwait    
                merge(array, start, end);
            }
        }    
    }

    
}



int main( ) {
    int* nums = malloc(sizeof(int) * SIZE); 
    int i;
    time_t time_start, time_stop;

    /* put in random numbers */
    for(i = 0; i < SIZE; i++) {
        nums[i] = rand( ) % 100;
    }

    /* print them 
    printf("Unsorted numbers: ");
    for(i = 0; i < SIZE; i++) {
        printf("%d ", nums[i]);
    }*/
    
    int top = SIZE-1;
    time_start= time(NULL);
    mergeSort(nums, 0, top);
    time_stop = time(NULL);

    /* print them again 
    printf("\nSorted numbers: ");
    for(i = 0; i < SIZE; i++) {
        printf("%d ", nums[i]);
    }*/
    printf("\n Parallel Thresik Merge Sort take: %0.0f",difftime(time_stop,time_start));
    printf("\n");

    return 0;
}

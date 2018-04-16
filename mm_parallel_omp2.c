#include <time.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
  int    N; //size of columns and rows (matrices)
  clock_t start_t, end_t;
  double total_t;
  time_t time_start, time_stop;
  int	 tid, nthreads, i, j, k, l;
  double **a, **b, **c, **d, Z;
  double *a_block, *b_block, *c_block, *d_block;

  if(argc<2){
    printf("Usage: mm matrix_size\n");
    exit(-1);
  }

  N = atoi(argv[1]);

  a = (double **) malloc(N*sizeof(double *)); /* matrix a to be multiplied */
  b = (double **) malloc(N*sizeof(double *)); /* matrix b to be multiplied */
  c = (double **) malloc(N*sizeof(double *)); /* result matrix c */
  d = (double **) malloc(N*sizeof(double *)); /* result matrix d */

  a_block = (double *) malloc(N*N*sizeof(double)); /* Storage for matrices */
  b_block = (double *) malloc(N*N*sizeof(double));
  c_block = (double *) malloc(N*N*sizeof(double));
  d_block = (double *) malloc(N*N*sizeof(double));

  for (i=0; i<N; i++)   /* Initialize pointers to a */
    a[i] = a_block+i*N;

  for (i=0; i<N; i++)   /* Initialize pointers to b */
    b[i] = b_block+i*N;
  
  for (i=0; i<N; i++)   /* Initialize pointers to c */
    c[i] = c_block+i*N;

  for (i=0; i<N; i++)   /* Initialize pointers to d */
    d[i] = d_block+i*N;

  for (i=0; i<N; i++) /* last matrix has been initialized */
    for (j=0; j<N; j++)
      a[i][j]= (i+j) * ((double) rand());
  for (i=0; i<N; i++)
    for (j=0; j<N; j++)
      b[i][j]= i * j * ((double) rand());
  for (i=0; i<N; i++){
    for (j=0; j<N; j++){
      c[i][j]= 0.0;
      d[i][j]= 0.0;
    }
   }

  /* STEP 1: Z is the (integer) sum of ell elements of a anb b */
  start_t = clock(); 
  time_start= time(NULL);
  Z=0.0;
  /* last matrix has been initialized */
  
	    #pragma omp parallel 
      {
    		#pragma omp for  private(i,j) reduction(+:Z) schedule(runtime)
        for (i=0; i<N; i++) 
    		{
  	    	
          for (j=0; j<N; j++)
  	    	{
  	      		Z += (a[i][j] + b[i][j]) ;
  	    	}
  	    }	
       

  		#pragma omp single
      {
       printf("Value of Z: %e\n", Z);
 	  	 
      }

      nthreads = omp_get_num_threads();
  	  	/* STEP 2: variation of matrix multiplication */
  	  
  	  	    #pragma omp for nowait private(i,j,k,l) schedule(runtime)
            for (i=0; i<N; i++) 
            {
    	    		for(j=0; j<N; j++) 
              {    
    	      			
                  for (k=0; k<N; k++) 
                  {
    	        			c[i][j] += a[i][k] * b[k][j];
    						    for(l=0; l<i+N+N; l++) 
                    {
    		  					   c[i][j] -= ((Z*k)+l-k)/(l+k);
    	        			}
    	      			}
    	    		}
    	  		}
            
            /* STEP 3: add elements of matrices d=a+b */
            #pragma omp for nowait private(i,j) schedule(runtime)
            for (i=0; i<N; i++) 
            {
              
              for(j=0; j<N; j++) 
              {
                  d[i][j] = a[i][j] + b[i][j];
              }
            }
          }
  		
 		
	  
  
  
  end_t = clock();
  time_stop = time(NULL);
  total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC *1000.0;
  printf("Threads number is : %d. \n",nthreads);
  printf("Total time taken by CPU: %0.2f seconds. \n", total_t  );
  printf("Task time is about %0.0f seconds for matrix size equals to  %d \n",difftime(time_stop,time_start),N);
  printf ("Done.\n");
  exit(0);
}


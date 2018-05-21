#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
#include<sys/sysinfo.h>
int main(int argc,char *argv[]) {
	clock_t start, end;
     	double cpu_time_used;
	start = clock();
	if(argc!=3)
	{
		printf("usage: ./a.out size iterations\n");
		exit(1);
	}
	omp_set_num_threads(get_nprocs());
	int size = atoi(argv[1]);
	int iterations = atoi(argv[2]);
	//printf("size = %d , iterations = %d\n",size,iterations);
	int *first = (int *)malloc(size*size*sizeof(int));
	int *second = (int *)malloc(size*size*sizeof(int));
	int *multiply = (int *)malloc(size*size*sizeof(int));
	if(first ==NULL || second ==NULL || multiply == NULL)
	{
		printf("Failed to allocate memory\n");
		return -1;
	}
	int c,d,k,sum = 0;
	int tid,nthreads;
	# pragma omp parallel shared ( first, second, multiply, size ) private (c, d, k, nthreads, tid )
	{
	tid = omp_get_thread_num();
	printf("Hello World from thread = %d\n", tid);
	if (tid == 0) 
	 {
 		 nthreads = omp_get_num_threads();
		 printf("Number of threads = %d\n", nthreads);
 	}

	#pragma omp for
		for (c = 0; c < size; c++)
        	for (d = 0; d < size; d++){
            		*(first + c*size + d) = rand()%10+1;
			*(second + c*size +d) = rand()%10+1;
		}
	#pragma omp for
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++) {
        		for (k = 0; k < size; k++) {
          			sum = sum + *(first + c*size + k) * *(second+ k*size+d);
        		}
 
        	*(multiply+size*c+d) = sum;
        	sum = 0;
      		}
    	}
	}
    	printf("Product of the matrices:\n");
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++)
        		printf("%d\t", *(multiply+c*size+d));
      	printf("\n");
    	}
	end = clock();
	cpu_time_used = ((double)(end-start))/CLOCKS_PER_SEC;
	printf("Took %f seconds to execute\n",cpu_time_used);
	return 0;
}

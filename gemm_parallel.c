#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int size,iterations;
int *first,*second,*multiply,*multiply_seq;

#if defined(_OPENMP)
	#include<omp.h>
	#include<sys/sysinfo.h>
	;
	void gemm_omp()
	{
		omp_set_num_threads(get_nprocs());
		int c,d,k,tid,nthreads,sum = 0;
	# pragma omp parallel shared ( first, second, multiply, size ) private (c, d, k, nthreads, tid )
	{
	if (tid == 0) 
	 {
 		 nthreads = omp_get_num_threads();
		 printf("Number of threads = %d\n", nthreads);
 	}
	#pragma omp for
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++) {
        		for (k = 0; k < size; k++) {
          			sum = sum + *(first + c*size + k) + *(second+ k*size+d);
        		}

        	*(multiply+size*c+d) = sum;
        	sum = 0;
      		}
    	}
	}
	}
#else
	#include<pthread.h>
	struct v {
   	int i;
   	int j;
	};

	void *runner(void *param) {
   	struct v *data = param; // the structure that holds our data
   	int n, sum = 0; //the counter and sum

   	//Row multiplied by column
   	for(n = 0; n< size; n++){
      		sum += *(first+data->i*size+n) * *(second+n*size+data->j);
   	}
   	//assign the sum to its coordinate
   	*(multiply+data->i*size+data->j) = sum;

   	//Exit the thread
   	pthread_exit(0);
	}

	void gemm_pthread()
	{
	int c,d;
	pthread_t *tid = (pthread_t *)malloc(size*size*sizeof(pthread_t));
	pthread_attr_t attr;
        pthread_attr_init(&attr);
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++) {
        		struct v *data = (struct v *) malloc(sizeof(struct v));
         		if(data == NULL)
			{
				printf("Failed to allocate memory\n");
				return -1;
			}
			data->i = c;
         		data->j = d;
		pthread_create((tid+c*size+d),&attr,runner,data);
		}
	}
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++)
			pthread_join(*(tid+c*size+d),NULL);
	}

	}
#endif
void gemm_seq()
{
	int c,d,k,sum = 0;
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++) {
        		for (k = 0; k < size; k++) {
          			sum = sum + *(first + c*size + k) * *(second+ k*size+d);
        		}
 
        	*(multiply_seq+size*c+d)  = sum;
        	sum = 0;
      		}
    	}
}
int check()
{
	int c,d;
	for (c = 0; c < size; c++)
        	for (d = 0; d < size; d++){
            		if(*(multiply_seq + c*size + d) != *(multiply + c*size + d))
				return 0;
		}
	return 1;
}
int main(int argc,char *argv[])
{
	size = atoi(argv[1]);
	iterations = atoi(argv[2]);
	clock_t start, end;
     	double cpu_time_used;
	if(argc!=3)
	{
		printf("usage: ./a.out size iterations\n");
		exit(1);
	}

	first = (int *)malloc(size*size*sizeof(int));
	second = (int *)malloc(size*size*sizeof(int));
	multiply = (int *)malloc(size*size*sizeof(int));
	if(first ==NULL || second ==NULL || multiply == NULL)
	{
		printf("Failed to allocate memory\n");
		return -1;
	}
	int c,d,k,sum = 0;
	for (c = 0; c < size; c++)
        	for (d = 0; d < size; d++){
            		*(first + c*size + d) = rand()%10+1;
			*(second + c*size +d) = rand()%10+1;
		}
	#if defined(_OPENMP)
	start = clock();
	gemm_omp();
	end = clock();
	cpu_time_used = ((double)start-end)/CLOCKS_PER_SEC;
	printf("Executed using openmp\n Took %f seconds to execute\n",cpu_time_used);
	#else
	start = clock();
	gemm_pthread();
	end = clock();
	cpu_time_used = ((double)start-end)/CLOCKS_PER_SEC;
	printf("Executed using pthreads\n Took %f seconds to execute\n",cpu_time_used);
	#endif
	start = clock();
	gemm_pthread();
	end = clock();
	cpu_time_used = ((double)start-end)/CLOCKS_PER_SEC;
	printf("sequential execution took %f seconds to execute\n",cpu_time_used);

    	printf("Product of the matrices:\n");
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++)
        		printf("%d\t", *(multiply_seq+c*size+d));
      	printf("\n");
    	}
	if(check() == 1)
	{
		printf("multiplication is correct\n we can proceed\n");
	}
	return 0;
}

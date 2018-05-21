#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#if defined(_OPENMP)	//header files
	#include<omp.h>
	#include<sys/sysinfo.h>
#else
	#include<pthread.h>
	struct v {
	   int i;
	   int j;
	};
#endif
//global variables
int *first,*second,*multiply,*multiply_pthread,*multiply_omp;
int size,iterations;

void print_matrix(int *address)	//function to print a matrix
{
	int c,d;
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++)
        		printf("%d\t", *(address+c*size+d));
      	printf("\n");
    	}
}
#if defined(_OPENMP)
	//matrix multiplication using openmp
	void gemm_omp()
	{		int tid,nthreads,c,d,k,sum = 0;
			omp_set_num_threads(get_nprocs());
			# pragma omp parallel shared ( first, second, multiply_omp, size ) private (c, d, k,sum, nthreads, tid )
			{
			tid = omp_get_thread_num();
			if (tid == 0) 
			 {	
			 	//get number of threads available
		 		 nthreads = omp_get_num_threads();
				 printf("Number of threads = %d\n", nthreads);
		 	}
			#pragma omp for
			for (c = 0; c < size; c++) {
			  		for (d = 0; d < size; d++) {
						for (k = 0; k < size; k++) {
				  			sum = sum + *(first + c*size + k) * *(second+ k*size+d);
						}
		 
					*(multiply_omp+size*c+d) = sum;
					sum = 0;
			  		}
				}
			}
	}
#else

	void *runner(void *param) {
	   struct v *data = param; // the structure that holds our data
	   int n, sum = 0; //the counter and sum

	   //Row multiplied by column
	   for(n = 0; n< size; n++){
		  sum += *(first+data->i*size+n) * *(second+n*size+data->j);
	   }
	   //assign the sum to its coordinate
	   *(multiply_pthread+data->i*size+data->j) = sum;

	   //Exit the thread
	   pthread_exit(0);
	}
	//matrix multiplication using pthread
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
					exit(1);
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
int check(int *matrixA,int *matrixB)
{
	int c,d;
	for (c = 0; c < size; c++)
        	for (d = 0; d < size; d++){
            		if(*(matrixA + c*size + d) != *(matrixB + c*size + d))
				return 0;
		}
	return 1;
}
void gemm_seq()
{
	int c,d,k,sum = 0;
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++) {
        		for (k = 0; k < size; k++) {
          			sum = sum + *(first + c*size + k) * *(second+ k*size+d);
        		}
 
        	*(multiply+size*c+d)  = sum;
        	sum = 0;
      		}
    	}
}

int main(int argc,char *argv[])
{
	clock_t start, end;
     	double cpu_time_used;
	start = clock();
	if(argc!=3)
	{
		printf("usage: ./a.out size iterations\n");
		exit(1);
	}
	
	size = atoi(argv[1]);
	iterations = atoi(argv[2]);
	//printf("size = %d , iterations = %d\n",size,iterations);
	first = (int *)malloc(size*size*sizeof(int));
	second = (int *)malloc(size*size*sizeof(int));
	multiply = (int *)malloc(size*size*sizeof(int));
	if(first ==NULL || second ==NULL || multiply == NULL)
	{
		printf("Failed to allocate memory\n");
		return -1;
	}
	#if defined(_OPENMP)
	multiply_omp = (int *)malloc(size*size*sizeof(int));
	if(multiply_omp == NULL)
	{
		printf("Failed to allocate memory\n");
		return -1;
	}
	#else
	multiply_pthread = (int *)malloc(size*size*sizeof(int));
	if(multiply_pthread == NULL)
	{
		printf("Failed to allocate memory\n");
		return -1;
	}
	#endif
	int c,d,k,sum = 0;
	for (c = 0; c < size; c++)
        	for (d = 0; d < size; d++){
            		*(first + c*size + d) = rand()%10+1;
			*(second + c*size +d) = rand()%10+1;
		}
		printf("first matrix\n");
	print_matrix(first);
	printf("second matrix\n");
	print_matrix(second);
	gemm_seq();
    	printf("product matrix\n");
    	print_matrix(multiply);
    	#if defined(_OPENMP)
    	gemm_omp();
		printf("product by openmp\n");
		print_matrix(multiply_omp);
		#else
    	gemm_pthread();
    	printf("product by pthreads\n");
    	print_matrix(multiply_pthread);
    	#endif
    	#if defined(_OPENMP)
    		if(check(multiply,multiply_omp) == 1)
    			printf("functionality check success\n");
    		else
    			printf("functionality check failed\n");
    	#else
    		if(check(multiply,multiply_pthread) == 1)
    			printf("functionality check success\n");
    		else
    			printf("functionality check failed\n");
    	#endif
	return 0;
	}

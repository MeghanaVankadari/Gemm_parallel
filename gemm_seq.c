#include<stdio.h>
#include<stdlib.h>
#include<time.h>
double cpu_time_used = 0;
int main(int argc,char *argv[])
{
	clock_t start, end;
	start = clock();
	if(argc!=3)
	{
		printf("usage: ./a.out size iterations\n");
		exit(1);
	}
	
	int size = atoi(argv[1]);
	int iterations = atoi(argv[2]);
	//printf("size = %d , iterations = %d\n",size,iterations);
	int ite = 0;
	for(ite = 0; ite < iterations ; ite++)
	{
	start = clock();
	int *first = (int *)malloc(size*size*sizeof(int));
	int *second = (int *)malloc(size*size*sizeof(int));
	int *multiply = (int *)malloc(size*size*sizeof(int));
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
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++) {
        		for (k = 0; k < size; k++) {
          			sum = sum + *(first + c*size + k) * *(second+ k*size+d);
        		}
 
        	*(multiply+size*c+d)  = sum;
        	sum = 0;
      		}
    	}
 
    	/*printf("Product of the matrices:\n");
	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++)
        		printf("%d\t", *(multiply+c*size+d));
      	printf("\n");
    	}*/
	end = clock();
	cpu_time_used = cpu_time_used + ((double)(end-start))/CLOCKS_PER_SEC;
	}
	cpu_time_used = cpu_time_used / iterations;
	printf("%d	%f\n",size,cpu_time_used);
	return 0;
}

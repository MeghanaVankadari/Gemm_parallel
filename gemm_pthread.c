#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>

struct v {
   int i;
   int j;
};
int *first,*second,*multiply;
int size;
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
	int iterations = atoi(argv[2]);
	pthread_t *tid = (pthread_t *)malloc(size*size*sizeof(pthread_t));
	pthread_attr_t attr;
        pthread_attr_init(&attr);
	//printf("size = %d , iterations = %d\n",size,iterations);
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
	printf("first matrix\n");
	for (c = 0; c < size; c++) {
          	for (d = 0; d < size; d++)
         		printf("%d\t", *(first+c*size+d));
       	 printf("\n");
        	 }
    	printf("second matrix:\n");
    	for (c = 0; c < size; c++) {
      		for (d = 0; d < size; d++)
        		printf("%d\t", *(second+c*size+d));
      	printf("\n");
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

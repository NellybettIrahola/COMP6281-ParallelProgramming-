#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "sequential_quicksort.h"

#define MAXNUMBER 1000 

int main(int argc, char *argv[]) { 
    int* numbers;
    int i,numbelem;
    clock_t start_time, end_time;

    if ( argc != 2 )
      printf( "usage: %s <number of elements in array>\n", argv[0] );
    else
      numbelem = atoi( argv[1] );
    start_time = clock();
    srand(time(0));
    numbers=(int*) malloc(numbelem* sizeof(int));
    numbers=create_array_sequential(numbelem);
    
    //printf("The initial array is:\n");
    //for(i=0;i<numbelem;i++){
    //   printf("%d  ",numbers[i]);
    //}
    //printf("\n");

  
    quicksort(numbers, 0, numbelem-1); 
    //printf("Sorted array: \n"); 
    //for(i=0;i<numbelem;i++){
    //   printf("%d  ",numbers[i]);
    //}
    printf("calculate final result\n");

    end_time = clock();
    printf("Time taken is %f seconds \n", ((double)(end_time - start_time))/CLOCKS_PER_SEC);
    return 0; 
} 



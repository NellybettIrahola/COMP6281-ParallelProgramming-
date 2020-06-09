#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


#define MAXNUMBER 1000 

void swap(int* a, int* b){ 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 

int split_compare (int arr[], int lower, int upper){ 
    int pivot = arr[lower];    
    int i = (lower+1);  
    int j;
    for (j = lower+1; j <=upper; j++) 
    { 
        // If current element is smaller than the pivot 
        if (arr[j] < pivot) 
        { 
            swap(&arr[j],&arr[i]); 
            i++;
        } 
    } 
    swap(&arr[i - 1], &arr[lower]); 
    return (i-1); 
} 

void quicksort(int arr[], int lower, int upper){ 
    if (lower < upper) { 
	//pivot and comparison
        int partion_elem = split_compare(arr, lower, upper); 
  
        //Left and right
        quicksort(arr, lower, partion_elem - 1); 
        quicksort(arr, partion_elem + 1, upper); 
    } 
} 


  
//Creates an array of random numbers
int* create_array_sequential(int size){
int* numbers=(int*) malloc(size* sizeof(int));
int i;
for (i = 0; i < size; i++){
   numbers[i]=rand() % (MAXNUMBER + 1); 
}

return(numbers);
} 



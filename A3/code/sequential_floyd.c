/**********************************************************************
 * FILE: sequential_floyd.c
 * NAME: NELLYBETT IRAHOLA
 * ID: 40079991
 * DESCRIPTION:     
 *   This program implements a sequential version of Floyd's shortest
 *   path algortihm. 
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>

int* create_graph (int size);
int adding_elem(int a,int b);

#define MAXWEIGHT 20     
#define CONNPROB 100
#define CONNPROBASE 100


int main (int argc, char *argv[]){

int* numbers;
int* d;
int	numbelem,
        i,j,k;

clock_t start_time, end_time;


if ( argc != 2 )
      printf( "usage: %s <size of graph>\n", argv[0] );
else
      numbelem = atoi( argv[1] );

/* Set seed for random number generator equal to task ID */
   srand(time(0));
     
   numbers=create_graph(numbelem);
   d=(int*) malloc(numbelem*numbelem* sizeof(int));
   for(i=0;i<numbelem*numbelem;i++){
      d[i]=numbers[i];
   }
   /*
   printf("Initial graph \n");
   for (i = 0; i < numbelem; i++){
      for (k = 0; k < numbelem; k++){
         printf("%d ", d[i*numbelem+k]);
      }
      printf("\n");
   }
   */
   start_time = clock();

   for (k = 0; k < numbelem; k++){
      for (i = 0; i < numbelem; i++){
         for (j = 0; j < numbelem; j++){
            if(i==j || i==k || j==k)
               continue;
            if(d[i*numbelem+j]<adding_elem(d[i*numbelem+k],d[k*numbelem+j]))
               d[i*numbelem+j]=d[i*numbelem+j];
            else
               d[i*numbelem+j]=adding_elem(d[i*numbelem+k],d[k*numbelem+j]);  
            
         }
      }
   }  
   
   end_time = clock();
   printf("Time taken is %f seconds \n", ((double)(end_time - start_time))/CLOCKS_PER_SEC);
   /*
   printf("\nFinal graph \n");
   for (i = 0; i < numbelem; i++){
      for (k = 0; k < numbelem; k++){
         printf("%d ", d[i*numbelem+k]);
      }
      printf("\n");
   }
   */
return 0;
}


int adding_elem(int a,int b){
   if(a==INT_MAX || b==INT_MAX)
      return INT_MAX;

   return a+b;
}

int* create_graph(int size){
   int* numbers=(int*) malloc(size*size* sizeof(int));
   int i,k,prob;
   for (i = 0; i < size; i++){
      for (k = 0; k < size; k++){
         prob=rand() % (CONNPROBASE + 1);
         if(i==k){
            numbers[i*size+k]=0;
         }else{
            if(prob<=CONNPROB)
               numbers[i*size+k]=(rand() % MAXWEIGHT) + 1;//rand() % (MAXWEIGHT + 1);
            else
               numbers[i*size+k]=INT_MAX;
         }
      }
   }  

   return(numbers);
} 

/**********************************************************************
 * FILE: quicksort.c
 * OTHER FILES: sequential_quicksort.c
 * NAME: NELLYBETT IRAHOLA
 * ID: 40079991
 * DESCRIPTION:     
 *   This program implements a Parallel sorting algorithm known as
 *   Quicksort on assuming a hypercube  topology. 
**********************************************************************/
#include "mpi.h"
#include "sequential_quicksort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int* create_array (int size);
int get_partner(int d,int task, int* taskelem,int numtasks);

#define MAXNUMBER 1000      /* maximum number in the array */
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[]){
int* taskselem;
int* numbers;
int* sub_numbers;
int* final_result;
int* small_pivot;
int* bigger_pivot;
int* rec_array;
int	taskid,	        /* task ID */
	numtasks,       /* number of tasks */
        newnumtasks,
	numbelem,
        d,
        position_final,
        group_size,
        rc,             /* return code */
        i,
        m,
        pivot,
        aux,
        color,
        partner,
        k,
        pivot_rec,
        big,
        size_small,
        size_big,
        size_rec,
        size_subarray,
        leader,
        j;
MPI_Group  orig_group, new_group;
MPI_Comm   old_comm,new_comm;
clock_t start_time, end_time;
MPI_Status status;

/* Obtain number of tasks and task ID */
MPI_Init(&argc,&argv);

MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
printf ("MPI task %d has started...\n", taskid);


if ( argc != 2 )
      printf( "usage: %s <numbers in array>\n", argv[0] );
else
      numbelem = atoi( argv[1] );

/* Set seed for random number generator equal to task ID */
   srand(time(0));
   numbers=NULL;
   if (taskid == MASTER) {
     
      numbers=create_array(numbelem);
//      for(i=0;i<numbelem;i++){
//         printf("%d  ",numbers[i]);
//      }
      printf("\n");
   }

   taskselem = (int *)malloc(sizeof(int) * numtasks);
   for (j = 0; j < numtasks; j++) {
      taskselem[j]=j;
   }


// For each process, create a buffer that will hold a subset of the entire array
  sub_numbers = (int *)malloc(sizeof(int) * ((int) ceil(numbelem/numtasks)));

// Scatter the random numbers from the root process to all processes in the MPI world
  MPI_Scatter(numbers,(int) ceil(numbelem/numtasks), MPI_INT, sub_numbers,(int) ceil(numbelem/numtasks), MPI_INT, 0, MPI_COMM_WORLD);
MPI_Barrier(MPI_COMM_WORLD);

  size_subarray=(int)ceil(numbelem/numtasks);
  d=(int)log2(numtasks);
  free(numbers);
start_time = clock();


for (i = 0; i < d; i++) {
   
   group_size=(int) ceil(numtasks/(pow(2,i)));
   
//Leader selects pivot and BCast pivot to the group
   
   if(size_subarray!=0){
      pivot=sub_numbers[rand() % (size_subarray)];
   }else{
      pivot=-1;      
   }
  
   if((taskid+1) % group_size==0){
      
      for(m=1;m<group_size;m++){
          MPI_Recv(&pivot_rec, 1, MPI_INT,taskid-m, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          if(pivot==-1){
             if(pivot_rec!=-1)
                pivot=pivot_rec;
          }
      }
     
      for(m=1;m<group_size;m++){
          MPI_Send(&pivot, 1, MPI_INT,(taskid-m),1, MPI_COMM_WORLD);
      }
   }else{
      pivot_rec=pivot;
      leader=((int)ceil((double)(taskid+1)/group_size) * group_size)-1;
      MPI_Send(&pivot_rec, 1, MPI_INT,leader,1, MPI_COMM_WORLD);
      MPI_Recv(&pivot, 1, MPI_INT, leader, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   }



MPI_Barrier(MPI_COMM_WORLD);

//Compare and split

size_small=0;
size_big=0;
for(j=0;j<size_subarray;j++){
    if(sub_numbers[j]<pivot)
       size_small=size_small+1;
    else
       size_big=size_big+1;
}

if(size_small!=0)
   small_pivot=(int *)malloc(sizeof(int) * size_small);
else
   small_pivot=NULL;

if(size_big!=0)
   bigger_pivot=(int *)malloc(sizeof(int) * size_big);
else
   bigger_pivot=NULL;

size_small=0;
size_big=0;
for(j=0;j<size_subarray;j++){
    if(sub_numbers[j]<pivot){
       small_pivot[size_small]=sub_numbers[j];
       size_small=size_small+1;
    }else{
       bigger_pivot[size_big]=sub_numbers[j];
       size_big=size_big+1;
    }

}

free(sub_numbers);
MPI_Barrier(MPI_COMM_WORLD);

//Get partner
   partner=get_partner(d-i,taskid,taskselem,numtasks);


//Exchange with partner
   if(taskid>partner){
       
  
       MPI_Send(&size_small, 1, MPI_INT,partner,1, MPI_COMM_WORLD);
       MPI_Recv(&size_rec, 1, MPI_INT, partner, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       aux=size_rec;       
       
       size_rec=size_rec+size_big;
       if(size_rec!=0)
          sub_numbers=(int *)malloc(sizeof(int) * size_rec);
       else
          sub_numbers=NULL;
       
       //Send first
       if(size_small!=0){
          MPI_Send(small_pivot, size_small, MPI_INT,partner,2, MPI_COMM_WORLD);
       }
       
       //Receive later
       j=0;
       if(aux!=0){
           rec_array=(int *)malloc(sizeof(int) * aux);
           MPI_Recv(rec_array, aux, MPI_INT, partner, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           while(j<aux){
                sub_numbers[j]=rec_array[j];
                j=j+1;
           }
       }else{
          rec_array=NULL;
       }

    
       for(k=0;k<size_big;k++){
          sub_numbers[j]=bigger_pivot[k];
          j=j+1;
       }
       
   }else{
       
      
       MPI_Recv(&size_rec, 1, MPI_INT, partner, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       MPI_Send(&size_big, 1, MPI_INT,partner,1, MPI_COMM_WORLD);
      
       aux=size_rec;
       
       size_rec=size_rec+size_small;
     
       if(size_rec!=0)
           sub_numbers=(int *)malloc(sizeof(int) * size_rec);
       else
          sub_numbers=NULL;

       
       //Receive first
       j=0;
       if(aux!=0){
           rec_array=(int *)malloc(sizeof(int) * aux);
           MPI_Recv(rec_array, aux, MPI_INT, partner, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           while(j<aux){
                sub_numbers[j]=rec_array[j];
                j=j+1;
           }
       }else{
          rec_array=NULL;
       }

       for(k=0;k<size_small;k++){
          sub_numbers[j]=small_pivot[k];
          j=j+1;
       }
       
       //Send later
       if(size_big!=0){
          MPI_Send(bigger_pivot, size_big, MPI_INT,partner,2, MPI_COMM_WORLD);
       }
   

   }

MPI_Barrier(MPI_COMM_WORLD);

   size_subarray=size_rec;
   free(small_pivot);
   free(bigger_pivot);
   free(rec_array); 
} 
MPI_Barrier(MPI_COMM_WORLD);
quicksort(sub_numbers,0,size_subarray-1);


//Master gather the results from all the processes, others send result to master
if(taskid==0){
   final_result=(int*) malloc(numbelem* sizeof(int));
    
   for(i=0;i<size_subarray;i++){
      final_result[i]=sub_numbers[i];
   }
   position_final=size_subarray;


   //Get each process result
   for(i=1;i<numtasks;i++){
      rec_array=NULL;
      MPI_Recv(&size_rec, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if(size_rec==0)
         continue;

      rec_array=(int *)malloc(sizeof(int) * size_rec);
      MPI_Recv(rec_array, size_rec, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      
      for(j=0;j<size_rec;j++){
         final_result[position_final]=rec_array[j];
         position_final++;
      }

      free(rec_array);
   }

   //printf("The final array is:\n");
   //for(i=0;i<numbelem;i++){
   //   printf("%d  ",final_result[i]);
   //}
   //printf("\n");

   end_time = clock();
   printf("Time taken is %f seconds \n", ((double)(end_time - start_time))/CLOCKS_PER_SEC);

}else{
   MPI_Send(&size_subarray, 1, MPI_INT,0,1, MPI_COMM_WORLD);
   if(size_subarray!=0)
      MPI_Send(sub_numbers, size_subarray, MPI_INT,0,2, MPI_COMM_WORLD);
}



MPI_Finalize();
return 0;
}


//Returns the partner for a task
int get_partner(int d,int task, int* taskelem,int numtasks){
   int i;
   int dimenum=(int)pow(2,d-1);
   
   for(i=0;i<numtasks;i++){
       
       if((task ^ taskelem[i])==dimenum)
          return (taskelem[i]);
   }

   return (-1);
}

//Creates an array of random numbers
int* create_array(int size){
int* numbers=(int*) malloc(size* sizeof(int));
int i;
for (i = 0; i < size; i++){
   numbers[i]=rand() % (MAXNUMBER + 1); 
}

return(numbers);
} 

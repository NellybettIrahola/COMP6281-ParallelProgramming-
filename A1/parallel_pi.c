/**********************************************************************
 * FILE: mpi_pi_reduce.c
 * OTHER FILES: dboard.c
 * DESCRIPTION:  
 *   MPI pi Calculation Example - C Version 
 *   Collective Communication example:  
 *   This program calculates pi using a "dartboard" algorithm.  See
 *   Fox et al.(1988) Solving Problems on Concurrent Processors, vol.1
 *   page 207.  All processes contribute to the calculation, with the
 *   master averaging the values for pi. This version uses mpc_reduce to 
 *   collect results
 * AUTHOR: Blaise Barney. Adapted from Ros Leibensperger, Cornell Theory
 *   Center. Converted to MPI: George L. Gusciora, MHPCC (1/95) 
 * LAST REVISED: 06/13/13 Blaise Barney
**********************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int* create_array (int size);

#define ROUNDS 100      /* number of times "darts" is iterated */
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[]){
int* taskselem;
int* numbers;
int* sub_numbers;
int	taskid,	        /* task ID - also used as seed number */
	numtasks,       /* number of tasks */
	numbelem,
   d,
   group_size,
   rc,             /* return code */
	i,
   k,
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
      printf( "usage: %s <number of workers>\n", argv[0] );
else
      numbelem = atoi( argv[1] );

/* Set seed for random number generator equal to task ID */
srand(time(0));

if (taskid == MASTER) {
   numbers=create_array(numbelem);
   taskselem = (int *)malloc(sizeof(int) * numtasks);
   for (j = 0; j < numtasks; i++) {
      taskselem[j]=j;
   }
}

// For each process, create a buffer that will hold a subset of the entire array
  sub_numbers = (int *)malloc(sizeof(int) * (numbelem/numtasks));


// Scatter the random numbers from the root process to all processes in the MPI world
  MPI_Scatter(numbers, numbelem/numtasks, MPI_INT, sub_numbers,numbelem/numtasks, MPI_INT, 0, MPI_COMM_WORLD);

d=log2(numtasks);
old_comm=MPI_COMM_WORLD;
start_time = clock();


for (i = 0; i < d; i++) {
   
   group_size=numtasks/(pow(2,i));
   color=taskid % group_size;
   MPI_Comm_split (old_comm, color, rank,  &new_comm ); 
   
   if(taskid % group_size==0){

   }
   
   rc = MPI_Reduce(&homepi, &pisum, 1, MPI_DOUBLE, MPI_SUM,
                   MASTER, MPI_COMM_WORLD);

   /* Master computes average for this iteration and all iterations */
   if (taskid == MASTER) {
      pi = pisum/numtasks;
      avepi = ((avepi * i) + pi)/(i + 1); 
      printf("   After %8d throws, average value of pi = %10.8f\n",
              (DARTS * (i + 1)),avepi);
   }    
    
} 

end_time = clock();
printf("Time taken is %f seconds \n", ((double)(end_time - start_time))/CLOCKS_PER_SEC);

MPI_Finalize();
return 0;
}



/**************************************************************************
* subroutine dboard
* DESCRIPTION:
*   Used in pi calculation example codes. 
*   the circle."  If so, score is incremented.  After throwing the 
*   specified number of darts, pi is calculated.  The computed value 
*   of pi is returned as the value of this function, dboard. 
*
*   Explanation of constants and variables used in this function:
*   darts       = number of throws at dartboard
*   score       = number of darts that hit circle
*   n           = index variable
****************************************************************************/

int* create_array(int size){
int* numbers=(int*) malloc(size* sizeof(int));

for (int i = 0; i < size; i++){
   numbers[i]=rand();
}

return(numbers);
} 

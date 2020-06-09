/**********************************************************************
 * FILE: parallel_floyd_s1.c
 * NAME: NELLYBETT IRAHOLA
 * ID: 40079991
 * DESCRIPTION:     
 *   This program implements a Parallel version of Floyd's shortest
 *   path algortihm using 2D block mapping with broadcast. 
**********************************************************************/
#include "mpi.h"
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
#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[]){
int* numbers;
int* row_deposit;
int* send_elems;
int* col_deposit;
int coor_ij[2];
int	taskid,	        /* task ID */
	numtasks,       /* number of tasks */
        numbelem,
        numelem_p,
        pr,
        k_owner,
        x_coor,
        y_coor,
        select_x,
        select_y,
        start,
        i,j,k;


MPI_Comm   col_comm,row_comm;
clock_t start_time, end_time;
MPI_Status status;

/* Obtain number of tasks and task ID */
MPI_Init(&argc,&argv);

MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
printf ("MPI task %d has started...\n", taskid);


if ( argc != 2 )
      printf( "usage: %s <size of graph>\n", argv[0] );
else
      numbelem = atoi( argv[1] );

   pr=sqrt(numtasks);
   if(pr*pr!=numtasks || pr>numbelem || numbelem%pr!=0){
      
      if(taskid==MASTER)
         printf("Error processors number");
      
      MPI_Abort(MPI_COMM_WORLD,1);
      return -1;
   }

/* Set seed for random number generator equal to task ID */
   srand(time(0));

   if(taskid==MASTER)
      numbers=create_graph(numbelem);
   else
      numbers=(int*) malloc(numbelem*numbelem* sizeof(int));
   
   //Bcast graph 
   MPI_Bcast(numbers,numbelem*numbelem,MPI_INT,MASTER,MPI_COMM_WORLD);
   
   if(taskid==MASTER){
   /*   
   printf("\nInitial graph \n");
   for (i = 0; i < numbelem; i++){
      for (k = 0; k < numbelem; k++){
         printf("%d ", numbers[i*numbelem+k]);
      }
      printf("\n");
   }
   printf("\n");
   */
   }
   start_time = clock();

   // Determine Pij

   coor_ij[0]=taskid/pr;
   coor_ij[1]=taskid%pr;
   
   // Define Bcast groups
   
   MPI_Comm_split(MPI_COMM_WORLD,coor_ij[0],coor_ij[1],&row_comm);
   MPI_Comm_split(MPI_COMM_WORLD,coor_ij[1],coor_ij[0],&col_comm);

   // Define group of elements per process
   numelem_p=numbelem/pr;
 
   // Start Floyd
   int coord_k[2];   
   for(k=0;k<numbelem;k++){
      k_owner=k/numelem_p;
      coord_k[0]=k/pr;
      coord_k[1]=k%pr;

      // Arrays for row and column
      row_deposit=(int*)malloc(numelem_p*sizeof(int));
      col_deposit=(int*)malloc(numelem_p*sizeof(int));

      //Validate k is in process rows
      if(k_owner==coor_ij[1]){
         for(j=0;j<numelem_p;j++){
            col_deposit[j]=numbers[((numelem_p*coor_ij[0])*numbelem+(numbelem*j))+k];
         }
      }
      MPI_Bcast(col_deposit,numelem_p,MPI_INT,k_owner,row_comm);
      

      //Validate k is in process columns
      if(k_owner==coor_ij[0]){
         for(j=0;j<numelem_p;j++){
            row_deposit[j]=numbers[(k*numbelem)+(numelem_p*coor_ij[1]+j)];
         }
      }
      MPI_Bcast(row_deposit,numelem_p,MPI_INT,k_owner,col_comm);
    
      
      for(j=0;j<numelem_p;j++){
         for(i=0;i<numelem_p;i++){
            x_coor=numelem_p*coor_ij[0]+i;
            y_coor=numelem_p*coor_ij[1]+j;
    
            if(x_coor==y_coor || y_coor==k || x_coor==k)
               continue;
            
               select_x=col_deposit[i];
               select_y=row_deposit[j];
           
            if(numbers[x_coor*numbelem+y_coor]>adding_elem(select_x,select_y))
               numbers[x_coor*numbelem+y_coor]=adding_elem(select_x,select_y);
         
         }
      }
      
      //printf("\ntaskid:%d Partial graph \n",taskid);
      //for (i = 0; i < numbelem; i++){
      //   for (j = 0; j < numbelem; j++){
      //      printf("%d ", numbers[i*numbelem+j]);
      //   }
      //   printf("\n");
      //}
      //printf("\n");

      MPI_Barrier(MPI_COMM_WORLD);

      free(col_deposit);
      free(row_deposit);
   }  

      if(taskid!=MASTER){
         send_elems=(int*) malloc(numelem_p*numelem_p*sizeof(int));
         for(i=0;i<numelem_p;i++){
            for(j=0;j<numelem_p;j++){
               x_coor=numelem_p*coor_ij[0]+i;
               y_coor=numelem_p*coor_ij[1]+j;
               send_elems[i*numelem_p+j]=numbers[x_coor*numbelem+y_coor];
            }
         }
         MPI_Send(send_elems, numelem_p*numelem_p, MPI_INT, MASTER, 0, MPI_COMM_WORLD);         
         free(send_elems);

      }else{
         for(k=1;k<numtasks;k++){
            int* receive_elems = (int*)malloc(numelem_p*numelem_p * sizeof(int));
            MPI_Recv(receive_elems,numelem_p*numelem_p, MPI_INT, k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            for(i=0;i<numelem_p;i++){
               for(j=0;j<numelem_p;j++){
                  x_coor=numelem_p*(k/pr)+i;
                  y_coor=numelem_p*(k%pr)+j;
                  numbers[x_coor*numbelem+y_coor]=receive_elems[i*numelem_p+j];
               }
            }

         }  
         /*    
         printf("\ntaskid:%d Final graph \n",taskid);
         for (i = 0; i < numbelem; i++){
            for (j = 0; j < numbelem; j++){
               printf("%d ", numbers[i*numbelem+j]);
            }
            printf("\n");
         }
         printf("\n");
         */
         end_time = clock();
         printf("Time taken is %f seconds \n", ((double)(end_time - start_time))/CLOCKS_PER_SEC);
   }

   

MPI_Finalize();
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

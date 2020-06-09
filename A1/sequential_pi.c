#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
void srandom (unsigned seed);

int main(int argc, char *argv[]){

/*Variable initialization*/

#define sqr(x)	((x)*(x))
long random(void);
double x_coord, y_coord, pi,r,avepi; 
int score,k, n, darts, rounds, tasks,i;
clock_t start_time,end_time;
unsigned int cconst;  /* must be 4-bytes in size */
darts=15728640;
rounds=100;
start_time=clock();
MPI_Init(&argc,&argv);
/*************************************************************************
 * The cconst variable must be 4 bytes. We check this and bail if it is
 * not the right size
 ************************************************************************/

if (sizeof(cconst) != 4) {
   printf("Wrong data size for cconst variable in dboard routine!\n");
   printf("See comments in source file. Quitting.\n");
   exit(1);
   }
   
   cconst = 2 << (31 - 1);
   srandom(time(0));
   avepi = 0;

for (i = 0; i < rounds; i++) {

   score=0;   
   /* "throw darts at board" */
   for (n = 1; n <= darts; n++)  {
      
     /* generate random numbers for x and y coordinates */
      r =(double)random()/cconst;
      x_coord = (2.0 * r) - 1.0;

      r =(double)random()/cconst;
      y_coord = (2.0 * r) - 1.0;
     
      /* if dart lands in circle, increment score */
      if ((sqr(x_coord) + sqr(y_coord)) <= 1.0)
           score++;
      }

/* calculate pi */
      pi = 4.0 * (double)score/(double)darts;
      avepi = ((avepi * i) + pi)/(i + 1);
      printf("After %d throws, average value of pi = %10.8f\n",(darts * i),avepi);
}

printf("After %d throws, average value of pi = %10.8f\n",(darts * rounds),avepi);

end_time = clock();
printf("Time taken is %lf seconds \n", ((double)(end_time - start_time))/CLOCKS_PER_SEC);

MPI_Finalize();
return 0;
}

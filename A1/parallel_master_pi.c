#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define MASTER 0
void srandom (unsigned seed);
double dboard (int darts);
#define DARTS 15728640
#define ROUNDS 100

int main (int argc, char *argv[])
{

double  homepi,         /* value of pi calculated by current task */
        pisum,          /* sum of tasks' pi values */
        pi,             /* average of pi after "darts" is thrown */
        avepi;          /* average pi value for all iterations */
int     taskid,         /* task ID - also used as seed number */
        numtasks,	/* number of tasks */
        rc,             /* return code */
        commtasks,
        i;
clock_t start_time, end_time;
start_time = clock();
    MPI_Comm child_processes;

    /* Obtain number of tasks and task ID */
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

    if ( argc != 2 )
        printf( "usage: %s <number of workers>\n", argv[0] );
    else
        numtasks = atoi( argv[1] );

    printf ("Parent task id is: %d. Total tasks in parent comm are: %d \n", taskid, numtasks);

    /*
    int MPI_Comm_spawn(
        const char *command,
                        char *argv[],
                        int maxprocs,
                        MPI_Info info,
                        int root,
                        MPI_Comm comm,
                        MPI_Comm *intercomm,
                        int array_of_errcodes[])
    */

MPI_Comm_spawn( "parallel_child_pi.o", argv, numtasks, MPI_INFO_NULL, 0, MPI_COMM_SELF, &child_processes, MPI_ERRCODES_IGNORE );

/* Set seed for random number generator equal to task ID */
srandom (taskid);

avepi = 0;
for (i = 0; i < ROUNDS; i++) {
   /* All tasks calculate pi using dartboard algorithm */
  
   /* Use MPI_Reduce to sum values of homepi across all tasks
    * Master will store the accumulated value in pisum
    * - homepi is the send buffer
    * - pisum is the receive buffer (used by the receiving task only)
    * - the size of the message is sizeof(double)
    * - MASTER is the task that will receive the result of the reduction
    *   operation
    * - MPI_SUM is a pre-defined reduction function (double-precision
    *   floating-point vector addition).  Must be declared extern.
    * - MPI_COMM_WORLD is the group of tasks that will participate.
    */

   rc = MPI_Reduce(&homepi, &pisum, 1, MPI_DOUBLE, MPI_SUM, MPI_ROOT, child_processes);
   /* Master computes average for this iteration and all iterations */
   pi = pisum/numtasks;
   avepi = ((avepi * i) + pi)/(i + 1);

}
    end_time = clock();
    printf("   After %8d throws, average value of pi = %10.8f\n",(DARTS * ROUNDS),avepi);
    printf("Time taken is %f seconds \n", ((double)(end_time - start_time))/CLOCKS_PER_SEC);
    MPI_Finalize();

    return 0;
}


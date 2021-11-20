#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 100.000      // trabalho final com o valores 10.000, 100.000, 1.000.000

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) && trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

int main(int argc, char** argv)
{
    int vetor[ARRAY_SIZE];
    int i;
    int id;            /* Identificador do processo */
    int n;             /* Numero de processos */
    double time1, time2;
    time1 = MPI_Wtime();

    for (i=0 ; i<ARRAY_SIZE; i++)              /* init array with worst case for sorting */
        vetor[i] = ARRAY_SIZE-i;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    #ifdef DEBUG
    printf("\nunsorted Array: ");
    for (i=0 ; i<ARRAY_SIZE; i++)              /* print unsorted array */
        printf("[%03d] ", vetor[i]);
    #endif

    bs(ARRAY_SIZE, vetor);                     /* sort array */


    #ifdef DEBUG
    printf("\nsorted Array: ");
    for (i=0 ; i<ARRAY_SIZE; i++)                              /* print sorted array */
        printf("[%03d] ", vetor[i]);
    #endif


    time2 = MPI_Wtime();
    printf("\nsize: %d", ARRAY_SIZE);
    printf("\ntime: %f\n", time2-time1);

    MPI_Finalize();
    return 0;
}
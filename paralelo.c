#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define tam_vetor 40      // trabalho final com o valores 10.000, 100.000, 1.000.000

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

int *interleaving(int vetor[], int tam)
{
	int *vetor_auxiliar;
	int i1, i2, i_aux;

	vetor_auxiliar = (int *)malloc(sizeof(int) * tam);

	i1 = 0;
	i2 = tam / 2;

	for (i_aux = 0; i_aux < tam; i_aux++) {
		if (((vetor[i1] <= vetor[i2]) && (i1 < (tam / 2))) || (i2 == tam))
			vetor_auxiliar[i_aux] = vetor[i1++];
		else
			vetor_auxiliar[i_aux] = vetor[i2++];
	}

	return vetor_auxiliar;
}

int main(int argc, char** argv)
{
    int vetor[tam_vetor];
    int i;
    int id;            /* Identificador do processo */
    int n;             /* Numero de processos */
    double time;
    int pai;
    MPI_Status Status; /* Status de retorno */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    printf("id: %d n: %d", id, n);

    if (id == 0) { //Raiz
        for (i=0 ; i<tam_vetor; i++)              /* init array with worst case for sorting */
            vetor[i] = tam_vetor-i;
        #ifdef DEBUG
            printf("\nunsorted Array: ");
            for (i=0 ; i<tam_vetor; i++)              /* print unsorted array */
                printf("[%03d] ", vetor[i]);
        #endif
    }
    else {
        printf("\nentrou filho");
        if (id % 2){
            printf("\nid percent 2");
            pai = (id - 1)/2;
        } else {
            printf("\nelse");
            pai = (id - 2)/2;
        }
        printf("\npassou do pai");
        printf("\nmeu pai: %d", pai);
        printf("\nsou filho, id: %d , meu pai eh: %d", id, pai);
        MPI_Recv(vetor, tam_vetor, MPI_INT, pai, 2, MPI_COMM_WORLD, &Status);
        MPI_Get_count(&Status, MPI_INT, tam_vetor);  // descubro tamanho da mensagem recebida
        printf("\ntam_vetor: %d, pai: %d, id: %d ", tam_vetor, pai, id);
    }
    if ( id >= n-2 ){
        printf("\nconquisto");
        bs(tam_vetor, vetor);  // conquisto
    }
    else {
        printf("\ndividir");
        // // dividir
        // // quebrar em duas partes e mandar para os filhos
        int left = (id * 2) + 1;
        int right = (id * 2) + 2;
        MPI_Send(&vetor[0],tam_vetor/2, MPI_INT, left, 1, MPI_COMM_WORLD);
        MPI_Send(&vetor[tam_vetor/2],tam_vetor/2,MPI_INT, right, 1, MPI_COMM_WORLD);

        // // receber dos filhos


        MPI_Recv(&vetor[0], tam_vetor/2, MPI_INT, left, 2, MPI_COMM_WORLD, &Status);
        MPI_Recv(&vetor[tam_vetor/2], tam_vetor/2, MPI_INT, right, 2, MPI_COMM_WORLD, &Status);
        // MPI_Recv ( &vetor[0], filho esquerda);
        // MPI_Recv ( &vetor[tam_vetor/2], filho direita);

        // // intercalo vetor inteiro
        // vetor = interleaving(vetor, tam_vetor/2);
    }
    if ( id !=0 ){
        printf("\nSEND");
        MPI_Send(&vetor, tam_vetor, MPI_INT, pai, 1, MPI_COMM_WORLD);
        // MPI_Send ( vetor, pai, tam_vetor );  // tenho pai, retorno vetor ordenado pra ele
    }
    else {
        printf("\nsorted Array: ");
        for (i=0 ; i<tam_vetor; i++)                              /* sou o raiz, mostro vetor */
            printf("[%03d] ", vetor[i]);

        time += MPI_Wtime();
        printf("\nsize: %d", tam_vetor);
        printf("\ntime: %f\n", MPI_Wtime());
    }



    MPI_Finalize();
    return 0;
}
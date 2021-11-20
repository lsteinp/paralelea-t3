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
    double time1, time2;
    int pai;
    MPI_Status Status; /* Status de retorno */
    int tam = tam_vetor;
    int *vetor_auxiliar;
    int leaf;
    int delta;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    time1 = MPI_Wtime();

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
        if (id % 2){
            pai = (id - 1)/2;
        } else {
            pai = (id - 2)/2;
        }
        MPI_Recv(vetor, tam, MPI_INT, pai, 1, MPI_COMM_WORLD, &Status);
        MPI_Get_count(&Status, MPI_INT, &tam);  // descubro tamanho da mensagem recebida
        printf("\ntam: %d, pai: %d, id: %d ", tam, pai, id);
    }
    if ( id >= (n-1)/2 ){
        printf("\nconquisto: %d", id);
        leaf = 1;
        bs(tam, vetor);  // conquisto
    }
    else {
        printf("\ndividir: %d", id);
        leaf = 0;
        // // dividir
        // // quebrar em duas partes e mandar para os filhos
        int left = (id * 2) + 1;
        int right = (id * 2) + 2;
        MPI_Send(&vetor[0],tam/2, MPI_INT, left, 1, MPI_COMM_WORLD);
        MPI_Send(&vetor[tam/2],tam/2,MPI_INT, right, 1, MPI_COMM_WORLD);

        // // receber dos filhos
        MPI_Recv(&vetor[0], tam/2, MPI_INT, left, 2, MPI_COMM_WORLD, &Status);
        MPI_Recv(&vetor[tam/2], tam/2, MPI_INT, right, 2, MPI_COMM_WORLD, &Status);

        // // intercalo vetor inteiro
        vetor_auxiliar = interleaving(vetor, tam);
        // if(id == 0){
            printf("\nid: %d,vetor normal: ", id);
            for (i=0 ; i<tam; i++)                              /* sou o raiz, mostro vetor */
                printf("[%03d] ", vetor[i]);
            print("\n\n")
            printf("\n id: %d,vetor intercalado: ", id);
            for (i=0 ; i<tam; i++)                              /* sou o raiz, mostro vetor */
                printf("[%03d] ", vetor_auxiliar[i]);
        // }
    }
    if ( id !=0 ){
        if (leaf){ // sou folha, manda o array que ordenei
        printf("\nsou folha: %d,retornando array ordenado para: %d\n", id, pai);
            MPI_Send(&vetor, tam, MPI_INT, pai, 2, MPI_COMM_WORLD);
        } else { // nao sou folha, mando o array intercalado
            printf("\nnao sou folha: %d,retornando array ordenado para: %d\n", id, pai);
            MPI_Send(&vetor_auxiliar, tam, MPI_INT, pai, 2, MPI_COMM_WORLD);
        }
    }
    else {
        printf("\nsorted Array: ");
        for (i=0 ; i<tam; i++)                              /* sou o raiz, mostro vetor */
            printf("[%03d] ", vetor_auxiliar[i]);

        time2 = MPI_Wtime();
        printf("\nsize: %d", tam);
        printf("\ntime: %f\n", time2-time1);
    }



    MPI_Finalize();
    return 0;
}
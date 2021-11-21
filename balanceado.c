#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

// #define DEBUG 1            // comentar esta linha quando for medir tempo
#define tam_vetor 1000000     // trabalho final com o valores 10000, 100000, 1000000

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
    int leaf = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    int delta = tam_vetor/n;
    int rest = tam_vetor % delta;
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
        if (id % 2){ //calcula id do pai
            pai = (id - 1)/2;
        } else {
            pai = (id - 2)/2;
        }
        MPI_Recv(vetor, tam, MPI_INT, pai, 1, MPI_COMM_WORLD, &Status);//recebo carga de trabalho do meu pai
        MPI_Get_count(&Status, MPI_INT, &tam);  // descubro tamanho da mensagem recebida
    }
    if ( id >= (n-1)/2 ){//valido se sou folha para conquistar
        leaf = 1;
        bs(tam, vetor);  // conquisto
    }
    else {
        // dividir

        //retiro uma parte para mim
        // int aux[delta] = &vetor[0];

        // quebro o restante e mandar para os filhos
        int left = (id * 2) + 1;
        int right = (id * 2) + 2;


        //envio o resto
        MPI_Send(&vetor[0], delta, MPI_INT, left, 1, MPI_COMM_WORLD);
        MPI_Send(&vetor[delta], delta,MPI_INT, right, 1, MPI_COMM_WORLD);

        //ordeno minha parte
        if(id ==0){
            bs(delta+rest, &vetor[delta*2]);
        } else {
            bs(delta, &vetor[delta*2]);
        }
        // receber dos filhos
        MPI_Recv(&vetor[0], delta, MPI_INT, left, 2, MPI_COMM_WORLD, &Status);
        MPI_Recv(&vetor[delta], delta, MPI_INT, right, 2, MPI_COMM_WORLD, &Status);

        // intercalo vetor inteiro
        vetor_auxiliar = interleaving(vetor, tam);
    }
    if ( id !=0 ){
        if (leaf){ // sou folha, manda o array que ordenei
            MPI_Send(&vetor, tam, MPI_INT, pai, 2, MPI_COMM_WORLD);
        } else { // nao sou folha, mando o array intercalado
            MPI_Send(vetor_auxiliar, tam, MPI_INT, pai, 2, MPI_COMM_WORLD);
        }
    }
    else {// sou raiz, mostrar o vetor e o tempo de execução

        #ifdef DEBUG
            printf("\nsorted Array: ");
            for (i=0 ; i<tam; i++)
                printf("[%03d] ", vetor_auxiliar[i]);

        #endif
        time2 = MPI_Wtime();
        printf("\nsize: %d", tam);
        printf("\ntime: %f\n", time2-time1);
    }

    MPI_Finalize();
    return 0;
}
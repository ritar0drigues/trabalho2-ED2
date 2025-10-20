#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tad.h"


int seed(int num, NoRB **raiz){

    srand((unsigned) time(NULL));

    dado* sobe = NULL;
    int flag = 0;

    for(int i=0;i<num;i++){
        int rnd = rand() % 1000000;
        char nome[100];
        char estilo[50];
        snprintf(nome, sizeof(nome), "%dartista", rnd);
        snprintf(estilo, sizeof(estilo), "estilo%d", rand()%10);

        dado info;

        // copia as strings para os arrays, garantindo terminação
        strncpy(info.artista.nome, nome, 49);
        info.artista.nome[49] = '\0';

        strncpy(info.artista.estilo, estilo, 49);
        info.artista.estilo[49] = '\0';

        info.artista.qtd_albuns = 0;
        info.artista.albuns = NULL;


        inserirNo(raiz, &info, NO_ARTISTA, NULL, &sobe, &flag);
    }

    printf("Inseridos %d artistas.\n", num);
    imprimirArvore(*raiz);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tad.h"

int seed(int num,NoRB **raiz) {

    srand((unsigned) time(NULL));

    dado* sobe = NULL;
    int flag = 0;
    num = 50; // número de artistas que serão inseridos

    for(int i = 0; i < num; i++) {
        char nome[50];    // nome do artista, até 49 caracteres + '\0'
        char estilo[50];

        // nome começa com o número e depois um texto
        snprintf(nome, sizeof(nome), "%dartista", i); // exemplo: "0-artista", "1-artista", etc.

        // estilo aleatório "estilo0" a "estilo9"
        snprintf(estilo, sizeof(estilo), "estilo%d", rand() % 10);

        dado info;

        // copia as strings para os arrays da struct, garantindo terminação
        strncpy(info.artista.nome, nome, 49);
        info.artista.nome[49] = '\0';

        strncpy(info.artista.estilo, estilo, 49);
        info.artista.estilo[49] = '\0';

        info.artista.qtd_albuns = 0;
        info.artista.albuns = NULL;

        // insere no nó da árvore
        inserirNo(raiz, &info, NO_ARTISTA, NULL, &sobe, &flag);
    }

    printf("Inseridos %d artistas.\n", num);
    imprimirArvore(*raiz);

    return 0;
}




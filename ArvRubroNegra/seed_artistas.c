#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tad.h"

// Compilar: gcc -o seed_artistas seed_artistas.c ArtistasRB.c mainRB.c -I.

int seed(int num, NoRB **raiz){
    int n = 1000; // número padrão de artistas

    srand((unsigned) time(NULL));

    

    for(int i=0;i<num;i++){
        int rnd = rand() % 1000000; // número aleatório (mais seguro para snprintf)
        char nome[100];
        char estilo[50];
        snprintf(nome, sizeof(nome), "%dartista", rnd);
        snprintf(estilo, sizeof(estilo), "estilo%d", rand()%10);

        DadosArtista artista;
        artista.nome = nome; // atenção: criarNo faz strdup
        artista.estilo = estilo;
        artista.qtd_albuns = 0;
        artista.albuns = NULL;

        inserirNo(raiz, &artista, NO_ARTISTA, NULL);
        if(*raiz) (*raiz)->cor = PRETO;
    }
    printf("Inseridos %d artistas.\n", num);
    imprimirArvore(*raiz);

    return 0;
}

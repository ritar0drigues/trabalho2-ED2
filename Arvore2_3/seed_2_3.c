#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tad.h"

// Compilar: gcc -o seed_2_3.exe seed_2_3.c FuncGenéricas2_3.c main2_3.c album.c Musicas2_3.c utils2_3.c -I. -std=c11

int seed(int num, NoRB **raiz){

    srand((unsigned) time(NULL));

    void *sobe = NULL;
    int flag = 0;

    for(int i=0;i<num;i++){
        int rnd = rand() % 1000000;
        char nome[100];
        char estilo[50];
        snprintf(nome, sizeof(nome), "%dartista", rnd);
        snprintf(estilo, sizeof(estilo), "estilo%d", rand()%10);

        DadosArtista artista;
        artista.nome = strdup(nome); 
        artista.estilo = strdup(estilo);
        artista.qtd_albuns = 0;
        artista.albuns = NULL;

    inserirNo(raiz, &artista, NO_ARTISTA, NULL, &sobe, &flag);
    }

    printf("Inseridos %d artistas.\n", num);
    imprimirArvore(*raiz);

    return 0;
}

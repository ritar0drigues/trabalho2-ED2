#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void libera_musicas(Musica* lista){
    if(lista){
        Musica* atual;
        while(lista!=NULL){
            atual = lista;
            lista = lista->prox;
            free(atual->titulo);
            free(atual->minutos);
            free(atual);
        }
    }
}

Musica* criar_musica(char* titulo,char* minutos){
    Musica* novo = (Musica*) malloc(sizeof(Musica));
    novo->minutos = strdup(minutos);
    novo->titulo = strdup(titulo);
    novo->prox = NULL;
    return novo;
}

void cadastra_musica(Musica** lista,char* titulo,char* duracao,int *flag){
    if(*lista){
        Musica* atual = *lista;
        Musica* anterior = NULL;
        while(atual!=NULL && strcasecmp(atual->titulo,titulo)<0){
            anterior = atual;
            atual = atual->prox;
        }
        Musica* novo = criar_musica(titulo,duracao);
        *flag = 1;
        if(anterior == NULL) { 
            novo->prox = *lista;
            *lista = novo;
        } else {
            anterior->prox = novo;
            novo->prox = atual;
        }
    }
    else{
        Musica* novo = criar_musica(titulo,duracao);
        (*lista) = novo;
        *flag = 1;
    }
}

void exibe_musicas(Musica* lista){
    while(lista!=NULL){
        printf("Musica: %s\n", lista->titulo);
        printf("Duracao: %s\n", lista->minutos);
        lista = lista->prox;
    }
}

Musica* Busca_musicas(Musica* lista,char* nome){
    while(lista!=NULL && strcmp(lista->titulo,nome)!=0){
        lista = lista->prox;
    }
    return lista;
}

void remove_musicas(Musica** lista,char *titulo,int* flag){
    Musica* anterior  = NULL;
    Musica* atual = (*lista);
    while(atual!=NULL && strcasecmp(atual->titulo,titulo)!=0){
        anterior = atual;
        atual = atual->prox;
    }
    if(atual){
        *flag = 1;
        if(atual==(*lista)){
            (*lista) = atual->prox;
        }
        else{
            anterior->prox = atual->prox;
        }
    free(atual->minutos);
    free(atual->titulo);
        free(atual);
    }
}

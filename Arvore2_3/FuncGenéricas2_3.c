#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int libera_albuns(NoRB* album){
    if(album){
        if(libera_albuns(album->esq)){
            libera_albuns(album->dir);
            libera_albuns(album->meio);
        }
        free(album->info1.album.nome);
        free(album->info2.album.nome);
        libera_musicas(album->info1.album.musicas);
        libera_musicas(album->info2.album.musicas);
        free(album);
    }
    return (album!=NULL);
}

NoRB* criar_no(void* dados,NoRB* FEsq, NoRB* FCen,TipoNo tipo,NoRB* pai){
    NoRB* novo = (NoRB*) malloc(sizeof(NoRB));
    novo->tipo = tipo;
    novo->pai = pai;
    novo->esq = FEsq;
    novo->meio = FCen;
    novo->dir = NULL;
    novo->ninfos = 1;
    if(tipo== NO_ARTISTA){
        DadosArtista* artista = (DadosArtista*) dados;
        novo->info1.artista.nome = strdup(artista->nome);
        novo->info1.artista.estilo = strdup(artista->estilo);
        novo->info1.artista.qtd_albuns = artista->qtd_albuns;
        novo->info1.artista.albuns = NULL;
    }
    else{
        DadosAlbum* album = (DadosAlbum*) dados;
        novo->info1.album.nome = strdup(album->nome);
        novo->info1.album.ano = album->ano;
        novo->info1.album.qtd_musicas = album->qtd_musicas;
        novo->info1.album.musicas = NULL;
    }
    return novo;
}

void copia_dados(void* recebe,void* envia,TipoNo tipo){
    if(recebe){
        if(tipo==NO_ARTISTA){
            DadosArtista* destino = (DadosArtista*) recebe;
            DadosArtista* origem = (DadosArtista*) envia;
            destino->nome = strdup(origem->nome);
            destino->estilo = strdup(origem->estilo);
            destino->qtd_albuns = origem->qtd_albuns;
            destino->albuns = NULL;
        }
        else{
            DadosAlbum* destino = (DadosAlbum*) recebe;
            DadosAlbum* origem = (DadosAlbum*) envia;
            destino->nome = strdup(origem->nome);
            destino->ano = origem->ano;
            destino->qtd_musicas = origem->qtd_musicas;
            destino->musicas = NULL;
        }
    }
}

void adiciona_info(NoRB** NO,void* info, NoRB* filho){
    char* nomeInfo = (((*NO)->tipo==NO_ARTISTA) ? ((DadosArtista*)info)->nome : ((DadosAlbum*)info)->nome);
    if(strcmp(nomeInfo, (*NO)->info1.album.nome) > 0){
        copia_dados(&(*NO)->info2, info, (*NO)->tipo);
        (*NO)->dir = filho;
    }
    else {
        copia_dados(&(*NO)->info2, &(*NO)->info1, (*NO)->tipo);
        (*NO)->dir = (*NO)->meio;
        copia_dados(&(*NO)->info1, info, (*NO)->tipo);
        (*NO)->meio = filho;
    }
    (*NO)->ninfos = 2;
}

int eh_folha(NoRB* NO){
    return (NO->esq==NULL);
}

NoRB* quebrar_NO(NoRB** NO, void* info, NoRB* filho, void** sobe){
    char* nomeInfo = ((*NO)->tipo==NO_ARTISTA) ? ((DadosArtista*)info)->nome : ((DadosAlbum*)info)->nome;
    NoRB* maior = NULL;
    if ((*NO)->tipo == NO_ARTISTA)
        *sobe = malloc(sizeof(DadosArtista));
    else
        *sobe = malloc(sizeof(DadosAlbum));
    if(strcmp(nomeInfo, (*NO)->info2.album.nome) > 0){
        copia_dados(*sobe, &(*NO)->info2, (*NO)->tipo);
        (*NO)->ninfos = 1;
        maior = criar_no(info, filho, (*NO)->dir, (*NO)->tipo,(*NO));
        (*NO)->dir = NULL;
    }
    else if(strcmp(nomeInfo, (*NO)->info1.album.nome) > 0){
        copia_dados(*sobe, info, (*NO)->tipo);
        maior = criar_no(&(*NO)->info2, filho, (*NO)->dir, (*NO)->tipo,(*NO));
        (*NO)->dir = NULL;
        (*NO)->ninfos = 1;
    }
    else{
        copia_dados(*sobe, &(*NO)->info1, (*NO)->tipo);
        copia_dados(&(*NO)->info2, info, (*NO)->tipo);
        (*NO)->ninfos = 1;
        maior = criar_no(&(*NO)->info2, (*NO)->meio, (*NO)->dir, (*NO)->tipo,(*NO));
        (*NO)->meio = filho;
        (*NO)->dir = NULL;
    }
    return maior;
}

NoRB* inserirNo(NoRB **raiz, void *dados, TipoNo tipo, NoRB* pai, void* sobe, int* flag){
    NoRB* maior;
    char* nomeInfo = (tipo==NO_ARTISTA) ? ((DadosArtista*)dados)->nome : ((DadosAlbum*)dados)->nome;
    if(*raiz == NULL){
        *raiz = criar_no(dados,NULL,NULL,tipo,pai);
        *flag = 1;
    }
    else {
        if(eh_folha(*raiz)){
            if((*raiz)->ninfos == 1){
                adiciona_info(raiz,dados,NULL);
            } else {
                maior = quebrar_NO(raiz, dados, NULL, &sobe);
                if(pai == NULL){
                    *raiz = criar_no(sobe, (*raiz), maior, (*raiz)->tipo, pai);
                    maior = NULL;
                }
            }
        }
        else {
            if(strcmp(nomeInfo, (*raiz)->info1.album.nome) < 0){
                maior = inserirNo(&(*raiz)->esq,dados,(*raiz)->tipo,(*raiz),sobe,flag);
            }
            else {
                if((*raiz)->ninfos == 1 || strcmp(nomeInfo, (*raiz)->info2.album.nome) < 0){
                    maior = inserirNo(&(*raiz)->meio,dados,(*raiz)->tipo,(*raiz),sobe,flag);
                }
                else {
                    maior = inserirNo(&(*raiz)->dir,dados,(*raiz)->tipo,(*raiz),sobe,flag);
                }
            }
            if(maior!=NULL){
                if((*raiz)->ninfos==1){
                    adiciona_info(raiz, sobe, maior);
                    maior = NULL;
                }
                else {
                   maior = quebrar_NO(raiz, sobe, maior, &sobe);
                    if(pai==NULL){
                        (*raiz) = criar_no(sobe, (*raiz), maior, (*raiz)->tipo, pai);
                        maior = NULL;
                    }
                }
            }
        }
        *flag = 1;
    }
    return maior;
}

void exibe_dados(dado dado,TipoNo tipo){
    if(tipo==NO_ARTISTA){
       printf("NOME: %s\n",dado.artista.nome);
       printf("ESTILO: %s\n",dado.artista.estilo);
       printf("ALBUNS: %d\n",dado.artista.qtd_albuns);
    }
    else{
        printf("NOME: %s \n", dado.album.nome);
        printf("ANO: %d \n", dado.album.ano);
        printf("MUSICAS: %d\n",dado.album.qtd_musicas);
    }
}

void imprimirArvore(NoRB *raiz){
    if(raiz){
        imprimirArvore(raiz->esq);
        exibe_dados(raiz->info1,raiz->tipo);
        imprimirArvore(raiz->meio);
        if(raiz->ninfos==2){
            exibe_dados(raiz->info2,raiz->tipo);
            imprimirArvore(raiz->dir);
        }
    }
}

dado* buscar_item(NoRB* raiz, char* nome) {
    dado* resultado = NULL;
    if (raiz) {
        int cmp1 = strcmp(nome, raiz->info1.album.nome);
        if (cmp1 < 0) {
            resultado = buscar_item(raiz->esq, nome);
        }
        else if (cmp1 == 0) {
            resultado = &raiz->info1;
        }
        else {
            if (raiz->ninfos == 1) {
                resultado = buscar_item(raiz->dir, nome);
            } 
            else {
                int cmp2 = strcmp(nome, raiz->info2.album.nome);
                if (cmp2 < 0) {
                    resultado = buscar_item(raiz->meio, nome);
                }
                else if (cmp2 == 0) {
                    resultado = &raiz->info2;
                }
                else {
                    resultado = buscar_item(raiz->dir, nome);
                }
            }
        }
    }
    return resultado;
}
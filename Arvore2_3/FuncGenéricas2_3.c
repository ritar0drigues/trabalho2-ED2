#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Compare two key strings. If both are pure integers, compare numerically.
   Otherwise fall back to strcmp. Returns <0 if a<b, 0 if equal, >0 if a>b. */
static int cmp_keys(const char *a, const char *b){
    if(a==b) return 0;
    if(!a) return -1;
    if(!b) return 1;

    char *end1, *end2;
    long v1 = strtol(a, &end1, 10);
    long v2 = strtol(b, &end2, 10);
    if(*end1 == '\0' && *end2 == '\0'){
        if(v1 < v2) return -1;
        if(v1 > v2) return 1;
        return 0;
    }
    return strcmp(a,b);
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
        /* inicializa info2 para evitar ponteiros não inicializados */
        novo->info2.artista.nome = NULL;
        novo->info2.artista.estilo = NULL;
        novo->info2.artista.qtd_albuns = 0;
        novo->info2.artista.albuns = NULL;
    }
    else{
        DadosAlbum* album = (DadosAlbum*) dados;
        novo->info1.album.nome = strdup(album->nome);
        novo->info1.album.ano = album->ano;
        novo->info1.album.qtd_musicas = album->qtd_musicas;
        novo->info1.album.musicas = NULL;
        /* inicializa info2 para evitar ponteiros não inicializados */
        novo->info2.album.nome = NULL;
        novo->info2.album.ano = 0;
        novo->info2.album.qtd_musicas = 0;
        novo->info2.album.musicas = NULL;
    }
    return novo;
}

void copia_dados(void* recebe,void* envia,TipoNo tipo){
    if(recebe){
        if(tipo==NO_ARTISTA){
            DadosArtista* destino = (DadosArtista*) recebe;
            DadosArtista* origem = (DadosArtista*) envia;
            /* libera conteúdo antigo, se houver */
            if(destino->nome) free(destino->nome);
            if(destino->estilo) free(destino->estilo);
            destino->nome = strdup(origem->nome);
            destino->estilo = strdup(origem->estilo);
            destino->qtd_albuns = origem->qtd_albuns;
            destino->albuns = NULL;
        }
        else{
            DadosAlbum* destino = (DadosAlbum*) recebe;
            DadosAlbum* origem = (DadosAlbum*) envia;
            if(destino->nome) free(destino->nome);
            destino->nome = strdup(origem->nome);
            destino->ano = origem->ano;
            destino->qtd_musicas = origem->qtd_musicas;
            destino->musicas = NULL;
        }
    }
}

void adiciona_info(NoRB** NO,void* info, NoRB* filho){
    char* nomeInfo;
    if((*NO)->tipo==NO_ARTISTA){
        DadosArtista* informa = (DadosArtista*)info;
        if(informa)
            nomeInfo = informa->nome;
    }
    else{
        DadosAlbum* informa = (DadosAlbum*)info;
        if(informa)
            nomeInfo = informa->nome;
        
    }
    char* nome1 = (((*NO)->tipo==NO_ARTISTA) ? (*NO)->info1.artista.nome : (*NO)->info1.album.nome);
    if(cmp_keys(nomeInfo, nome1) > 0){
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
    if(!NO || !(*NO) || !info) return NULL;

    TipoNo tipo = (*NO)->tipo;
    char* nomeInfo = (tipo==NO_ARTISTA) ? ((DadosArtista*)info)->nome : ((DadosAlbum*)info)->nome;
    NoRB* maior = NULL;

    /* allocate and zero the structure that will be promoted */
    if (tipo == NO_ARTISTA)
        *sobe = calloc(1, sizeof(DadosArtista));
    else
        *sobe = calloc(1, sizeof(DadosAlbum));

    /* safe access to current keys */
    char *a_name = (tipo==NO_ARTISTA) ? (*NO)->info1.artista.nome : (*NO)->info1.album.nome;
    char *b_name = ((*NO)->ninfos==2) ? ((tipo==NO_ARTISTA) ? (*NO)->info2.artista.nome : (*NO)->info2.album.nome) : NULL;

    /* Case 1: new key is less than a_name -> promote a_name */
    if (a_name && cmp_keys(nomeInfo, a_name) < 0) {
        copia_dados(*sobe, &(*NO)->info1, tipo);        /* sobe = a */
        copia_dados(&(*NO)->info1, info, tipo);         /* info1 = new key */
        maior = criar_no(&(*NO)->info2, (*NO)->meio, (*NO)->dir, tipo, *NO); /* right sibling keeps b */
        (*NO)->ninfos = 1;
        (*NO)->meio = filho;
        (*NO)->dir = NULL;
    }
    /* Case 2: new key is greater than b_name -> promote b_name */
    else if (b_name && cmp_keys(nomeInfo, b_name) > 0) {
        copia_dados(*sobe, &(*NO)->info2, tipo);        /* sobe = b */
        maior = criar_no(info, filho, (*NO)->dir, tipo, *NO); /* right sibling gets new key */
        (*NO)->dir = NULL;
        (*NO)->ninfos = 1;
    }
    /* Case 3: new key is between a and b -> promote new key */
    else {
        copia_dados(*sobe, info, tipo);                 /* sobe = new key */
        maior = criar_no(&(*NO)->info2, filho, (*NO)->dir, tipo, *NO); /* right sibling keeps b */
        (*NO)->dir = NULL;
        (*NO)->ninfos = 1;
    }

    return maior;
}

NoRB* inserirNo(NoRB **raiz, void *dados, TipoNo tipo, NoRB* pai, void** sobe, int* flag){
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
                maior = quebrar_NO(raiz, dados, NULL, sobe);
                if(pai == NULL){
                    (*raiz) = criar_no(*sobe, (*raiz), maior, (*raiz)->tipo, pai);
                    (*raiz)->esq->pai = *raiz;
                    (*raiz)->meio->pai = *raiz;
                    maior = NULL;
                }
            }
        }
        else{
            /* determine correct names based on node type */
            char *r_info1 = ((*raiz)->tipo == NO_ARTISTA) ? (*raiz)->info1.artista.nome : (*raiz)->info1.album.nome;
            char *r_info2 = (((*raiz)->ninfos == 2) && ((*raiz)->tipo == NO_ARTISTA)) ? (*raiz)->info2.artista.nome : (((*raiz)->ninfos == 2) ? (*raiz)->info2.album.nome : NULL);

            if(r_info1 && cmp_keys(nomeInfo, r_info1) < 0){
                maior = inserirNo(&(*raiz)->esq,dados,(*raiz)->tipo,(*raiz),sobe,flag);
            }
            else {
                if((*raiz)->ninfos == 1 || (r_info2 && cmp_keys(nomeInfo, r_info2) < 0)){
                    maior = inserirNo(&(*raiz)->meio,dados,(*raiz)->tipo,(*raiz),sobe,flag);
                }
                else {
                    maior = inserirNo(&(*raiz)->dir,dados,(*raiz)->tipo,(*raiz),sobe,flag);
                }
            }
            if(maior!=NULL){
                if((*raiz)->ninfos==1){
                    adiciona_info(raiz, *sobe, maior);
                    maior = NULL;
                }
                else{
                   maior = quebrar_NO(raiz, *sobe, maior, sobe);
                    if(pai==NULL){
                        (*raiz) = criar_no(*sobe, (*raiz), maior, (*raiz)->tipo, pai);
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
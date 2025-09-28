#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void rotacaoDireita(NoRB** raiz){
    int cor = (*raiz)->cor;
    NoRB* aux = (*raiz)->esq;
    aux->cor = cor;
    (*raiz)->esq = aux->dir;
    if (aux->dir) aux->dir->pai = *raiz;
    aux->dir = (*raiz);
    aux->dir->pai = aux;
    (*raiz)->cor = VERMELHO;
    (*raiz) = aux;
}

void rotacaoEsquerda(NoRB** raiz){
    int cor = (*raiz)->cor;
    NoRB* aux = (*raiz)->dir;
    aux->cor = cor;
    (*raiz)->dir = aux->esq;

    if (aux->esq){
        aux->esq->pai = *raiz;
    }

    aux->esq = (*raiz);
    aux->esq->pai = aux;
    (*raiz)->cor = VERMELHO;
    (*raiz) = aux;
}

NoRB *criarNo(void *dado, TipoNo tipo, NoRB* pai){
    NoRB* no = (NoRB*) malloc(sizeof(NoRB));
    no->tipo = tipo;
    if (tipo == NO_ARTISTA) {
        DadosArtista* artista = (DadosArtista*) dado;
        no->dado.artista.nome = strdup(artista->nome);
        no->dado.artista.estilo = strdup(artista->estilo);
        no->dado.artista.qtd_albuns = artista->qtd_albuns;
        no->dado.artista.albuns = NULL;
    }
    else if (tipo == NO_ALBUM) {
        DadosAlbum* album = (DadosAlbum*)dado;
        no->dado.album.titulo = strdup(album->titulo);
        no->dado.album.ano = album->ano;
        no->dado.album.qtd_musicas = album->qtd_musicas;
        no->dado.album.musicas = NULL;
    }
    no->cor = VERMELHO;
    no->esq = no->dir = NULL;
    no->pai = pai;
    return no;
}

int eh_folha(NoRB* NO){
    return (NO->esq==NULL && NO->dir==NULL);
}
void Verificaseroda_paraesq(NoRB **no){
    if((*no)->dir && (*no)->dir->cor == VERMELHO){
        if((*no)->esq == NULL || (*no)->esq->cor == PRETO){
            rotacaoEsquerda(no);
        }
    }
}

void troca_cor(NoRB* no){
    if(no->esq) no->esq->cor = PRETO;
    if(no->dir) no->dir->cor = PRETO;
    if(no->cor==PRETO){
        no->cor = VERMELHO;
    }
    else{
        no->cor = PRETO;
    }
}

void balancearRubroNegra(NoRB** no){
    if(*no){
        if(!eh_folha(*no)){
            if((*no)->dir!=NULL){
                Verificaseroda_paraesq(no);
            }
            if((*no)->esq!=NULL && (*no)->esq->esq!=NULL){
                if((*no)->esq->cor == VERMELHO && (*no)->esq->esq->cor == VERMELHO){
                    rotacaoDireita(no);
                }
            }
            if((*no)->esq!=NULL && (*no)->dir !=NULL){
                if((*no)->esq->cor == VERMELHO && (*no)->dir->cor == VERMELHO){
                    troca_cor((*no));
                }
            }  
        }
    }
}

void inserirNo(NoRB **raiz, void *dado, TipoNo tipo, NoRB* pai,int* flag){
    if((*raiz)){
        int cmp = 0;
        if (tipo == NO_ARTISTA) {
            DadosArtista* artista = (DadosArtista*)dado;
            cmp = strcasecmp((*raiz)->dado.artista.nome, artista->nome);
        } else if (tipo == NO_ALBUM) {
            DadosAlbum* album = (DadosAlbum*)dado;
            cmp = strcasecmp((*raiz)->dado.album.titulo, album->titulo);
        }
        if(cmp>0){
            inserirNo(&(*raiz)->esq, dado, tipo, (*raiz),flag);
        }
        else if(cmp<0){
            inserirNo(&(*raiz)->dir, dado, tipo, (*raiz),flag);   
        }
        else{
            printf("Elemento já cadastrado.\n");
        }
        balancearRubroNegra(raiz);
    }
    else{
        NoRB *novo = criarNo(dado, tipo, pai);
        (*raiz) = novo;
        *flag = 1;
    }
}

void imprimirArvore(NoRB *raiz){
    if(raiz){
        imprimirArvore(raiz->esq);
        if (raiz->tipo == NO_ARTISTA){
            printf("Artista: %s | Estilo: %s\n", raiz->dado.artista.nome, raiz->dado.artista.estilo);
        } 
        else if(raiz->tipo == NO_ALBUM){
            printf("Álbum: %s | Ano: %d\n", raiz->dado.album.titulo, raiz->dado.album.ano);
        }
        imprimirArvore(raiz->dir);
    }
}

NoRB* buscar_item(NoRB* raiz, char* nome){
    NoRB* resultado = NULL;
    if(raiz){
        int cmp;
        if(raiz->tipo==NO_ARTISTA){
            cmp = strcmp(raiz->dado.artista.nome,nome);
        }
        else{
            cmp = strcmp(raiz->dado.artista.nome,nome);
        }
        if(cmp>0){
            resultado = buscar_item(raiz->dir, nome);
        }
        if(cmp<0){
            resultado = buscar_item(raiz->esq,nome);
        }
        else{
            resultado = raiz;
        }
    }
    return resultado;
}

void exibe_dados(NoRB* no){
    if(no){
        if(no->tipo==NO_ARTISTA){
           printf("NOME: %s\n",no->dado.artista.nome);
           printf("ESTILO: %s\n",no->dado.artista.estilo);
           printf("ALBUNS: %d\n", no->dado.artista.qtd_albuns);
        }
        else{
            printf("NOME: %s \n", no->dado.album.titulo);
            printf("ANO: %d \n", no->dado.album.ano);
            printf("MUSICAS: %d\n",no->dado.album.qtd_musicas);
        }
    }
}

int remove_ArvRB(NoRB **raiz,char *nome){
    if(buscar_item(raiz,nome)){
        *raiz = remove_NO((*raiz),nome);
        if(raiz!=NULL){
            (*raiz)->cor = PRETO;
        }
        return 1;
    }
    else{
        return 0;
    }
}
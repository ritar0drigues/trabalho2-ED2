#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Rotação à direita
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

// Rotação à esquerda
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

// Cria um novo nó (duplica strings internas)
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
        /* Ajuste: campo é 'titulo' em DadosAlbum */
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


// Inverte cores dos filhos e do nó atual (usado no balanceamento)
void troca_cor(NoRB* no) {
       if (no) {
        if (no->esq) no->esq->cor = PRETO;
        if (no->dir) no->dir->cor = PRETO;
        
        if (no->cor == PRETO)
            no->cor = VERMELHO;
        else
            no->cor = PRETO;
    }
}

int cor(NoRB* no) {
    int cor_no = PRETO;
    if (no) cor_no = no->cor;
    return cor_no;
}

// Balanceamento utilizado após inserção
void balancearRubroNegra(NoRB** no){
    if(*no){
        if(!eh_folha(*no)){
            if(cor((*no)->dir) == VERMELHO && cor((*no)->esq) == PRETO){
                rotacaoEsquerda(no);
            }
            if((*no)->esq!=NULL){
                if(cor((*no)->esq) == VERMELHO && cor((*no)->esq->esq) == VERMELHO){
                    rotacaoDireita(no);
                }
            }            
            if(cor((*no)->esq) == VERMELHO && cor((*no)->dir) == VERMELHO){
                troca_cor((*no));
            }
        }
    }
}

// Função exposta (prototipada em tad.h) — chama o balanceamento de inserção
void balancearremocao(NoRB** no){
    if(*no){
        if(!eh_folha(*no)){
            if(cor((*no)->dir) == VERMELHO){
                rotacaoEsquerda(no);
            }
            if((*no)->esq!=NULL){
                if(cor((*no)->esq) == VERMELHO && cor((*no)->esq->esq) == VERMELHO){
                    rotacaoDireita(no);
                }
            }            
            if(cor((*no)->esq) == VERMELHO && cor((*no)->dir) == VERMELHO){
                troca_cor((*no));
            }
        }
    }
}

// Insere nó (usa strcmp assumindo que estamos inserindo por nome/título)
void inserirNo(NoRB **raiz, void *dado, TipoNo tipo, NoRB* pai){
    if((*raiz)){
        int cmp = 0;
        if (tipo == NO_ARTISTA) {
            DadosArtista* artista = (DadosArtista*)dado;
            cmp = strcmp((*raiz)->dado.artista.nome, artista->nome);
        } else if (tipo == NO_ALBUM) {
            DadosAlbum* album = (DadosAlbum*)dado;
            cmp = strcmp((*raiz)->dado.album.titulo, album->titulo);
        }
        if(cmp>0){
            inserirNo(&(*raiz)->esq, dado, tipo, (*raiz));
        }
        else if(cmp<0){
            inserirNo(&(*raiz)->dir, dado, tipo, (*raiz));   
        }
        else{
            printf("Elemento já cadastrado.\n");
        }
        balancearRubroNegra(raiz);
    }
    else{
        NoRB *novo = criarNo(dado, tipo, pai);
        (*raiz) = novo;
    }
}

// Impressão em-ordem
void imprimirArvore(NoRB *raiz){
    if(raiz){
        imprimirArvore(raiz->esq);
        //printf("cor=%d\n", raiz->cor);
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
        cmp = strcmp(raiz->dado.artista.nome,nome);
        if(cmp>0){
            resultado = buscar_item(raiz->esq, nome);
        }
        else if(cmp<0){
            resultado = buscar_item(raiz->dir,nome);
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
            printf("TITULO: %s \n", no->dado.album.titulo);
            printf("ANO: %d \n", no->dado.album.ano);
            printf("MUSICAS: %d\n",no->dado.album.qtd_musicas);
        }
    }
}


void move2esqRED(NoRB** h) {
    troca_cor(*h);
    if((*h)->dir){
        if (cor((*h)->dir->esq) == VERMELHO) {
            rotacaoDireita(&(*h)->dir);
            rotacaoEsquerda(h);
            troca_cor(*h);
        }
    }
}

void move2dirRED(NoRB** h) {
    troca_cor(*h);
    if((*h)->esq){
        if (cor((*h)->esq->esq) == VERMELHO) {
            rotacaoDireita(h);
            troca_cor(*h);
        }
    }
}

int remove_ArvRB(NoRB **raiz,char *nome){
    int removeu = 0;
    NoRB* remover = buscar_item((*raiz),nome);
    if(remover){
        remove_NO(raiz,nome);
        if((*raiz)!=NULL){
            (*raiz)->cor = PRETO;
        }
        removeu =1;
    }
    return removeu;
}

void copiar_dados(NoRB* destino,NoRB* origem){
    if((destino && origem) && (destino->tipo==origem->tipo)){
        if(destino->tipo  == NO_ARTISTA){
            free(destino->dado.artista.estilo);
            free(destino->dado.artista.nome);
            destino->dado.artista.albuns = origem->dado.artista.albuns;
            destino->dado.artista.estilo = strdup(origem->dado.artista.estilo);
            destino->dado.artista.nome = strdup(origem->dado.artista.nome);
            destino->dado.artista.qtd_albuns = origem->dado.artista.qtd_albuns;
        }
        else{
            free(destino->dado.album.titulo);
            destino->dado.album.musicas = origem->dado.album.musicas;
            destino->dado.album.ano = origem->dado.album.ano;
            destino->dado.album.titulo = strdup(origem->dado.album.titulo);
            destino->dado.album.qtd_musicas = origem->dado.album.qtd_musicas;
        }
    }
}

void Procura_menor(NoRB* no, NoRB** menor){
    if(no){
        (*menor) = no;
        if(no->esq){
            Procura_menor(no->esq,menor);
        }
    }
}

void libera_conteudo_no(NoRB* n) {
    if (n){
        if (n->tipo == NO_ARTISTA) {
            free(n->dado.artista.nome);
            free(n->dado.artista.estilo);
        } 
        else {
            free(n->dado.album.titulo);
        }
    }
}

void remover_Menor_Direita(NoRB** NO) {
    if((*NO)->dir == NULL){
        libera_conteudo_no(*NO);
        free(*NO);
        *NO = NULL;
    }
    else{
        if(cor((*NO)->dir) == PRETO && cor((*NO)->dir->esq) == PRETO){
            move2esqRED(NO);
        }
        remover_Menor_Direita(&(*NO)->esq);
        balancearRubroNegra(NO);
    }
}


void remove_NO(NoRB** raiz, char* nome){
    if(*raiz){
        if(strcmp(nome,(*raiz)->dado.artista.nome)<0){
            if((*raiz)->esq){
                if((cor((*raiz)->esq) == PRETO) && cor((*raiz)->esq->esq)==PRETO){
                    move2esqRED(raiz);
                }
            }
            remove_NO(&(*raiz)->esq,nome);
        }
        else{
            if(cor((*raiz)->esq)==VERMELHO){
                rotacaoDireita(raiz);
            }
            if(strcmp(nome,(*raiz)->dado.artista.nome)==0 && (*raiz)->dir == NULL){
                free((*raiz)->dado.artista.nome);
                if((*raiz)->tipo == NO_ARTISTA){
                    free((*raiz)->dado.artista.estilo);
                    libera_albuns((*raiz)->dado.artista.albuns);
                }
                else{
                    libera_musicas((*raiz)->dado.album.musicas);
                }
                free(*raiz);
                *raiz = NULL;
            }
            else{
                if(cor((*raiz)->dir)==PRETO && cor((*raiz)->dir->esq)==PRETO){
                    move2dirRED(raiz);
                }
                if(strcmp(nome,(*raiz)->dado.artista.nome)==0){
                    NoRB* menor = NULL;
                    Procura_menor((*raiz)->dir,&menor);
                    copiar_dados((*raiz),menor);
                    remover_Menor_Direita(&(*raiz)->dir);
                }
                else{
                    remove_NO(&(*raiz)->dir,nome);
                }
            }
        }
        balancearremocao(raiz);
    }     
}
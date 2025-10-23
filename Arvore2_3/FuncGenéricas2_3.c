#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* copia os campos relevantes de 'envia' para 'recebe' conforme o tipo */
void copia_dados(dado *recebe, dado *envia, TipoNo tipo)
{
    if (recebe && envia){
        if (tipo == NO_ARTISTA)
        {
            strncpy(recebe->artista.nome, envia->artista.nome, 49);
            recebe->artista.nome[49] = '\0';
            strncpy(recebe->artista.estilo, envia->artista.estilo, 49);
            recebe->artista.estilo[49] = '\0';
            recebe->artista.qtd_albuns = envia->artista.qtd_albuns;
            recebe->artista.albuns = envia->artista.albuns;
        }
        else
        {
            strncpy(recebe->album.nome, envia->album.nome, 49);
            recebe->album.nome[49] = '\0';
            recebe->album.ano = envia->album.ano;
            recebe->album.qtd_musicas = envia->album.qtd_musicas;
            recebe->album.musicas = envia->album.musicas;
        }
    }
}

/* cria um nó com cópia dos campos (não aloca strings dinamicamente) */
NoRB *criar_no(dado *dados, NoRB *FEsq, NoRB *FCen, TipoNo tipo)
{
    NoRB *novo = (NoRB *)malloc(sizeof(NoRB));
    novo->tipo = tipo;
    novo->esq = FEsq;
    novo->meio = FCen;
    novo->dir = NULL;
    novo->ninfos = 1;

    if (dados)
        copia_dados(&novo->info1, dados, tipo);
        if (tipo == NO_ARTISTA) {
            novo->info2.artista.nome[0] = '\0';
            novo->info2.artista.estilo[0] = '\0';
            novo->info2.artista.qtd_albuns = 0;
            novo->info2.artista.albuns = NULL;
        }
        else{
            novo->info2.album.nome[0] = '\0';
            novo->info2.album.ano = 0;
            novo->info2.album.qtd_musicas = 0;
            novo->info2.album.musicas = NULL;
        }

    return novo;
}

int eh_folha(NoRB *NO)
{
    return (NO->esq == NULL && NO->meio == NULL && NO->dir == NULL);
}

/* adiciona 'info' em NO que tem 1 info (passando filho referente ao lado direito quando necessário) */
void adiciona_info(NoRB **NO, dado *info, NoRB *filho)
{
    char *nomeInfo =  info->artista.nome;
    char *nome1 = (*NO)->info1.artista.nome;
    if (cmp_keys(nomeInfo, nome1) > 0)
    {
        copia_dados(&(*NO)->info2, info, (*NO)->tipo);
        (*NO)->dir = filho;
    }
    else
    {
        copia_dados(&(*NO)->info2, &(*NO)->info1, (*NO)->tipo);
        (*NO)->dir = (*NO)->meio;
        copia_dados(&(*NO)->info1, info, (*NO)->tipo);
        (*NO)->meio = filho;
    }
    (*NO)->ninfos = 2;
}

/* quebra um nó com 2 infos inserindo 'info' (ou promovendo *sobe); retorna novo irmão direito (maior) e preenche *sobe com dado promovido.
   filho é o ponteiro direito correspondente à info passada (pode ser NULL para folhas). */
NoRB *quebrar_NO(NoRB **NO, dado *info, NoRB *filho, dado **sobe)
{
    NoRB *maior = NULL;
    if ((*NO)){

        TipoNo tipo = (*NO)->tipo;
        char *a_name = (*NO)->info1.artista.nome;
        char *b_name =  (*NO)->info2.artista.nome;
        char *nomeInfo = info->artista.nome;

        *sobe = (dado *)malloc(sizeof(dado));
        if (cmp_keys(nomeInfo, b_name) > 0)
        {
            copia_dados(*sobe, &(*NO)->info2, tipo);
            (*NO)->ninfos = 1;
            maior = criar_no(info, (*NO)->dir, filho, tipo);            
            (*NO)->dir = NULL;
        }
        else if (cmp_keys(nomeInfo, a_name) > 0)
        {
            copia_dados(*sobe, &(*NO)->info1, tipo);
            maior = criar_no(&(*NO)->info2, filho, (*NO)->dir, tipo);
            (*NO)->dir = NULL;
            (*NO)->ninfos = 1;
            
        }
        else
        {
            copia_dados(*sobe, &(*NO)->info1, tipo);
            copia_dados(&(*NO)->info1, info, tipo);
            (*NO)->ninfos = 1;
            maior = criar_no(&(*NO)->info2, (*NO)->dir, (*NO)->dir, tipo);
            (*NO)->meio = filho;
            (*NO)->dir = NULL;
            (*NO)->ninfos = 1;
        }

    }
    return maior;
}

/* função de inserção recursiva: retorna irmão direito criado (maior) se o nó atual foi quebrado e precisa propagar, caso contrário retorna NULL.
   Se um split sobe para uma raiz (pai == NULL), esta função transforma a raiz em novo nó contendo *sobe e filhos old + maior. */
NoRB *inserirNo(NoRB **raiz, dado *dados, TipoNo tipo, NoRB *pai, dado **sobe, int *flag)
{
    NoRB *maior = NULL;
    if (*raiz == NULL)
    {
        *raiz = criar_no(dados, NULL, NULL, tipo);
        if (*raiz) *flag = 1;
    }
    else{
        if (eh_folha(*raiz))
        {
            if ((*raiz)->ninfos == 1)
            {
                adiciona_info(raiz, dados, NULL);
                *flag = 1;
            }
            else
            {
                /* nó folha com 2 infos -> quebrar */
                maior = quebrar_NO(raiz, dados, NULL, sobe);
                /* se raiz (pai == NULL) e houve quebra, criar nova raiz aqui */
                if (pai == NULL)
                {
                    *raiz = criar_no(*sobe, (*raiz), maior, (*raiz)->tipo);
                    maior = NULL;
                }
                *flag = 1;
            }
        }else {
           char *nomeInfo;
            if (tipo == NO_ARTISTA)
            {
                nomeInfo = dados->artista.nome;
            } else {
                nomeInfo = dados->album.nome;
           }

            char *r_info1;
            if ((*raiz)->tipo == NO_ARTISTA)
            {
                r_info1 = (*raiz)->info1.artista.nome;
            } else {
                r_info1 = (*raiz)->info1.album.nome;
            }

            char *r_info2 = NULL;
            if ((*raiz)->ninfos == 2) {
                if ((*raiz)->tipo == NO_ARTISTA)
                {
                    r_info2 = (*raiz)->info2.artista.nome;
                }
                else
                {
                    r_info2 = (*raiz)->info2.album.nome;
                }
            }
            if (cmp_keys(nomeInfo, r_info1) < 0)
            {
                maior = inserirNo(&(*raiz)->esq, dados, tipo, *raiz, sobe, flag);
            }
            else{
                if ((*raiz)->ninfos == 1 || cmp_keys(nomeInfo, r_info2) < 0)
                {
                    maior = inserirNo(&(*raiz)->meio, dados, tipo, *raiz, sobe, flag);
                }
                else
                {
                    maior = inserirNo(&(*raiz)->dir, dados, tipo, *raiz, sobe, flag);
                }
            }
            if (maior != NULL)
            {
                /* houve split no filho e *sobe contém a chave a ser inserida aqui */
                if ((*raiz)->ninfos == 1)
                {
                    adiciona_info(raiz, *sobe, maior);
                    maior = NULL;
                }
                else
                {
                    /* este nó tem 2 infos -> quebrar com *sobe e maior */
                    maior = quebrar_NO(raiz, *sobe, maior, sobe);
                    /* se for raiz, promover criando nova raiz */
                    if (pai == NULL)
                    {
                        *raiz = criar_no(*sobe, (*raiz), maior, (*raiz)->tipo);
                        maior = NULL;
                    }
                }
            }
        }
    }
    *flag = 1;
    return maior;
}

void exibe_dados(dado dado, TipoNo tipo)
{
    if (tipo == NO_ARTISTA)
    {
        printf("NOME: %s\n", dado.artista.nome);
        printf("ESTILO: %s\n", dado.artista.estilo);
        printf("ALBUNS: %d\n", dado.artista.qtd_albuns);
    }
    else
    {
        printf("NOME: %s \n", dado.album.nome);
        printf("ANO: %d \n", dado.album.ano);
        printf("MUSICAS: %d\n", dado.album.qtd_musicas);
    }
}

void imprimirArvore(NoRB *raiz)
{
    if (raiz)
    {
        imprimirArvore(raiz->esq);
        exibe_dados(raiz->info1, raiz->tipo);
        imprimirArvore(raiz->meio);
        if (raiz->ninfos == 2)
        {
            exibe_dados(raiz->info2, raiz->tipo);
            imprimirArvore(raiz->dir);
        }
    }
}

dado* buscar_item(NoRB* raiz, char* nome) {
    dado *resultado = NULL;
    if (raiz && nome){

        /* obtém chaves do nó conforme tipo */
        char *k1;
        if(raiz->tipo == NO_ARTISTA){
            k1 = raiz->info1.artista.nome;
        } else {
            k1 = raiz->info1.album.nome;
        }

        char *k2 = NULL;
        if (raiz->ninfos == 2) {
            if(raiz->tipo == NO_ARTISTA){
                k2 = raiz->info2.artista.nome;
            } else {
                k2 = raiz->info2.album.nome;
            }
        }
 /* verifica igualdade no próprio nó primeiro */
        if (k1 && cmp_keys(nome, k1) == 0)
        {
            resultado = &raiz->info1;
        }
        else if (k2 && cmp_keys(nome, k2) == 0)
        {
            resultado = &raiz->info2;
        }
        else
        {
            /* decide em qual ramo descer usando cmp_keys */
            if (raiz->ninfos == 1)
            {
                if (k1 && cmp_keys(nome, k1) < 0)
                {
                    resultado = buscar_item(raiz->esq, nome);
                }
                else
                {
                    resultado = buscar_item(raiz->meio, nome);
                }
            }
            else
            {
                if (k1 && cmp_keys(nome, k1) < 0)
                {
                    resultado = buscar_item(raiz->esq, nome);
                }
                else if (k2 && cmp_keys(nome, k2) < 0)
                {
                    resultado = buscar_item(raiz->meio, nome);
                }
                else
                {
                    resultado = buscar_item(raiz->dir, nome);
                }
            }
        }
    }

    return resultado;
}
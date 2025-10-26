#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// compara strings que começam com números numericamente
int compare_numeric_prefix(const char *a, const char *b) {
    int num_a = 0, num_b = 0; 
    // lê o número do começo da string
    while (isdigit(*a)) {
        num_a = num_a * 10 + (*a - '0');
        a++;
    }
    while (isdigit(*b)) {
        num_b = num_b * 10 + (*b - '0');
        b++;
    }

    // compara os números
    if (num_a != num_b)
        return num_a - num_b;

    // se os números forem iguais, compara o restante da string lexicograficamente
    return strcmp(a, b);
}

/* copia os campos relevantes de 'envia' para 'recebe' conforme o tipo */
void copia_dados(dado *recebe, dado envia, TipoNo tipo){
    if (recebe){
        if (tipo == NO_ARTISTA){
            strncpy(recebe->artista.nome, envia.artista.nome, 49);
            recebe->artista.nome[49] = '\0';
            strncpy(recebe->artista.estilo, envia.artista.estilo, 49);
            recebe->artista.estilo[49] = '\0';
            recebe->artista.qtd_albuns = envia.artista.qtd_albuns;
            recebe->artista.albuns = envia.artista.albuns;
        }
        else{
            strncpy(recebe->album.nome, envia.album.nome, 49);
            recebe->album.nome[49] = '\0';
            recebe->album.ano = envia.album.ano;
            recebe->album.qtd_musicas = envia.album.qtd_musicas;
            recebe->album.musicas = envia.album.musicas;
        }
    }
}

/* cria um nó com cópia dos campos (não aloca strings dinamicamente) */
NoRB *criar_no(dado *dados, NoRB *FEsq, NoRB *FCen, TipoNo tipo){
    NoRB *novo = (NoRB *)malloc(sizeof(NoRB));
    novo->tipo = tipo;
    novo->esq = FEsq;
    novo->meio = FCen;
    novo->dir = NULL;
    novo->ninfos = 1;
    if(novo){
        if (dados) {
        copia_dados(&novo->info1, *dados, tipo);
        }
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
    }
    return novo;
}

int eh_folha(NoRB *NO)
{
    return (NO->esq == NULL);
}

/* adiciona 'info' em NO que tem 1 info (passando filho referente ao lado direito quando necessário) */
void adiciona_info(NoRB **NO, dado *info, NoRB *filho){
    char *nomeInfo =  info->artista.nome;
    char *nome1 = (*NO)->info1.artista.nome;
    if (compare_numeric_prefix(nomeInfo, nome1) > 0){
        copia_dados(&(*NO)->info2, *info, (*NO)->tipo);
        (*NO)->dir = filho;
    }   
    else{
        copia_dados(&(*NO)->info2, (*NO)->info1, (*NO)->tipo);
        (*NO)->dir = (*NO)->meio;
        copia_dados(&(*NO)->info1, *info, (*NO)->tipo);
        (*NO)->meio = filho;
    }
    (*NO)->ninfos = 2;
}

/* quebra um nó com 2 infos inserindo 'info' (ou promovendo *sobe); retorna novo irmão direito (maior) e preenche *sobe com dado promovido.
   filho é o ponteiro direito correspondente à info passada (pode ser NULL para folhas). */
NoRB *quebrar_NO(NoRB **NO, dado *info, NoRB *filho, dado **sobe){
    NoRB *maior = NULL;
    if ((*NO)){
        TipoNo tipo = (*NO)->tipo;
        char *a_name = (*NO)->info1.artista.nome;
        char *b_name =  (*NO)->info2.artista.nome;
        char *nomeInfo = info->artista.nome;

        *sobe = (dado *)malloc(sizeof(dado));
        if (compare_numeric_prefix(nomeInfo, b_name) > 0){
            copia_dados(*sobe,(*NO)->info2, tipo);
            (*NO)->ninfos = 1;
            maior = criar_no(info,(*NO)->dir, filho, tipo);            
            (*NO)->dir = NULL;
        }
        else if (compare_numeric_prefix(nomeInfo, a_name) > 0){
            // sobe é o valor do meio, que é 'info'
            copia_dados(*sobe, *info, tipo);
            maior = criar_no(&(*NO)->info2, filho, (*NO)->dir, tipo);
            (*NO)->dir = NULL;
            (*NO)->ninfos = 1;
        }

        else {
        copia_dados(*sobe, (*NO)->info1, tipo);  // sobe o menor
        copia_dados(&(*NO)->info1, *info, tipo); // novo menor substitui info1
        maior = criar_no(&(*NO)->info2, (*NO)->meio, (*NO)->dir, tipo);
        (*NO)->meio = filho;
        (*NO)->dir = NULL;
        (*NO)->ninfos = 1;
    }


    }
    return maior;
}

/* função de inserção recursiva: retorna irmão direito criado (maior) se o nó atual foi quebrado e precisa propagar, caso contrário retorna NULL.
   Se um split sobe para uma raiz (pai == NULL), esta função transforma a raiz em novo nó contendo *sobe e filhos old + maior. */
NoRB *inserirNo(NoRB **raiz, dado *dados, TipoNo tipo, NoRB *pai, dado **sobe, int *flag){
    NoRB *maior = NULL;
    if (*raiz == NULL){
        *raiz = criar_no(dados, NULL, NULL, tipo);
        if (*raiz){
            *flag = 1;
        }
    }
    else{
        if (eh_folha(*raiz)){
            if ((*raiz)->ninfos == 1){
                adiciona_info(raiz, dados, NULL);
                *flag = 1;
            }
            else{
                /* nó folha com 2 infos -> quebrar */
                maior = quebrar_NO(raiz, dados, NULL, sobe);
                /* se raiz (pai == NULL) e houve quebra, criar nova raiz aqui */
                if (pai == NULL){
                    *raiz = criar_no(*sobe, (*raiz), maior, (*raiz)->tipo);
                    maior = NULL;
                }
                *flag = 1;
            }
        }
        else{
            /* decide em qual filho descer */
            char *nomeInfo = (tipo == NO_ARTISTA) ? dados->artista.nome : dados->album.nome;
            char *r_info1 = ( (*raiz)->tipo == NO_ARTISTA ) ? (*raiz)->info1.artista.nome : (*raiz)->info1.album.nome;
            char *r_info2 = ( (*raiz)->ninfos == 2 ) ? ( ( (*raiz)->tipo == NO_ARTISTA ) ? (*raiz)->info2.artista.nome : (*raiz)->info2.album.nome ) : NULL;
            if (compare_numeric_prefix(nomeInfo, r_info1) < 0){
                maior = inserirNo(&(*raiz)->esq, dados, tipo, *raiz, sobe, flag);
            }
            else{
                if ((*raiz)->ninfos == 1 || compare_numeric_prefix(nomeInfo, r_info2) < 0){
                    maior = inserirNo(&(*raiz)->meio, dados, tipo, *raiz, sobe, flag);
                }
                else{
                    maior = inserirNo(&(*raiz)->dir, dados, tipo, *raiz, sobe, flag);
                }
            }
            if (maior != NULL){
                /* houve split no filho e *sobe contém a chave a ser inserida aqui */
                if ((*raiz)->ninfos == 1){
                    adiciona_info(raiz, *sobe, maior);
                    maior = NULL;
                }
                else{
                    /* este nó tem 2 infos -> quebrar com *sobe e maior */
                    maior = quebrar_NO(raiz, *sobe, maior, sobe);
                    /* se for raiz, promover criando nova raiz */
                    if (pai == NULL){
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

void exibe_dados(dado dado, TipoNo tipo){
    if (tipo == NO_ARTISTA){
        printf("NOME: %s\n", dado.artista.nome);
        printf("ESTILO: %s\n", dado.artista.estilo);
        printf("ALBUNS: %d\n", dado.artista.qtd_albuns);
    }
    else{
        printf("NOME: %s \n", dado.album.nome);
        printf("ANO: %d \n", dado.album.ano);
        printf("MUSICAS: %d\n", dado.album.qtd_musicas);
    }
}

void imprimirArvore(NoRB *raiz){
    if (raiz){
        imprimirArvore(raiz->esq);
        exibe_dados(raiz->info1, raiz->tipo);
        imprimirArvore(raiz->meio);
        if (raiz->ninfos == 2){
            exibe_dados(raiz->info2, raiz->tipo);
            imprimirArvore(raiz->dir);
        }
    }
}

dado* buscar_item(NoRB* raiz, char* nome) {
    if(raiz && nome){
        dado* resultado = NULL;
        /* obtém chaves do nó conforme tipo */
        char *k1 = raiz->info1.artista.nome;
        char *k2 = NULL;
        if (raiz->ninfos == 2) {
            k2 = raiz->info2.artista.nome;
        }
        /* verifica igualdade no próprio nó primeiro */
        if (compare_numeric_prefix(nome, k1) == 0){
            resultado = &raiz->info1;
        }
        else if (k2 && compare_numeric_prefix(nome, k2) == 0){
            resultado = &raiz->info2;
        }
        else{
            /* decide em qual ramo descer usando compare_numeric_prefix */
            if (compare_numeric_prefix(nome, k1) < 0){
                resultado = buscar_item(raiz->esq, nome);
            }
            else if(raiz->ninfos == 1 ||  compare_numeric_prefix(nome, k2) < 0) {
                resultado = buscar_item(raiz->meio, nome);
            }
            else {
                resultado = buscar_item(raiz->dir, nome);
            }
        }
    }
}

void cont_infos(NoRB* raiz, int* cont){
    if (raiz){
        (*cont)++;
        cont_infos(raiz->esq, cont);
        cont_infos(raiz->meio, cont);
        if(raiz->ninfos==2){
            (*cont)++;
            cont_infos(raiz->dir, cont);
        }
    }
}

// Retorna a menor info de uma subárvore (info1 ou info2)
dado* menor_info(NoRB* raiz) {
    if (!raiz) return NULL;

    NoRB* atual = raiz;
    while (atual->esq) {
        atual = atual->esq;
    }
    // Sempre retorna info1, que é a menor dentro do nó
    return &atual->info1;
}

// Retorna a maior info de uma subárvore (info1 ou info2)
dado* maior_info(NoRB* raiz) {
    if (!raiz) return NULL;

    NoRB* atual = raiz;
    while (atual->dir) {
        atual = atual->dir;
    }
    // Se ninfos == 2, info2 é a maior; senão, info1
    if (atual->ninfos == 2)
        return &atual->info2;
    else
        return &atual->info1;
}




void remove_info1_da_folha(NoRB** raiz, NoRB* pai, int n_da_info){
    if(*raiz){
        if(pai){
            int infos = 0;
            cont_infos(pai,&infos);
            if (pai->esq == *raiz) {
                if((*raiz)->ninfos==2){
                    copia_dados(&(*raiz)->info1, (*raiz)->info2, (*raiz)->tipo);
                    (*raiz)->ninfos=1;
                }
                else if(infos>3){
                        if(pai->meio->ninfos==2){
                            copia_dados(&pai->esq->info1,pai->info1,pai->tipo);
                            copia_dados(&pai->info1,pai->meio->info1,pai->tipo);
                            copia_dados(&pai->meio->info1,pai->meio->info2,pai->tipo);
                            pai->meio->ninfos=1;
                        }
                        else if(pai->ninfos==2){
                            copia_dados(&pai->esq->info1,pai->info1,pai->tipo);
                            copia_dados(&pai->info1,pai->meio->info1,pai->tipo);
                            copia_dados(&pai->meio->info1,pai->info2,pai->tipo);
                            if(pai->dir->ninfos==2){
                                copia_dados(&pai->info2,pai->dir->info1,pai->tipo);
                                copia_dados(&pai->dir->info1,pai->dir->info2,pai->tipo);
                                pai->dir->ninfos=1;
                            }
                            else{
                                copia_dados(&pai->meio->info2,pai->dir->info1,pai->tipo);
                                pai->ninfos = 1;
                                pai->meio->ninfos = 2;
                            }
                        }
                }
                else{
                    copia_dados(&pai->esq->info1, pai->info1, pai->tipo);
                    copia_dados(&pai->esq->info2, pai->meio->info1, pai->tipo);
                    pai->esq->ninfos = 2;
                    free(pai->meio);
                    pai->meio = NULL;
                    pai->ninfos = 0;
                }
            }
            else if(pai->meio == *raiz){
                if((*raiz)->ninfos==2){
                    copia_dados(&pai->dir->info1, (*raiz)->info2, pai->tipo);
                    (*raiz)->ninfos=1;
                }
                else if(infos>3){
                        if(pai->ninfos==1){
                            copia_dados(&pai->meio->info1,pai->info1,pai->tipo);
                            copia_dados(&pai->info1,pai->esq->info2,pai->tipo);
                            pai->esq->ninfos=0;
                        }
                        else{
                            copia_dados(&(*raiz)->info1,pai->info2,pai->tipo);
                            copia_dados(&(pai->info2),pai->dir->info1,pai->tipo);
                            if(pai->dir->ninfos==1){
                                free(pai->dir);
                                copia_dados(&(*raiz)->info2,pai->info2,pai->tipo);
                                pai->ninfos = 1;
                            }
                            else{
                                copia_dados(&pai->dir->info1,pai->dir->info2,pai->tipo);
                            }
                        }
                }
                else{
                    copia_dados(&pai->esq->info2, pai->info1, pai->tipo);
                    pai->esq->ninfos=2;
                    free(pai->meio);
                    pai->meio=NULL;
                    pai->ninfos=0;
                }
            }
            else if(pai->dir == *raiz){
                if((*raiz)->ninfos==2){
                    copia_dados(&pai->dir->info1, (*raiz)->info2, pai->tipo);
                    (*raiz)->ninfos=1;
                }
                else{
                    if(pai->meio->ninfos==1){
                        copia_dados(&pai->meio->info2,pai->info2,pai->tipo);
                        pai->ninfos = 1;
                        pai->meio->ninfos = 2;
                        free(pai->dir);
                    }
                    else{
                        copia_dados(&(*raiz)->info1,pai->info2,pai->tipo);
                        copia_dados(&pai->info2,pai->meio->info2,pai->tipo);
                        pai->meio->ninfos = 1;
                    }
                }
            }
        }
        else{
            // Removendo da raiz
            if((*raiz)->ninfos==2){
                copia_dados(&(*raiz)->info1,(*raiz)->info2,(*raiz)->tipo);
                (*raiz)->ninfos=1;
            }
            else{
                free(*raiz);
                *raiz=NULL;
            }
        }
    }
}


static void fix_parent_after_child_underflow(NoRB *pai, int pos) {
    if (pai){
        if (pos == 0) {
            if(pai->esq->esq->ninfos==1 && pai->ninfos==1 && pai->meio->ninfos==1){
                    copia_dados(&pai->meio->info2, pai->meio->info1, pai->meio->tipo);
                    copia_dados(&pai->meio->info1, pai->esq->esq->info1, pai->meio->tipo);
                    pai->meio->dir = pai->meio->meio;
                    pai->meio->meio = pai->meio->esq;
                    pai->meio->esq = pai->esq->esq;
                    pai->ninfos = 0;
                    pai->esq = pai->meio;
                    pai->meio = NULL;
                    return;
                }
                if(pai->esq->esq->ninfos==2 && pai->ninfos==1 && pai->meio->ninfos==1){
                    copia_dados(&pai->meio->info2, pai->meio->info1, pai->meio->tipo);
                    copia_dados(&pai->meio->info1, pai->info1, pai->meio->tipo);
                    pai->meio->dir = pai->meio->meio;
                    pai->meio->meio = pai->meio->esq;
                    pai->meio->esq = pai->esq->esq;
                    pai->ninfos = 0;
                    free(pai->esq);
                    pai->esq = pai->meio;
                    pai->esq->ninfos = 2;
                    pai->meio = NULL;
                    return;
                }
                if(pai->esq->esq->ninfos==2 && pai->ninfos==2 && pai->meio->ninfos==1){
                    copia_dados(&pai->meio->info2, pai->meio->info1, pai->meio->tipo);
                    copia_dados(&pai->meio->info1, pai->info1, pai->meio->tipo);
                    pai->meio->dir = pai->meio->meio;
                    pai->meio->meio = pai->meio->esq;
                    pai->meio->esq = pai->esq->esq;
                    copia_dados(&pai->info1, pai->info2,pai->meio->tipo);
                    free(pai->esq);
                    pai->esq = pai->meio;
                    pai->meio = pai->dir;
                    pai->dir = NULL;
                    pai->ninfos = 1;
                    pai->esq->ninfos = 2;
                    return;
                }
        }
        /* borrow case: pos == 1 (meio) -> tentar emprestar de left (prefer) ou right */
        else if (pos == 1) {
            if(pai->esq->ninfos==1 && pai->meio->esq->ninfos==2 && pai->ninfos==2){
                copia_dados(&(pai->esq->info2), pai->info1, pai->tipo);
                copia_dados(&pai->info1, pai->info2, pai->tipo);
                pai->esq->dir = pai->meio->esq;
                free(pai->meio);
                pai->meio = pai->dir;
                pai->dir = NULL;
                pai->ninfos=1;
                pai->esq->ninfos = 2;
                return;
            }

            if(pai->esq->ninfos==1 && pai->meio->esq->ninfos==2 && pai->ninfos==2 && pai->dir->ninfos==2){
                copia_dados(&pai->meio->info1,pai->info2, pai->tipo);
                copia_dados(&pai->info2, pai->dir->info1, pai->tipo);
                copia_dados(&(pai->dir->info1), pai->dir->info2, pai->tipo);
                pai->meio->esq = pai->dir->esq;
                pai->dir->esq = pai->dir->meio;
                pai->dir->meio = pai->dir->dir;
                pai->dir->dir = NULL;
                pai->dir->ninfos=1;
                pai->meio->ninfos = 1;
                return;
            }

            if(pai->esq->ninfos==1 && pai->meio->esq->ninfos==2 && pai->ninfos==1){
                copia_dados(&(pai->esq->info2), pai->info1, pai->tipo);
                pai->esq->dir = pai->meio->esq;
                pai->ninfos=0;
                pai->esq->ninfos=2;
                free(pai->meio);
                pai->meio = NULL;
                return;
            }

            if(pai->esq->ninfos==2 && pai->meio->esq->ninfos==2){
                copia_dados(&(pai->meio->info1),pai->info1, pai->tipo);
                copia_dados(&pai->info1, pai->esq->info2, pai->tipo);
                pai->meio->meio = pai->meio->esq;
                pai->meio->esq = pai->esq->dir;
                pai->esq->ninfos=1;
                pai->meio->ninfos = 1;
                return;
            }
        }
        /* borrow case: pos == 2 (dir) -> tentar emprestar de meio */
        if (pos == 2) {
            if(pai->meio->ninfos==1 && pai->ninfos==2){
                copia_dados(&(pai->meio->info2), pai->info2, pai->tipo);
                pai->meio->dir = pai->dir->esq;
                pai->dir = NULL;
                pai->ninfos=1;
                pai->meio->ninfos=2;
                return;
            }
            if(pai->meio->ninfos==2){
                copia_dados(&(pai->dir->info1), pai->info2, pai->tipo);
                copia_dados(&pai->info2, pai->meio->info2, pai->tipo);
                pai->dir->meio = pai->dir->esq;
                pai->dir->esq = pai->meio->dir;
                pai->meio->ninfos=1;
                pai->dir->ninfos=1;
                return;
            }
        }
    }
    
}


// ...existing code...

/* encontra o nó e seu pai contendo o maior elemento da subárvore 'raiz' */
void find_max_node_with_parent(NoRB** raiz,NoRB** pai,dado *maior) {
    if(*raiz){
        if((*raiz)->dir){
            (*pai) = (*raiz);
            find_max_node_with_parent(&(*raiz)->dir, pai, maior);
        }
        else{
            if ((*raiz)->ninfos == 2){
                copia_dados(maior,(*raiz)->info2,(*raiz)->tipo);
            }
            else{
                copia_dados(maior,(*raiz)->info1,(*raiz)->tipo);
            }
        }
    }
}

void find_min_node_with_parent(NoRB** raiz,NoRB** pai,dado *menor) {
    if(*raiz){
        copia_dados(menor,(*raiz)->info1,(*raiz)->tipo);
        if((*raiz)->esq){
            (*pai) = *raiz;
            find_min_node_with_parent(&(*raiz)->esq, pai, menor);
        }
    }
}

/* Função de remoção solicitada: remove valor que pode estar em nó interno.
   Quando a chave está em info1 de nó interno, substitui por maior da subárvore esquerda,
   remove duplicata na subárvore esquerda e, se esse filho ficar sem infos, reorganiza apenas
   o nó pai ('pai da subárvore' que é *raiz aqui) usando fix_parent_after_child_underflow.
   A rotina faz ajustes locais no pai; underflow do próprio pai será tratado pela recursão superior. */
void remove_no(NoRB** raiz, char* info, NoRB* pai, int* flag) {
    if (*raiz) {
        char *k1 = (*raiz)->info1.artista.nome;
        char *k2 = ((*raiz)->ninfos == 2) ? (*raiz)->info2.artista.nome : NULL;
        if (compare_numeric_prefix(info, k1) == 0) {
            if (eh_folha(*raiz)) {
                /* remover info1 em folha com 1 info */
                remove_info1_da_folha(raiz, pai, 1);
                *flag = 1; 
            }
            else {
                /* nó interno: procurar maior na subárvore esquerda */
                dado max_info;
                NoRB *pai_no = (*raiz);
                find_max_node_with_parent(&(*raiz)->esq, &pai_no, &max_info);
                /* copia maior encontrado para info1 do nó atual */
                copia_dados(&(*raiz)->info1, max_info, (*raiz)->tipo);
                remove_no(&(*raiz)->esq, max_info.artista.nome, (*raiz), flag);
                /* após retorno: se o filho esquerdo ficou sem infos (ou foi liberado), reorganize apenas o nó 'no' */
                if((pai->esq==(*raiz)) && (*raiz)->esq->ninfos == 0) {
                    fix_parent_after_child_underflow(pai, 0);
                }
                *flag = 1;
            }
        }
        /* caso: info é info2 no nó atual */
        else if (k2 && compare_numeric_prefix(info, k2) == 0) {
            /* se for folha, remova diretamente */
            if (eh_folha(*raiz)) {
                /* remover info2 em folha */
                (*raiz)->ninfos = 1;
                *flag = 1;
            }
            else{
                /* nó interno: procurar maior da subárvore meio/direita conforme política (usar dir preferencial) */
                /* procurar maior na subárvore do meio primeiro (pode ajustar para dir se desejar) */
                NoRB *pai_no = NULL;
                dado info;
                find_min_node_with_parent(&(*raiz)->dir, &pai_no, &info);
                if (!pai_no) {
                    find_max_node_with_parent(&(*raiz)->meio, &pai_no, &info);
                    copia_dados(&(*raiz)->info2,info, (*raiz)->tipo);
                    remove_no(&(*raiz)->meio, info.artista.nome, (*raiz), flag);
                    if ((*raiz)->dir->ninfos == 0){
                        fix_parent_after_child_underflow(pai, 2);
                    }
                    *flag = 1;
                }
                else {
                    /* copiar maior do meio para info2 */
                    copia_dados(&(*raiz)->info2, info, (*raiz)->tipo);
                    remove_no(&(*raiz)->dir, info.artista.nome, (*raiz), flag);
                    if ((*raiz)->ninfos == 0) {
                        fix_parent_after_child_underflow(pai, 1);
                    }
                    *flag = 1;
                }
            }
        }
        else{
            /* caso: chave não está no nó -> descer recursivamente para o filho correto */
            if (compare_numeric_prefix(info, k1) < 0) {
                remove_no(&(*raiz)->esq, info, (*raiz), flag);
                /* se filho esq ficou com 0 infos, conserte apenas o nó no */
                if(pai){
                    if(pai->meio==(*raiz) && (*raiz)->ninfos == 0) {
                        fix_parent_after_child_underflow(pai, 1);
                    }
                    else if (pai->esq==(*raiz) && (*raiz)->ninfos == 0) {
                        fix_parent_after_child_underflow(pai, 0);
                    }
                }
            }
            else {
                if (k2 == NULL || compare_numeric_prefix(info, k2) < 0) {
                    remove_no(&(*raiz)->meio, info, (*raiz), flag);
                    if(pai){
                        if(pai->esq==(*raiz) && (*raiz)->ninfos == 0) {

                            fix_parent_after_child_underflow(pai, 0);
                        }
                        else if(pai->meio == (*raiz) && (*raiz)->ninfos == 0) {
                            fix_parent_after_child_underflow(pai, 1);
                        }
                    }
                }
                else{
                    remove_no(&(*raiz)->dir, info, (*raiz), flag);
                    if (((*raiz)->ninfos == 0)) {
                        fix_parent_after_child_underflow(pai, 2);
                    }
                }
            }
        }

    }
}

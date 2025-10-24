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

void imprimirArvore(NoRB *raiz,int*cont){
    if (raiz){
        imprimirArvore(raiz->esq,cont);
        exibe_dados(raiz->info1, raiz->tipo);
        (*cont)++;
        printf("%d ",*cont);
        imprimirArvore(raiz->meio,cont);
        if (raiz->ninfos == 2){
            exibe_dados(raiz->info2, raiz->tipo);
            (*cont)++;
            printf("%d ",*cont);
            imprimirArvore(raiz->dir,cont);
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
    if(*cont<=3){
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
        if(n_da_info==1){
            if((*raiz)->ninfos == 2){
                    /* desliza info2 para info1 */
                    copia_dados(&(*raiz)->info1, (*raiz)->info2, (*raiz)->tipo);
                    (*raiz)->ninfos = 1;
                }
                else{
                    if(pai->esq == *raiz){
                        int num_infos = 0;
                        cont_infos(pai,&num_infos);
                        if(num_infos<=3){
                            copia_dados(&pai->info2,pai->meio->info1,(*raiz)->tipo);
                            free(pai->meio);
                            free(pai->esq);
                            pai->meio = NULL;
                            pai->esq = NULL;
                        }
                        else{
                            copia_dados(&(*raiz)->info1, pai->info1, (*raiz)->tipo);
                            copia_dados(&pai->info1, pai->meio->info1, (*raiz)->tipo);
                            if(pai->meio->ninfos==2){
                                copia_dados(&pai->meio->info1,pai->meio->info2,(*raiz)->tipo);
                                pai->meio->ninfos=1;
                            }
                            else{
                                copia_dados(&pai->meio->info1,pai->info2,(*raiz)->tipo);
                                copia_dados(&pai->info2,pai->dir->info1,(*raiz)->tipo);
                                if(pai->dir->ninfos==1){
                                    free(pai->dir);
                                    copia_dados(&pai->info2,pai->meio->info2,(*raiz)->tipo);
                                    pai->dir->ninfos=1;
                                }
                                else{
                                    copia_dados(&pai->dir->info1,pai->dir->info2,(*raiz)->tipo);
                                    pai->dir->ninfos=1;
                                }
                            }
                        }
                    }
                    else if(pai->meio == *raiz){
                        if(pai->ninfos==2){
                            copia_dados(&(*raiz)->info1,pai->info2,(*raiz)->tipo);
                            pai->meio->ninfos=1;
                            if(pai->dir){
                                if(pai->dir->ninfos==2){
                                    copia_dados(&pai->info2,pai->dir->info1,(*raiz)->tipo);
                                    copia_dados(&pai->dir->info1,pai->dir->info2,(*raiz)->tipo);
                                    pai->dir->ninfos=1;
                                }
                                else{
                                    copia_dados(&(*raiz)->info2,pai->dir->info1,(*raiz)->tipo);
                                    free(pai->dir);
                                    pai->dir=NULL;
                                    pai->ninfos=1;
                                }
                            }
                        }
                    }
                    else if(pai->dir == *raiz){
                        if(pai->meio->ninfos==1){
                            copia_dados(&pai->meio->info2,pai->info2,(*raiz)->tipo);
                            free(pai->dir);
                            pai->dir=NULL;
                            pai->ninfos=1;
                        }
                        else{
                            copia_dados(&pai->dir->info1,pai->info2,(*raiz)->tipo);
                            copia_dados(&pai->info2,pai->meio->info2,(*raiz)->tipo);
                            pai->meio->ninfos=1;
                        }
                    }
                    free(*raiz);
                    *raiz = NULL;
                }
            }
        }
        else if(n_da_info==2){
            if((*raiz)->ninfos==2){
                (*raiz)->ninfos=1;
            }
        }
}

/* encontra o nó e seu pai contendo o maior elemento da subárvore 'raiz' */
static NoRB *find_max_node_with_parent(NoRB *raiz, NoRB **out_parent) {
    NoRB *parent = NULL;
    NoRB *cur = raiz;
    if (!cur) {
        if (out_parent) *out_parent = NULL;
        return NULL;
    }
    while (cur->dir) {
        parent = cur;
        cur = cur->dir;
    }
    if (out_parent) *out_parent = parent;
    return cur;
}
/* ajusta apenas o nó 'pai' quando o filho na posição pos (0=esq,1=meio,2=dir) ficou com 0 infos.
   Esta rotina NÃO tenta resolver underflow do próprio pai; apenas rearranja valores/filhos localmente
   conforme a especificação fornecida. */
static void fix_parent_after_child_underflow(NoRB *pai, int pos) {
    if (pai){
        NoRB *left = pai->esq;
        NoRB *mid  = pai->meio;
        NoRB *right = pai->dir;

        /* helper para puxar menor da subárvore (usado abaixo) */
        dado *menor_right = NULL;
        if (right) menor_right = menor_info(right);

        if (pos == 0) {
            /* underflow no filho esquerdo */
            if (pai->ninfos == 1) {
                /* pai tem 1 info: descer info1 do pai para a esquerda e juntar esquerda+meio */
                if (!left) {
                    /* criar left se faltante */
                    left = mid;
                    pai->esq = left;
                    pai->meio = NULL;
                }
                if (left) {
                    copia_dados(&left->info1, pai->info1, pai->tipo);
                    if (mid) {
                        /* se meio tem 2 infos, usar meio->info1 para preencher pai->info1 posteriormente;
                        caso meio tenha 1 info, após juntar left e meio o pai ficará sem infos (ninfos=0) */
                        if (mid->ninfos == 2) {
                            /* mover meio->info1 para left->info2, ajustar meio */
                            copia_dados(&left->info2, mid->info1, pai->tipo);
                            left->ninfos = 2;
                            /* promover meio->info2 para pai->info1 */
                            copia_dados(&pai->info1, mid->info2, pai->tipo);
                            mid->ninfos = 1;
                        } else {
                            /* meio tem 1 info: left absorve meio -> left terá 2 infos, pai perde sua única info */
                            copia_dados(&left->info2, mid->info1, pai->tipo);
                            left->ninfos = 2;
                            /* remover meio e ajustar ponteiros */
                            free(mid);
                            pai->meio = pai->dir;
                            pai->dir = NULL;
                            pai->ninfos = 0; /* pai perdeu sua única info */
                        }
                    } else {
                        /* não há meio: apenas mover info1 para left */
                        left->ninfos = 1;
                        pai->ninfos = 0;
                    }
                }
            } else {
                /* pai tem 2 infos: desce info1 para left, move info2->info1 e recolhe um menor da dir para info2,
                depois junta left e meio (mantendo ordem) */
                if (!left) left = pai->esq; /* safety */
                if (left) {
                    copia_dados(&left->info1, pai->info1, pai->tipo);
                    /* shift parent infos */
                    copia_dados(&pai->info1, pai->info2, pai->tipo);
                /* preencher info2 com menor da direita, se existir */
                    if (menor_right) {
                        copia_dados(&pai->info2, *menor_right, pai->tipo);
                    } else {
                        /* se não houver right, tentar preencher com meio->info1 se disponível */
                        if (mid && mid->ninfos >= 1) {
                            copia_dados(&pai->info2, mid->info1, pai->tipo);
                            /* quando usamos mid->info1, mid precisa ser ajustado abaixo */
                        }
                    }

                    /* agora juntar left + mid: colocar em left a ordem (left.info1, possivelmente left.info2, mid.info1) */
                    if (mid) {
                        if (mid->ninfos == 2) {
                            /* left.info2 = mid.info1, left.ninfos = 2; mid shift */
                            copia_dados(&left->info2, mid->info1, pai->tipo);
                            left->ninfos = 2;
                            /* ajustar mid: promover mid.info2 para mid.info1 */
                            copia_dados(&mid->info1, mid->info2, pai->tipo);
                            mid->ninfos = 1;
                        } else {
                            /* mid has 1 info: left absorbs it -> left.ninfos = 2; remove mid pointer */
                            copia_dados(&left->info2, mid->info1, pai->tipo);
                            left->ninfos = 2;
                            free(mid);
                            /* shift pointers: meio becomes dir, dir becomes NULL */
                            pai->meio = pai->dir;
                            pai->dir = NULL;
                            /* pai still has 2 infos but we've adjusted children */
                        }
                    }
                }
            }
        } else if (pos == 1) {
            /* underflow no filho do meio */
            if (pai->ninfos == 1) {
                /* pai possui uma info: descer info1 para meio e juntar meio com esquerda */
                if (!mid) {
                    mid = pai->meio;
                    pai->meio = NULL;
                }
                if (mid) {
                    copia_dados(&mid->info1, pai->info1, pai->tipo);
                    if (left && left->ninfos >= 1) {
                        /* left->info2 passa para mid->info2 e left perde info2 (ou é liberado) */
                        if (left->ninfos == 2) {
                            copia_dados(&mid->info2, left->info2, pai->tipo);
                            mid->ninfos = 2;
                            left->ninfos = 1;
                        } else {
                            /* left has 1 info: mid absorbs left -> mid.ninfos=2; remove left */
                            copia_dados(&mid->info2, left->info1, pai->tipo);
                            mid->ninfos = 2;
                            free(left);
                            pai->esq = NULL;
                            pai->meio = mid;
                            pai->ninfos = 0; /* pai lost its only info */
                        }
                    }
                }
            } else {
                /* pai tem 2 infos: descer info1 do pai para info2 do meio, e juntar meio com esquerdo */
                if (mid) {
                    copia_dados(&mid->info2, pai->info1, pai->tipo);
                    mid->ninfos = 2;
                    /* now shift parent: parent.info1 becomes parent.info2 */
                    copia_dados(&pai->info1, pai->info2, pai->tipo);
                    /* and try to fill parent.info2 from right subtree */
                    if (right && right->ninfos >= 1) {
                        dado *m = menor_info(right);
                        if (m) copia_dados(&pai->info2, *m, pai->tipo);
                    }
                    /* finally merge medio with esquerda: move left.info2 or left.info1 as needed */
                    if (left) {
                        if (left->ninfos == 2) {
                            /* left.info2 moves into mid.info1 (but mid already has two infos) -> instead shift */
                            copia_dados(&mid->info1, left->info2, pai->tipo);
                            /* adjust left */
                            left->ninfos = 1;
                        } else {
                            /* left has 1 info: mid will already have two infos; free left and adjust pointers */
                            free(left);
                            pai->esq = NULL;
                        }
                    }
                }
            }
        } else if (pos == 2) {
            /* underflow no filho direito */
            if (pai->ninfos == 1) {
                /* pai tem 1 info: descer info1 (único) para a direita e juntar meio+dir */
                if (!right) right = pai->dir;
                if (right) {
                    copia_dados(&right->info1, pai->info1, pai->tipo);
                    if (mid) {
                        if (mid->ninfos == 2) {
                            /* mid.info2 -> right.info2 */
                            copia_dados(&right->info2, mid->info2, pai->tipo);
                            right->ninfos = 2;
                            mid->ninfos = 1;
                        } else {
                            /* mid has 1 info: right absorve mid */
                            copia_dados(&right->info2, mid->info1, pai->tipo);
                            right->ninfos = 2;
                            free(mid);
                            pai->meio = NULL;
                            pai->dir = right;
                            pai->ninfos = 0;
                        }
                    }
                }
            } else {
                /* pai tem 2 infos: desce info2 (se existir) ou info1 para a direita e juntar meio+dir */
                if (!right) right = pai->dir;
                if (right) {
                    if (pai->ninfos == 2) {
                        copia_dados(&right->info1, pai->info2, pai->tipo);
                        /* parent.info2 should be filled later by a value from left/center if needed (not handled here) */
                    } else {
                        copia_dados(&right->info1, pai->info1, pai->tipo);
                    }

                    if (mid) {
                        if (mid->ninfos == 2) {
                            /* mid.info2 moves into right.info2 */
                            copia_dados(&right->info2, mid->info2, pai->tipo);
                            right->ninfos = 2;
                            mid->ninfos = 1;
                        } else {
                            copia_dados(&right->info2, mid->info1, pai->tipo);
                            right->ninfos = 2;
                            free(mid);
                            pai->meio = NULL;
                        }
                    }
                    /* after merging we may want to clear parent.info2 if we used it; leave parent state
                    to be treated by upper recursion (as requested). */
                }
            }
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
                remove_info1_da_folha(raiz, pai, 1);
                *flag = 1; 
            }
            else {
                /* nó interno: procurar maior na subárvore esquerda */
                NoRB *parent_max = NULL;
                NoRB *max_node = find_max_node_with_parent((*raiz)->esq, &parent_max);
                /* copia maior encontrado para info1 do nó atual */
                if (max_node->ninfos == 2) {
                    copia_dados(&(*raiz)->info1, max_node->info2, (*raiz)->tipo);
                }
                else {
                    copia_dados(&(*raiz)->info1, max_node->info1, (*raiz)->tipo);
                }
                /* agora remover a ocorrência duplicada na subárvore esquerda.
                Chama recursivamente remove_no na subárvore esquerda para apagar a chave usada. */
                char key_to_remove[MAX_NOME];
                if (max_node->ninfos == 2){
                    strncpy(key_to_remove, max_node->info2.artista.nome, MAX_NOME);
                }
                else{
                    strncpy(key_to_remove, max_node->info1.artista.nome, MAX_NOME);
                }
                key_to_remove[MAX_NOME-1] = '\0';
                int f = 0;
                remove_no(&(*raiz)->esq, key_to_remove, *raiz, &f);
                /* após retorno: se o filho esquerdo ficou sem infos (ou foi liberado), reorganize apenas o nó 'no' */
                if((*raiz)->esq == NULL || ((*raiz)->esq && (*raiz)->esq->ninfos == 0)) {
                    fix_parent_after_child_underflow(*raiz, 0);
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
                NoRB *parent_max = NULL;
                NoRB *max_node = find_max_node_with_parent((*raiz)->meio, &parent_max);
                if (!max_node) {
                    max_node = find_max_node_with_parent((*raiz)->dir, &parent_max);
                    /* copiar maior da dir para info2 */
                    if (max_node->ninfos == 2){
                        copia_dados(&(*raiz)->info2, max_node->info2, (*raiz)->tipo);
                    }
                    else{
                        copia_dados(&(*raiz)->info2, max_node->info1, (*raiz)->tipo);
                    }
                    /* remover duplicata recursivamente na subárvore direita */
                    char key_to_remove[MAX_NOME];
                    if(max_node->ninfos == 2){
                        strncpy(key_to_remove, max_node->info2.artista.nome, MAX_NOME);
                    }
                    else {
                        strncpy(key_to_remove, max_node->info1.artista.nome, MAX_NOME);
                    }
                    key_to_remove[MAX_NOME-1] = '\0';
                    int f = 0;
                    remove_no(&(*raiz)->dir, key_to_remove, *raiz, &f);
                    if ((*raiz)->dir == NULL || ((*raiz)->dir && (*raiz)->dir->ninfos == 0)) {
                        fix_parent_after_child_underflow(*raiz, 2);
                    }
                    *flag = 1;
                }
                else {
                    /* copiar maior do meio para info2 */
                    if (max_node->ninfos == 2){
                        copia_dados(&(*raiz)->info2, max_node->info2, (*raiz)->tipo);
                    }
                    else {
                        copia_dados(&(*raiz)->info2, max_node->info1, (*raiz)->tipo);
                    }
                    char key_to_remove[MAX_NOME];
                    if(max_node->ninfos == 2){
                        strncpy(key_to_remove, max_node->info2.artista.nome, MAX_NOME);
                    }
                    else{
                        strncpy(key_to_remove, max_node->info1.artista.nome, MAX_NOME);
                    }
                    key_to_remove[MAX_NOME-1] = '\0';
                    int f = 0;
                    remove_no(&(*raiz)->meio, key_to_remove, *raiz, &f);
                    if ((*raiz)->meio == NULL || ((*raiz)->meio && (*raiz)->meio->ninfos == 0)) {
                        fix_parent_after_child_underflow(*raiz, 1);
                    }
                    *flag = 1;
                }
            }
        }
        else{
            /* caso: chave não está no nó -> descer recursivamente para o filho correto */
            if (compare_numeric_prefix(info, k1) < 0) {
                remove_no(&(*raiz)->esq, info, *raiz, flag);
                /* se filho esq ficou com 0 infos, conserte apenas o nó no */
                if ((*raiz)->esq == NULL || ((*raiz)->esq && (*raiz)->esq->ninfos == 0)) {
                    fix_parent_after_child_underflow(*raiz, 0);
                }
            }
            else {
                if (k2 == NULL || compare_numeric_prefix(info, k2) < 0) {
                    remove_no(&(*raiz)->meio, info, *raiz, flag);
                    if ((*raiz)->meio == NULL || ((*raiz)->meio && (*raiz)->meio->ninfos == 0)) {
                        fix_parent_after_child_underflow(*raiz, 1);
                    }
                }
                else{
                    remove_no(&(*raiz)->dir, info, *raiz, flag);
                    if ((*raiz)->dir == NULL || ((*raiz)->dir && (*raiz)->dir->ninfos == 0)) {
                        fix_parent_after_child_underflow(*raiz, 2);
                    }
                }
            }
        }

    }
}

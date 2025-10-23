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

/* Funções para remoção em Árvore 2-3 */

/* Verifica se um nó tem apenas 1 elemento */
int tem_um_elemento(NoRB *no)
{
    return (no != NULL && no->ninfos == 1);
}

/* Busca o maior elemento na subárvore (antecessor) */
dado* buscar_maior(NoRB *no, char **chave)
{
    if (no == NULL) return NULL;
    
    if (eh_folha(no)) {
        if (no->ninfos == 2) {
            if (no->tipo == NO_ARTISTA) {
                *chave = no->info2.artista.nome;
            } else {
                *chave = no->info2.album.nome;
            }
            return &no->info2;
        } else {
            if (no->tipo == NO_ARTISTA) {
                *chave = no->info1.artista.nome;
            } else {
                *chave = no->info1.album.nome;
            }
            return &no->info1;
        }
    } else {
        if (no->dir != NULL) {
            return buscar_maior(no->dir, chave);
        } else if (no->meio != NULL) {
            return buscar_maior(no->meio, chave);
        } else {
            return buscar_maior(no->esq, chave);
        }
    }
}

/* Busca o menor elemento na subárvore (sucessor) */
dado* buscar_menor(NoRB *no, char **chave)
{
    if (no == NULL) return NULL;
    
    if (eh_folha(no)) {
        if (no->tipo == NO_ARTISTA) {
            *chave = no->info1.artista.nome;
        } else {
            *chave = no->info1.album.nome;
        }
        return &no->info1;
    } else {
        return buscar_menor(no->esq, chave);
    }
}

/* Obtém a chave de um dado conforme seu tipo */
char* obter_chave(dado *d, TipoNo tipo)
{
    if (tipo == NO_ARTISTA) {
        return d->artista.nome;
    } else {
        return d->album.nome;
    }
}

/* Faz rotação à direita (irmão direito tem 2 elementos) */
void rotacao_direita(NoRB *pai, int pos_filho)
{
    NoRB *filho, *irmao;
    
    if (pos_filho == 0) {  /* filho esquerdo pega de centro */
        filho = pai->esq;
        irmao = pai->meio;
        
        if (irmao && irmao->ninfos == 2) {
            copia_dados(&filho->info2, &pai->info1, pai->tipo);
            copia_dados(&pai->info1, &irmao->info1, pai->tipo);
            copia_dados(&irmao->info1, &irmao->info2, pai->tipo);
            irmao->info2 = (dado){0};
            
            filho->dir = irmao->esq;
            irmao->esq = irmao->meio;
            irmao->meio = NULL;
            
            filho->ninfos = 2;
            irmao->ninfos = 1;
        }
    } else if (pos_filho == 1) {  /* filho central pega de direito */
        filho = pai->meio;
        irmao = pai->dir;
        
        if (irmao && irmao->ninfos == 2) {
            copia_dados(&filho->info2, &pai->info2, pai->tipo);
            copia_dados(&pai->info2, &irmao->info1, pai->tipo);
            copia_dados(&irmao->info1, &irmao->info2, pai->tipo);
            irmao->info2 = (dado){0};
            
            filho->dir = irmao->esq;
            irmao->esq = irmao->meio;
            irmao->meio = NULL;
            
            filho->ninfos = 2;
            irmao->ninfos = 1;
        }
    }
}

/* Faz rotação à esquerda (irmão esquerdo tem 2 elementos) */
void rotacao_esquerda(NoRB *pai, int pos_filho)
{
    NoRB *filho, *irmao;
    
    if (pos_filho == 1) {  /* filho central pega de esquerdo */
        filho = pai->meio;
        irmao = pai->esq;
        
        if (irmao && irmao->ninfos == 2) {
            copia_dados(&filho->info2, &filho->info1, pai->tipo);
            copia_dados(&filho->info1, &pai->info1, pai->tipo);
            copia_dados(&pai->info1, &irmao->info2, pai->tipo);
            irmao->info2 = (dado){0};
            
            filho->meio = filho->esq;
            filho->esq = irmao->dir;
            irmao->dir = NULL;
            
            filho->ninfos = 2;
            irmao->ninfos = 1;
        }
    } else if (pos_filho == 2) {  /* filho direito pega de central */
        filho = pai->dir;
        irmao = pai->meio;
        
        if (irmao && irmao->ninfos == 2) {
            copia_dados(&filho->info2, &filho->info1, pai->tipo);
            copia_dados(&filho->info1, &pai->info2, pai->tipo);
            copia_dados(&pai->info2, &irmao->info2, pai->tipo);
            irmao->info2 = (dado){0};
            
            filho->meio = filho->esq;
            filho->esq = irmao->dir;
            irmao->dir = NULL;
            
            filho->ninfos = 2;
            irmao->ninfos = 1;
        }
    }
}

/* Faz fusão de nós: junta filho com irmão */
void mesclar_filhos(NoRB *pai, int pos_filho)
{
    NoRB *esquerdo, *direito;
    
    if (pos_filho == 0) {  /* mescla esquerdo com central */
        esquerdo = pai->esq;
        direito = pai->meio;
        
        copia_dados(&esquerdo->info2, &pai->info1, pai->tipo);
        esquerdo->ninfos = 2;
        esquerdo->meio = direito->esq;
        esquerdo->dir = direito->meio;
        
        copia_dados(&pai->info1, &pai->info2, pai->tipo);
        pai->info2 = (dado){0};
        pai->ninfos = 1;
        pai->meio = direito->dir;
        
        free(direito);
    } else if (pos_filho == 1) {  /* mescla central com direito */
        esquerdo = pai->meio;
        direito = pai->dir;
        
        copia_dados(&esquerdo->info2, &pai->info2, pai->tipo);
        esquerdo->ninfos = 2;
        esquerdo->meio = direito->esq;
        esquerdo->dir = direito->meio;
        
        pai->info2 = (dado){0};
        pai->ninfos = 1;
        pai->dir = direito->dir;
        
        free(direito);
    }
}

/* Reequilibra a árvore antes de descer */
void reequilibrar(NoRB *pai, int pos_filho)
{
    NoRB *filho = NULL;
    
    if (pos_filho == 0) filho = pai->esq;
    else if (pos_filho == 1) filho = pai->meio;
    else filho = pai->dir;
    
    if (filho == NULL) return;
    
    /* Tenta rotação à direita */
    if (pos_filho < 2) {
        NoRB *irmao_dir = (pos_filho == 0) ? pai->meio : pai->dir;
        if (irmao_dir && irmao_dir->ninfos == 2) {
            rotacao_direita(pai, pos_filho);
            return;
        }
    }
    
    /* Tenta rotação à esquerda */
    if (pos_filho > 0) {
        NoRB *irmao_esq = (pos_filho == 1) ? pai->esq : pai->meio;
        if (irmao_esq && irmao_esq->ninfos == 2) {
            rotacao_esquerda(pai, pos_filho);
            return;
        }
    }
    
    /* Se não conseguiu rotacionar, faz fusão */
    if (pos_filho == 0) {
        mesclar_filhos(pai, 0);
    } else if (pos_filho == 1) {
        if (pai->dir != NULL) {
            mesclar_filhos(pai, 1);
        } else {
            mesclar_filhos(pai, 0);
        }
    } else {
        mesclar_filhos(pai, 1);
    }
}

/* Função principal de remoção */
int remover_no(NoRB **raiz, char *chave, TipoNo tipo)
{
    if (*raiz == NULL) return 0;
    
    NoRB *no = *raiz;
    char *k1, *k2 = NULL;
    
    /* Obtém chaves do nó */
    if (no->tipo == NO_ARTISTA) {
        k1 = no->info1.artista.nome;
        if (no->ninfos == 2) k2 = no->info2.artista.nome;
    } else {
        k1 = no->info1.album.nome;
        if (no->ninfos == 2) k2 = no->info2.album.nome;
    }
    
    if (cmp_keys(chave, k1) == 0) {  /* Remove info1 */
        if (eh_folha(no)) {
            /* Elemento é folha: remover diretamente */
            no->info1 = no->info2;
            no->info2 = (dado){0};
            no->ninfos--;
            return 1;
        } else {
            /* Não é folha: substituir por antecessor ou sucessor */
            char *chave_aux = NULL;
            dado *antecessor;
            
            if (no->esq && no->esq->ninfos > 1) {
                /* Usar antecessor do filho esquerdo */
                antecessor = buscar_maior(no->esq, &chave_aux);
                copia_dados(&no->info1, antecessor, no->tipo);
                remover_no(&no->esq, chave_aux, tipo);
            } else if (no->meio && no->meio->ninfos > 1) {
                /* Usar sucessor do filho central */
                antecessor = buscar_menor(no->meio, &chave_aux);
                copia_dados(&no->info1, antecessor, no->tipo);
                remover_no(&no->meio, chave_aux, tipo);
            } else {
                /* Fazer merge e remover recursivamente */
                mesclar_filhos(no, 0);
                remover_no(raiz, chave, tipo);
            }
            return 1;
        }
    } else if (k2 && cmp_keys(chave, k2) == 0) {  /* Remove info2 */
        if (eh_folha(no)) {
            /* Elemento é folha: remover diretamente */
            no->info2 = (dado){0};
            no->ninfos--;
            return 1;
        } else {
            /* Não é folha: substituir por antecessor ou sucessor */
            char *chave_aux = NULL;
            dado *antecessor;
            
            if (no->meio && no->meio->ninfos > 1) {
                /* Usar antecessor do filho central */
                antecessor = buscar_maior(no->meio, &chave_aux);
                copia_dados(&no->info2, antecessor, no->tipo);
                remover_no(&no->meio, chave_aux, tipo);
            } else if (no->dir && no->dir->ninfos > 1) {
                /* Usar sucessor do filho direito */
                antecessor = buscar_menor(no->dir, &chave_aux);
                copia_dados(&no->info2, antecessor, no->tipo);
                remover_no(&no->dir, chave_aux, tipo);
            } else {
                /* Fazer merge e remover recursivamente */
                mesclar_filhos(no, 1);
                remover_no(raiz, chave, tipo);
            }
            return 1;
        }
    } else {
        /* Não está em Raiz: buscar em subárvores */
        int pos_filho = -1;
        
        if (cmp_keys(chave, k1) < 0) {
            pos_filho = 0;  /* Filho esquerdo */
        } else if (no->ninfos == 1 || (k2 && cmp_keys(chave, k2) < 0)) {
            pos_filho = 1;  /* Filho central */
        } else {
            pos_filho = 2;  /* Filho direito */
        }
        
        /* Verifica se filho tem apenas 1 elemento antes de descer */
        NoRB *filho = NULL;
        if (pos_filho == 0) filho = no->esq;
        else if (pos_filho == 1) filho = no->meio;
        else filho = no->dir;
        
        if (filho && tem_um_elemento(filho)) {
            reequilibrar(no, pos_filho);
        }
        
        /* Após reequilibrar, pode ter mudado a posição, então tenta novamente */
        if (pos_filho == 0 && remover_no(&no->esq, chave, tipo)) return 1;
        else if (pos_filho == 1 && remover_no(&no->meio, chave, tipo)) return 1;
        else if (pos_filho == 2 && remover_no(&no->dir, chave, tipo)) return 1;
        
        return 0;
    }
}

/* Wrapper para remover artista */
int remover_artista(NoRB **raiz, char *nome, int *flag)
{
    if (remover_no(raiz, nome, NO_ARTISTA)) {
        /* Se raiz ficou vazia, promove seu único filho */
        if (*raiz && (*raiz)->ninfos == 0) {
            NoRB *temp = *raiz;
            if ((*raiz)->esq != NULL) {
                *raiz = (*raiz)->esq;
            } else if ((*raiz)->meio != NULL) {
                *raiz = (*raiz)->meio;
            } else if ((*raiz)->dir != NULL) {
                *raiz = (*raiz)->dir;
            } else {
                *raiz = NULL;
            }
            free(temp);
        }
        *flag = 1;
        return 1;
    }
    *flag = 0;
    return 0;
}

/* Wrapper para remover álbum */
int remover_album(NoRB **albuns, char *nome, int *flag)
{
    if (remover_no(albuns, nome, NO_ALBUM)) {
        /* Se raiz ficou vazia, promove seu único filho */
        if (*albuns && (*albuns)->ninfos == 0) {
            NoRB *temp = *albuns;
            if ((*albuns)->esq != NULL) {
                *albuns = (*albuns)->esq;
            } else if ((*albuns)->meio != NULL) {
                *albuns = (*albuns)->meio;
            } else if ((*albuns)->dir != NULL) {
                *albuns = (*albuns)->dir;
            } else {
                *albuns = NULL;
            }
            free(temp);
        }
        *flag = 1;
        return 1;
    }
    *flag = 0;
    return 0;
}
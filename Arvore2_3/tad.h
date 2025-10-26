
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _MSC_VER
/* disponibiliza protótipos para strdup e strcasecmp em MinGW/GCC */
char *strdup(const char *s);
int strcasecmp(const char *s1, const char *s2);
#endif

#define MAX_NOME   50
#define MAX_TITULO 50
#define MAX_ESTILO 50

#define VERMELHO 0
#define PRETO    1

/* Lista encadeada de músicas */
typedef struct Musica {
    char *titulo;
    char* minutos;
    struct Musica *prox;
} Musica;



typedef struct DadosAlbum{
    char nome[50];
    int ano;
    int qtd_musicas;
    Musica *musicas;       // lista encadeada ordenada de músicas
} DadosAlbum;

typedef struct DadosArtista{
    char nome[50];
    char estilo[50];
    int qtd_albuns;
    struct NoRB *albuns;  
} DadosArtista;

typedef enum {
    NO_ARTISTA,
    NO_ALBUM
} TipoNo;

typedef union {
        DadosArtista artista;
        DadosAlbum album;
} dado;

typedef struct NoRB {
    TipoNo tipo;
    dado info1;
    dado info2;
    int ninfos;
    struct NoRB *esq;
    struct NoRB *dir;
    struct NoRB *meio;
} NoRB;




// ---------------- Protótipos ---------------- 
// Funções genéricas para árvore 2-3
int libera_albuns(NoRB* album);
NoRB *criar_no(dado *dados, NoRB *FEsq, NoRB *FCen, TipoNo tipo);
int eh_folha(NoRB *NO);
void copia_dados(dado* recebe, dado envia, TipoNo tipo);
void adiciona_info(NoRB** NO, dado* info, NoRB* filho);
NoRB* quebrar_NO(NoRB** NO, dado* info, NoRB* filho, dado** sobe);
NoRB* inserirNo(NoRB **raiz, dado *dados, TipoNo tipo, NoRB* pai, dado** sobe, int* flag);
void exibe_dados(dado dado,TipoNo tipo);
void imprimirArvore(NoRB *raiz);
dado* buscar_item(NoRB* raiz, char* nome);
int cont_filhos(NoRB* raiz, int* cont);
void remove_no(NoRB** raiz,char* nome,NoRB* pai,int* flag);
//Musicas.c
void libera_musicas(Musica* lista);
Musica* criar_musica(char* titulo,char* minutos);
void cadastra_musica(Musica** lista,char* titulo,char* duracao,int *flag);
void exibe_musicas(Musica* lista);
Musica* Busca_musicas(Musica* lista,char* nome);
void remove_musicas(Musica** lista,char *titulo,int* flag);

//utils.c
char confirma_desejo_de_remover_artista(char* nome);
char confirma_desejo_de_remover_album(char* nome);
void deixa_string_minuscula(char *s);
void ler_dados_artista(dado* artista,char* nome,char* estilo);
void ler_dados_album(dado* album, char* nome, int* ano);
int validar_estilo(const char* estilo);
int validarFormatoduracao(const char* duracao);
int seed(int num, NoRB **raiz);
int cmp_keys(const char *a, const char *b);
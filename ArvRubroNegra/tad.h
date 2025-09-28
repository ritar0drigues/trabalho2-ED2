
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME   50
#define MAX_TITULO 50
#define MAX_ESTILO 50

#define VERMELHO 0
#define PRETO    1


/* Lista encadeada de músicas */
typedef struct Musica {
    char *titulo;
    int minutos;
    struct Musica *prox;
} Musica;



typedef struct DadosAlbum{
    char *titulo;
    int ano;
    int qtd_musicas;
    Musica *musicas;       // lista encadeada ordenada de músicas
} DadosAlbum;

typedef struct DadosArtista{
    char *nome;
    char *estilo;
    int qtd_albuns;
    struct NoRB *albuns;  
} DadosArtista;

typedef enum {
    NO_ARTISTA,
    NO_ALBUM
} TipoNo;

typedef struct NoRB {
    TipoNo tipo;
    union {
        DadosArtista artista;
        DadosAlbum album;
    } dado;
    int cor;
    struct NoRB *esq;
    struct NoRB *dir;
    struct NoRB *pai;
} NoRB;




// ---------------- Protótipos ---------------- 
// Funções genéricas para árvore rubro-negra
void rotacaoDireita(NoRB** raiz);
void rotacaoEsquerda(NoRB** raiz);
NoRB *criarNo(void *dado, TipoNo tipo, NoRB* pai);
void troca_cor(NoRB* no);
void balancearRubroNegra(NoRB** no);
void inserirNo(NoRB **raiz, void *dado, TipoNo tipo, NoRB* pai,int* flag);
void imprimirArvore(NoRB *raiz);
NoRB* buscar_item(NoRB* raiz, char* nome);
void exibe_dados(NoRB* no);

//Album   *criarAlbum(const char *titulo, int ano);
//*Musica  *criarMusica(const char *titulo, int minutos);

//void liberarMusicas(Musica *lista);
//void liberarAlbuns(Album *raiz);
//void liberarArtistas(Artista *raiz);

// Inserção 
//Album   *inserirAlbum(Album *raiz, Album *novo);
//Musica  *inserirMusica(Musica *lista, Musica *nova);

 //Busca 
//Artista *buscarArtista(Artista *raiz, const char *nome);
//Album   *buscarAlbum(Album *raiz, const char *titulo);
//Musica  *buscarMusica(Musica *lista, const char *titulo);

 //Remoção 
//Artista *removerArtista(Artista *raiz, const char *nome);
//Album   *removerAlbum(Album *raiz, const char *titulo);
//Musica  *removerMusica(Musica *lista, const char *titulo);

 //Impressão 
//void imprimirAlbuns(Album *raiz);
//void imprimirMusicas(Musica *lista);

 //Operações auxiliares da árvore rubro-negra 
//void rotacaoEsquerdaArtista(Artista **raiz, Artista *x);
//void rotacaoDireitaArtista(Artista **raiz, Artista *y);
//void corrigirInsercaoArtista(Artista **raiz, Artista *z);

//void rotacaoEsquerdaAlbum(Album **raiz, Album *x);
//void rotacaoDireitaAlbum(Album **raiz, Album *y);
//void corrigirInsercaoAlbum(Album **raiz, Album *z);

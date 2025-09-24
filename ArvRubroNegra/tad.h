#ifndef TAD_H
#define TAD_H

#define MAX_NOME   100
#define MAX_TITULO 150
#define MAX_ESTILO 50

#define VERMELHO 0
#define PRETO    1

/* ---------------- Estruturas ---------------- */

/* Lista encadeada de músicas */
typedef struct Musica {
    char titulo[MAX_TITULO];
    int minutos;
    struct Musica *prox;
} Musica;

/* Árvore Rubro-Negra de Álbuns */
typedef struct Album {
    char titulo[MAX_TITULO];
    int ano;
    int qtd_musicas;
    Musica *musicas;       // lista encadeada ordenada de músicas

    int cor;
    struct Album *esq;
    struct Album *dir;
    struct Album *pai;
} Album;

/* Árvore Rubro-Negra de Artistas */
typedef struct Artista {
    char nome[MAX_NOME];
    char estilo[MAX_ESTILO];
    int qtd_albuns;
    Album *albuns;         // raiz da RB de álbuns do artista

    int cor;
    struct Artista *esq;
    struct Artista *dir;
    struct Artista *pai;
} Artista;

/* ---------------- Protótipos ---------------- */

/* Criação e destruição */
Artista *criarArtista(const char *nome, const char *estilo);
Album   *criarAlbum(const char *titulo, int ano);
Musica  *criarMusica(const char *titulo, int minutos);

void liberarMusicas(Musica *lista);
void liberarAlbuns(Album *raiz);
void liberarArtistas(Artista *raiz);

/* Inserção */
Artista *inserirArtista(Artista *raiz, Artista *novo);
Album   *inserirAlbum(Album *raiz, Album *novo);
Musica  *inserirMusica(Musica *lista, Musica *nova);

/* Busca */
Artista *buscarArtista(Artista *raiz, const char *nome);
Album   *buscarAlbum(Album *raiz, const char *titulo);
Musica  *buscarMusica(Musica *lista, const char *titulo);

/* Remoção */
Artista *removerArtista(Artista *raiz, const char *nome);
Album   *removerAlbum(Album *raiz, const char *titulo);
Musica  *removerMusica(Musica *lista, const char *titulo);

/* Impressão */
void imprimirArtistas(Artista *raiz);
void imprimirAlbuns(Album *raiz);
void imprimirMusicas(Musica *lista);

/* Operações auxiliares da árvore rubro-negra */
void rotacaoEsquerdaArtista(Artista **raiz, Artista *x);
void rotacaoDireitaArtista(Artista **raiz, Artista *y);
void corrigirInsercaoArtista(Artista **raiz, Artista *z);

void rotacaoEsquerdaAlbum(Album **raiz, Album *x);
void rotacaoDireitaAlbum(Album **raiz, Album *y);
void corrigirInsercaoAlbum(Album **raiz, Album *z);

#endif

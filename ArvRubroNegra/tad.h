
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
    char* minutos;
    struct Musica *prox;
} Musica;



typedef struct DadosAlbum{
    char *nome;
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

typedef union {
        DadosArtista artista;
        DadosAlbum album;
} dado;

typedef struct NoRB {
    TipoNo tipo;
    dado dado;
    int cor;
    struct NoRB *esq;
    struct NoRB *dir;
    struct NoRB *pai;
} NoRB;




// ---------------- Protótipos ---------------- 
// Funções genéricas para árvore rubro-negra
void libera_musicas(Musica* lista);
void libera_albuns(NoRB* album);
void rotacaoDireita(NoRB** raiz);
void rotacaoEsquerda(NoRB** raiz);
NoRB *criarNo(void *dado, TipoNo tipo, NoRB* pai);
int eh_folha(NoRB* NO);
void troca_cor(NoRB* no);
int cor(NoRB* no);
void balancearRubroNegra(NoRB** no);
void inserirNo(NoRB **raiz, void *dado, TipoNo tipo, NoRB* pai,int* flag);
void imprimirArvore(NoRB *raiz);
NoRB* buscar_item(NoRB* raiz, char* nome);
void exibe_dados(NoRB* no);
void move2esqRED(NoRB** h);
void move2dirRED(NoRB** h);
int remove_ArvRB(NoRB **raiz,char *nome);
void copiar_dados(NoRB* destino,NoRB* origem);
void Procura_menor(NoRB* no,NoRB** menor);
void libera_conteudo_no(NoRB* n);
void remover_Menor_Direita(NoRB** NO);
void remove_NO(NoRB** raiz, char* nome);


//Musicas.c
Musica* criar_musica(char* titulo,char* minutos);
void cadastra_musica(Musica** lista,char* titulo,char* duracao,int *flag);
void exibe_musicas(Musica* lista);
Musica* Busca_musicas(Musica* lista,char* nome);
void remove_musicas(Musica** lista,char *titulo,int* flag);

//utils.c
char confirma_desejo_de_remover_artista(char* nome);
char confirma_desejo_de_remover_album(char* nome);
void deixa_string_minuscula(char *s);
void ler_dados_artista(DadosArtista* artista,char* nome,char* estilo);
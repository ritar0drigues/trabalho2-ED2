#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



void deixa_string_minuscula(char *s) {
    for (int i = 0; s[i]; i++) {
        s[i] = tolower((unsigned char)s[i]);
    }
}

void ler_dados_artista(dado* info, char* nome, char* estilo){
    printf("Nome do artista: ");
    scanf(" %49[^\n]", nome);
    getchar(); // consome o '\n' sobrando
    printf("Estilo do artista: ");
    scanf(" %49[^\n]", estilo);
    getchar();
    
    deixa_string_minuscula(nome);
    deixa_string_minuscula(estilo);

    // copia para os arrays do struct
    strncpy(info->artista.nome, nome, 49);
    info->artista.nome[49] = '\0';
    strncpy(info->artista.estilo, estilo, 49);
    info->artista.estilo[49] = '\0';

    info->artista.qtd_albuns = 0;
    info->artista.albuns = NULL;
}

void ler_dados_album(dado* info, char* nome, int *ano){
    printf("Informe o nome do album:\n");
    scanf(" %49[^\n]", nome);
    getchar(); // consome o '\n'
    printf("Informe o ano do album:\n");
    scanf("%d", ano);
    getchar(); // consome o '\n' sobrando

    // copia para os arrays do struct
    strncpy(info->album.nome, nome, 49);
    info->album.nome[49] = '\0';
    info->album.ano = *ano;

    info->album.qtd_musicas = 0;
    info->album.musicas = NULL;
}


char confirma_desejo_de_remover_artista(char* nome){
    char opcao;
    printf("Se você remover o artista %s todos os albuns juntamente com suas musicas serão removidos.\n",nome);
    printf("Tem certeza que deseja remove-lo.(1-sim|2-nao)\n");
    do{
        scanf(" %c", &opcao);
        if(opcao != '1' && opcao != '2'){
            printf("Erro!! Informe uma opcao válida.\n");
        }
    }while(opcao!= '1' && opcao!='2');
    return opcao;
}

char confirma_desejo_de_remover_album(char* nome){
    char opcao;
    printf("Se você remover o album %s todos as musicas do album também serão removidas.\n",nome);
    printf("Tem certeza que deseja remove-lo.(1-sim|2-nao)\n");
    do{
        scanf(" %c", &opcao);
        if(opcao != '1' && opcao != '2'){
            printf("Erro!! Informe uma opcao válida.\n");
        }
    }while(opcao!= '1' && opcao!='2');
    return opcao;
}

int validar_estilo(const char* estilo){
    const char* estilos_validos[] = {"rock", "sertanejo", "funk", "a"};
    int num_estilos = 4;
    int valido = 0;
    int i = 0;
    while(valido!=1 && i<num_estilos){
        if (strcasecmp(estilo, estilos_validos[i]) == 0) {
            valido = 1;
        }
        i++;
    }
    return valido;
}


int validarFormatoduracao(const char* duracao) {
    int valido = 1;
    if (duracao == NULL || strlen(duracao) != 5) {
        valido = 0;
    }
    else{
        if (duracao[2] != ':') {
            valido = 0;
        }
        else{
            for (int i = 0; i < 5; i++) {
                if (i != 2) { 
                    if (duracao[i] < '0' || duracao[i] > '9') {
                        valido = 0;
                    }
                }
            }
            if(valido){
                int minutos = (duracao[0] - '0') * 10 + (duracao[1] - '0');
                int segundos = (duracao[3] - '0') * 10 + (duracao[4] - '0');
                if (minutos < 0 || minutos > 59) {
                    valido = 0;
                }
                else if (segundos < 0 || segundos > 59) {
                    valido = 0;
                }
            }
        }
    }
    return valido;
}

// ...existing code...

#include <ctype.h>

// compara as sequências iniciais de dígitos em a e b
// retorna -1,0,1 conforme a parte numérica de 'a' é menor, igual ou maior que a de 'b'.
// se end_a/end_b não forem NULL, escreve neles o ponteiro para o primeiro caractere após a sequência de dígitos.
static int compare_digit_runs(const char *a, const char *b, const char **end_a, const char **end_b)
{
    const char *pa = a;
    const char *pb = b;

    while (*pa && isdigit((unsigned char)*pa)) pa++;
    while (*pb && isdigit((unsigned char)*pb)) pb++;

    size_t la = (size_t)(pa - a);
    size_t lb = (size_t)(pb - b);

    if (end_a) *end_a = pa;
    if (end_b) *end_b = pb;

    if (la == 0 && lb == 0) return 0;   // nenhum tem prefixo numérico
    if (la == 0) return 1;             // a não tem número, b tem -> trata b como "menor" (numéricos antes)
    if (lb == 0) return -1;            // a tem número, b não -> a é "menor"

    // ambos têm prefixo numérico: comparar comprimento (mais dígitos -> maior número)
    if (la < lb) return -1;
    if (la > lb) return 1;

    // mesmos comprimentos: comparar dígito a dígito (usar ASCII é equivalente a comparar valores numéricos dos dígitos)
    for (size_t i = 0; i < la; ++i) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[i];
        if (ca < cb) return -1;
        if (ca > cb) return 1;
    }
    return 0; // prefixos numéricos idênticos
}

// Exemplo de integração em cmp_keys (substituir a lógica atual por isso):
// ...existing code...
#include <ctype.h>
// ...existing code...

int cmp_keys(const char *a, const char *b)
{
    if (a == b) return 0;
    if (!a) return -1;
    if (!b) return 1;

    const unsigned char *pa = (const unsigned char *)a;
    const unsigned char *pb = (const unsigned char *)b;

    while (*pa && *pb) {
        if (isdigit(*pa) && isdigit(*pb)) {
            const unsigned char *ea = pa, *eb = pb;
            while (isdigit(*ea)) ea++;
            while (isdigit(*eb)) eb++;

            size_t la = (size_t)(ea - pa);
            size_t lb = (size_t)(eb - pb);

            if (la < lb) return -1;
            if (la > lb) return 1;

            for (size_t i = 0; i < la; ++i) {
                if (pa[i] < pb[i]) return -1;
                if (pa[i] > pb[i]) return 1;
            }

            pa = ea;
            pb = eb;
            continue;
        }

        if (*pa != *pb) return (*pa < *pb) ? -1 : 1;

        pa++; pb++;
    }

    if (*pa) return 1;
    if (*pb) return -1;
    return 0;
}
// ...existing code...

// ...existing code...
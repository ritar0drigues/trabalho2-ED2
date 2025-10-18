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

void ler_dados_artista(DadosArtista* artista,char* nome,char* estilo){
    printf("Nome do artista: ");
    scanf("%49[^\n]", nome);
    getchar();
    printf("Estilo do artista: ");
    scanf("%49[^\n]", estilo);
    getchar();
    deixa_string_minuscula(nome);
    deixa_string_minuscula(estilo);
    (*artista).nome = strdup(nome);
    (*artista).estilo = strdup(estilo);
    (*artista).qtd_albuns = 0;
    (*artista).albuns = NULL;
}

void ler_dados_album(DadosAlbum* album, char* nome, int* ano){
    printf("Informe o  nome do album:\n");
    scanf(" %49[^\n]",nome);
    printf("Informe o ano do album:\n");
    scanf("%d", ano);
    album->titulo = strdup(nome);
    album->ano = *ano;
    album->qtd_musicas = 0;
    album->musicas = NULL;
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
    const char* estilos_validos[] = {"rock", "sertanejo", "funk"};
    int num_estilos = 3;
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
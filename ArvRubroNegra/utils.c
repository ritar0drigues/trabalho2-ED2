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
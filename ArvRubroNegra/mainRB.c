#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tad.h"

int main(){
    NoRB* raiz = NULL;
    int op = 0;
    do{
        printf("____________________\n");
        printf("|MENU               |\n");
        printf("|1-Cadastrar Artista|\n");
        printf("|2-Listar Artistas  |\n");
        printf("|3-Buscar           |\n");
        printf("|4-Remover artista  |\n");
        printf("|5-Sair             |\n");
        printf("|___________________|\n");
        scanf("%d", &op);
        getchar();
        switch (op)
        {
        case 1:{
            DadosArtista artista;
            char nome[MAX_NOME];
            char estilo[MAX_ESTILO];
            printf("Nome do artista: ");
            scanf("%49[^\n]", nome);
            getchar();
            printf("Estilo do artista: ");
            scanf("%49[^\n]", estilo);
            getchar();
            int flag = 0;
            artista.nome = strdup(nome);
            artista.estilo = strdup(estilo);
            artista.qtd_albuns = 0;
            artista.albuns = NULL;
            inserirNo(&raiz, &artista, NO_ARTISTA, NULL,&flag);
            if(flag){
                printf("Artista cadastrado com sucesso.\n");
            }
            if(raiz){
                raiz->cor = PRETO;
            }
            // liberar memória temporária
            free(artista.nome);
            free(artista.estilo);
            break;
        }
        case 2:{
            if(raiz==NULL){
                printf("Nenhum artista cadastrado.\n");
            }
            else{
                imprimirArvore(raiz);
            }
            break;
        }
        case 3:{
            char nome[50];
            printf("Informe o nome do artista que deseja buscar.\n");
            scanf("%49[^\n]", nome);
            NoRB* resultado = buscar_item(raiz, nome);
            if(resultado){
                exibe_dados(resultado);
            }
            else{
                printf("Artista %s não encontrado.\n", nome);
            }
            break;
        }
        case 4:{
            char nome[50];
            printf("Informe o nome do artista que deseja remover.\n");
            scanf("%49[^\n]", nome);
            int opcao = '0';
            do{
                printf("Se você remover o artista %s todos os albuns juntamente com suas musicas serão removidos.\n",nome);
                printf("Tem certeza que deseja remove-lo.(1-sim\2-nao)\n");
                scanf(" %c", op);
            }while(opcao!= '1' && opcao!='2');
            if(opcao=='1'){
                
            }
            else{
                printf("Remocao cancelada.\n");
            }
            break;
        }
        case 5:{
            printf("Encerrando...\n");
            break;
        }
        default:
            printf("Erro!!Informe uma opção válida.\n");
            break;
        }
    }while(op != 5);
    return 0;
}
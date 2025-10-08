#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tad.h"

void menu_musicas(Musica** lista){
     int op = 0;
    do{
        printf(" ___________________\n");
        printf("|MENU               |\n");
        printf("|1-Cadastrar Música |\n");
        printf("|2-Listar musicas   |\n");
        printf("|3-Buscar musicas   |\n");
        printf("|4-Remover musicas  |\n");
        printf("|5-Sair             |\n");
        printf("|___________________|\n");
        scanf("%d", &op);
        getchar();
        switch (op)
        {
            case 1:{
                char titulo[50];
                char duracao[50];
                printf("Informe o nome da musica que deseja cadastrar.\n");
                scanf("%49[^\n]",titulo);
                getchar();
                printf("Informe a duracao da música.\n");
                scanf("%49[^\n]",duracao);
                getchar();
                int flag = 0;
                cadastra_musica(lista,titulo,duracao,&flag);
                if(flag){
                    printf("Musica cadastrada.\n");
                }
                else{
                    printf("Erro!!!Música não cadastrada.\n");
                }
                break;
            }
            case 2:{
                if((*lista)){
                    exibe_musicas((*lista));
                }
                else{
                    printf("Nenhuma música cadastrada nesse albúm.\n");
                }
                break;
            }
            case 3:{
                if(*lista){
                    char titulo[50];
                    printf("Informe o nome da musica que deseja buscar: \n");
                    scanf("%49[^\n]", titulo);
                    getchar();
                    Musica* result = Busca_musicas(*lista,titulo);
                    if(result){
                        printf("Titulo: %s\n", result->titulo);
                        printf("Duracao: %s\n", result->minutos);
                    }
                    else{
                        printf("Musica não encontrada.\n");
                    }
                }
                else{
                    printf("Nenhuma música cadastrada nesse albúm.\n");
                }
                break;
            }
            case 4:{
                if(*lista){
                    char nome[50];
                    printf("Informe o nome da musica que deseja remover.\n");
                    scanf("%49[^\n]", nome);
                    getchar();
                    int flag = 0;
                    remove_musicas(lista,nome,&flag);
                    if(flag){
                        printf("Musica %s removida com sucesso.\n",nome);
                    }
                    else{
                        printf("Musica %s não encontrada.\n",nome);
                    }
                }
                else{
                    printf("Nenhuma musica cadastrada.\n");
                }
                break;
            }
            case 5:{
                printf("Voltando...");
                break;
            }
            default:
                printf("Erro!!Informe uma opção vallida.\n");
                break;
        }
    }while(op!=5);
}

void menu_albuns(NoRB** raiz){
    int op = 0;
    do{
        printf(" ___________________\n");
        printf("|MENU               |\n");
        printf("|1-Cadastrar Album  |\n");
        printf("|2-Listar Albuns    |\n");
        printf("|3-Buscar           |\n");
        printf("|4-Remover albuns   |\n");
        printf("|5-Editar album     |\n");
        printf("|6-Sair             |\n");
        printf("|___________________|\n");
        scanf("%d", &op);
        getchar();
        switch (op)
        {
            case 1:{
                    DadosAlbum album;
                    char nome[MAX_NOME];
                    printf("Titulo do album: ");
                    scanf("%49[^\n]", nome);
                    getchar();
                    deixa_string_minuscula(nome);
                    int flag = 0;
                    album.nome = strdup(nome);
                    album.ano = 2025;
                    album.qtd_musicas = 0;
                    album.musicas = NULL;
                    inserirNo(raiz, &album, NO_ALBUM, NULL,&flag);
                    if(flag){
                        printf("Album cadastrado com sucesso.\n");
                    }
                    if((*raiz)){
                        (*raiz)->cor = PRETO;
                    }
                    // liberar memória temporária
                    free(album.nome);
                break;
            }
            case 2:{
                if((*raiz)==NULL){
                    printf("Nenhum album cadastrado.\n");
                }
                else{
                    imprimirArvore(*raiz);
                }
                break;
            }
            case 3:{
                char nome[50];
                printf("Informe o nome do album que deseja buscar.\n");
                scanf("%49[^\n]", nome);
                getchar();
                NoRB* resultado = buscar_item((*raiz), nome);
                if(resultado){
                    exibe_dados(resultado);
                }
                else{
                    printf("Album %s não encontrado.\n", nome);
                }
                break;
            }
            case 4:{
                char nome[50];
                printf("Informe o nome do album que deseja remover.\n");
                scanf("%49[^\n]", nome);
                getchar();
                char opcao = confirma_desejo_de_remover_album(nome);

                if(opcao=='1'){
                    if(remove_ArvRB(raiz,nome)){
                        printf("Album %s removido com sucesso.\n", nome);
                    }
                    else{
                        printf("Album %s não encontrado.\n", nome);
                    }
                }
                else{
                    printf("Remocao cancelada.\n");
                }
                break;
            }
            case 5:{
                char album[50];
                printf("Informe o nome do album do qual deseja editar as músicas.\n");
                scanf("%49[^\n]", album);
                getchar();
                NoRB* result = buscar_item((*raiz),album);
                if(result){
                    menu_musicas(&(result->dado.album.musicas));
                }
                else{
                    printf("Album não encontrado para esse artista.\n");
                }
                break;
            }
            case 6:
                printf("voltando...");
                break;
        default:
                printf("ERRO!! Informe uma opcao válida.\n");
                break;
        }
    }while(op!=6);
}

int main(){
    NoRB* raiz = NULL;
    int op = 0;
    do{
        printf(" ___________________\n");
        printf("|MENU               |\n");
        printf("|1-Cadastrar Artista|\n");
        printf("|2-Listar Artistas  |\n");
        printf("|3-Buscar           |\n");
        printf("|4-Remover artista  |\n");
        printf("|5-editar albuns    |\n");
        printf("|6-Sair             |\n");
        printf("|___________________|\n");
        scanf("%d", &op);
        getchar();
        switch (op)
        {
        case 1:{
            DadosArtista artista;
            char nome[MAX_NOME];
            char estilo[MAX_ESTILO];
            int flag = 0;
            ler_dados_artista(&artista,nome,estilo);
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
            getchar();
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
            getchar();
            char opcao = confirma_desejo_de_remover_artista(nome);

            if(opcao=='1'){
                if(remove_ArvRB(&raiz,nome)){
                    printf("Artista %s removido(a) com sucesso.\n", nome);
                }
                else{
                    printf("Artista %s não encontrado(a).\n", nome);
                }
            }
            else{
                printf("Remocao cancelada.\n");
            }
            break;
        }
        case 5:{
            char nome[MAX_NOME];
            printf("Informe o nome do artista do qual deseja ver os albuns.\n");
            scanf("%49[^\n]", nome);
            getchar();
            NoRB* resultado = buscar_item(raiz,nome);
            if(resultado){
                menu_albuns(&resultado->dado.artista.albuns);
            }
            else{
                printf("Não existe nenhum artista de nome %s cadastrado.\n", nome);
            }
            break;
        }
        case 6:{
            printf("Encerrando...\n");
            break;
        }
        default:
            printf("Erro!!Informe uma opção válida.\n");
            break;
        }
    }while(op != 6);
    return 0;
}
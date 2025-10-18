/* album.c foi preservado, mas a função libera_albuns é implementada em
   FuncGenéricas2_3.c. Mantemos este arquivo como placeholder para
   organização do projeto. */

#include "tad.h"


int libera_albuns(NoRB* album){
    if(album){
        if(libera_albuns(album->esq)){
            libera_albuns(album->dir);
            libera_albuns(album->meio);
        }
        free(album->info1.album.nome);
        free(album->info2.album.nome);
        libera_musicas(album->info1.album.musicas);
        libera_musicas(album->info2.album.musicas);
        free(album);
    }
    return (album!=NULL);
}
/* wrapper ou implementação alternativa pode ser colocada aqui se necessário */
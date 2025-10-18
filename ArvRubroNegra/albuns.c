#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void libera_albuns(NoRB* album){
    if(album){
        libera_albuns(album->esq);
        libera_albuns(album->dir);
        free(album->dado.album.titulo);
        libera_musicas(album->dado.album.musicas);
        free(album);
    }
}
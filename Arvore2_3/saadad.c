void remove_info1_da_folha(NoRB** raiz, NoRB* pai, int n_da_info){
    if (!*raiz) return;

    if (n_da_info == 1) {
        if ((*raiz)->ninfos == 2) {
            /* desliza info2 para info1 (não remove o nó) */
            copia_dados(&(*raiz)->info1, (*raiz)->info2, (*raiz)->tipo);
            (*raiz)->ninfos = 1;
        } else {
            /* folha com 1 info: liberar nó e ajustar ponteiro no pai,
               depois pedir para consertar o pai (underflow handling centralizado). */
            if (pai == NULL) {
                /* raiz simples -> apenas liberar */
                free(*raiz);
                *raiz = NULL;
                return;
            }

            /* identificar posição do filho no pai */
            int pos = -1; /* 0 = esq, 1 = meio, 2 = dir */
            if (pai->esq == *raiz) pos = 0;
            else if (pai->meio == *raiz) pos = 1;
            else if (pai->dir == *raiz) pos = 2;

            /* desconecta do pai antes de liberar para evitar ponteiros pendentes */
            if (pos == 0) pai->esq = NULL;
            else if (pos == 1) pai->meio = NULL;
            else if (pos == 2) pai->dir = NULL;

            free(*raiz);
            *raiz = NULL;

            /* pedir para consertar apenas o pai localmente */
            if (pos != -1) {
                fix_parent_after_child_underflow(pai, pos);
            }
        }
    }
    else if (n_da_info == 2) {
        if ((*raiz)->ninfos == 2) {
            (*raiz)->ninfos = 1;
        }
    }
}
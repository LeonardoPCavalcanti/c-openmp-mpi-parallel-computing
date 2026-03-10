#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Estrutura de um nó da lista encadeada
typedef struct No {
    char nome_arquivo[64];
    struct No* proximo;
} No;

// Cria um novo nó com o nome do arquivo
No* criarNo(const char* nome_arquivo) {
    No* novo = (No*)malloc(sizeof(No));
    if (!novo) { perror("malloc"); exit(EXIT_FAILURE); }
    snprintf(novo->nome_arquivo, sizeof(novo->nome_arquivo), "%s", nome_arquivo);
    novo->proximo = NULL;
    return novo;
}

// Insere o nó no final da lista
void inserirNo(No** cabeca, const char* nome_arquivo) {
    No* novo = criarNo(nome_arquivo);
    if (*cabeca == NULL) {
        *cabeca = novo;
    } else {
        No* temp = *cabeca;
        while (temp->proximo) temp = temp->proximo;
        temp->proximo = novo;
    }
}

// Processa o nó (imprime o nome e a thread executora)
void processarNo(No* no) {
    printf("Processando: %-12s | thread %d\n",
           no->nome_arquivo, omp_get_thread_num());
}

// Libera a memória alocada para a lista
void liberarLista(No** cabeca) {
    No* atual = *cabeca;
    while (atual) {
        No* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    *cabeca = NULL;
}

int main(void) {
    No* cabeca = NULL;
    const char* pokemons[10] = {
        "Pikachu.txt","Bulbasaur.txt","Charmander.txt","Squirtle.txt","Jigglypuff.txt",
        "Meowth.txt","Psyduck.txt","Eevee.txt","Snorlax.txt","Mewtwo.txt"
    };

    // Monta a lista com os nomes de arquivos (Pokémons)
    for (int i = 0; i < 10; i++) {
        inserirNo(&cabeca, pokemons[i]);
    }

    // Região paralela: single cria as tarefas, threads executam
    #pragma omp parallel
    {
        #pragma omp single
        {
            No* atual = cabeca;
            while (atual != NULL) {
                No* copia = atual;
                #pragma omp task firstprivate(copia)
                {
                    processarNo(copia);
                }
                atual = atual->proximo;
            }
            #pragma omp taskwait
        }
    }

    liberarLista(&cabeca);
    return 0;
}

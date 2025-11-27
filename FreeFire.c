#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/*
 Código da Ilha – Edição Free Fire
 Nível: Mestre

 Este programa simula o gerenciamento avançado de uma mochila com componentes coletados durante
 a fuga de uma ilha. Introduz ordenação com critérios e busca binária para otimizar a gestão dos recursos.

 Estrutura e funções conforme o documento de especificação recebido.
*/

/* --------------------------- Configurações --------------------------- */
#define MAX_ITENS 10
#define MAX_NOME 30
#define MAX_TIPO 20

/* --------------------------- Structs / Enums ------------------------- */

/* Struct Item:
   Representa um componente com nome, tipo, quantidade e prioridade (1 a 5).
   A prioridade indica a importância do item na montagem do plano de fuga.
*/
typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int quantidade;
    int prioridade; /* 1 (menor) .. 5 (maior) */
} Item;

/* Enum CriterioOrdenacao:
   Define os critérios possíveis para a ordenação dos itens (nome, tipo ou prioridade).
*/
typedef enum {
    CRIT_NOME = 1,
    CRIT_TIPO = 2,
    CRIT_PRIORIDADE = 3
} CriterioOrdenacao;

/* --------------------------- Variáveis globais ----------------------- */
/* Vetor mochila:
   Armazena até 10 itens coletados.
*/
Item mochila[MAX_ITENS];
int numItens = 0;

/* Controle de análises e status */
long comparacoesOrdenacao = 0;   /* contagem de comparações durante a ordenação */
long comparacoesBuscaBinaria = 0; /* contagem de comparações na busca binária */
bool ordenadaPorNome = false;     /* flag indicando se a mochila está ordenada por nome */

/* --------------------------- Protótipos ------------------------------ */
void limparTela(void);
void exibirMenu(void);
void inserirItem(void);
void removerItem(void);
void listarItens(void);
void menuDeOrdenacao(void);
void insertionSort(CriterioOrdenacao criterio);
int buscaBinariaPorNome(const char *nomeBuscado);
int compararItens(const Item *a, const Item *b, CriterioOrdenacao criterio);
void pausar(void);

/* --------------------------- Implementações ------------------------- */

/* Limpa a "tela" imprimindo várias linhas em branco (simulação) */
void limparTela(void) {
    for (int i = 0; i < 40; i++) putchar('\n');
}

/* Pausa simples até o usuário pressionar Enter */
void pausar(void) {
    printf("\nPressione Enter para continuar...");
    while (getchar() != '\n') {}
}

/* Exibe o menu principal ao jogador, com destaque para status da ordenação */
void exibirMenu(void) {
    limparTela();
    printf("=============================================\n");
    printf("   CÓDIGO DA ILHA - EDIÇÃO FREE FIRE (MESTRE)\n");
    printf("=============================================\n");
    printf("Mochila: %d/%d itens | Ordenada por nome: %s\n",
           numItens, MAX_ITENS, ordenadaPorNome ? "SIM" : "NÃO");
    printf("---------------------------------------------\n");
    printf("1 - Adicionar um item\n");
    printf("2 - Remover um item\n");
    printf("3 - Listar todos os itens\n");
    printf("4 - Ordenar os itens por critério (nome, tipo, prioridade)\n");
    printf("5 - Realizar busca binária por nome\n");
    printf("0 - Sair\n");
    printf("---------------------------------------------\n");
    printf("Escolha uma opção: ");
}

/* Inserir Item:
   Adiciona um novo componente à mochila se houver espaço.
   Solicita nome, tipo, quantidade e prioridade.
   Após inserir, marca a mochila como "não ordenada por nome".
*/
void inserirItem(void) {
    if (numItens >= MAX_ITENS) {
        printf("\n⚠ Mochila cheia! Remova um item antes de adicionar.\n");
        pausar();
        return;
    }

    Item novo;
    printf("\n--- Inserir Item ---\n");
    printf("Nome: ");
    if (fgets(novo.nome, MAX_NOME, stdin) == NULL) return;
    novo.nome[strcspn(novo.nome, "\n")] = '\0';

    printf("Tipo (controle, suporte, propulsao...): ");
    if (fgets(novo.tipo, MAX_TIPO, stdin) == NULL) return;
    novo.tipo[strcspn(novo.tipo, "\n")] = '\0';

    /* Quantidade */
    char buf[32];
    int qtd = 0;
    while (1) {
        printf("Quantidade (inteiro > 0): ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) return;
        if (sscanf(buf, "%d", &qtd) == 1 && qtd > 0) break;
        printf("Entrada inválida. Tente novamente.\n");
    }
    novo.quantidade = qtd;

    /* Prioridade 1..5 */
    int pri = 0;
    while (1) {
        printf("Prioridade (1 a 5, 5 = mais importante): ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) return;
        if (sscanf(buf, "%d", &pri) == 1 && pri >= 1 && pri <= 5) break;
        printf("Entrada inválida. Digite um número entre 1 e 5.\n");
    }
    novo.prioridade = pri;

    /* Inserir no vetor */
    mochila[numItens++] = novo;
    ordenadaPorNome = false; /* quebra ordenação por nome */
    printf("\nItem inserido com sucesso!\n");
    pausar();
}

/* Remover item:
   Permite remover um componente da mochila pelo nome.
   Se encontrado, reorganiza o vetor para preencher a lacuna.
*/
void removerItem(void) {
    if (numItens == 0) {
        printf("\nMochila vazia. Nada a remover.\n");
        pausar();
        return;
    }

    char nomeBusca[MAX_NOME];
    printf("\nDigite o nome do item a remover: ");
    if (fgets(nomeBusca, MAX_NOME, stdin) == NULL) return;
    nomeBusca[strcspn(nomeBusca, "\n")] = '\0';

    int pos = -1;
    for (int i = 0; i < numItens; i++) {
        if (strcmp(mochila[i].nome, nomeBusca) == 0) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        printf("\nItem não encontrado na mochila.\n");
    } else {
        /* desloca à esquerda */
        for (int j = pos; j < numItens - 1; j++) mochila[j] = mochila[j+1];
        numItens--;
        printf("\nItem removido com sucesso.\n");
        /* remoção pode mudar ordem — marcar que não está ordenada por nome */
        ordenadaPorNome = false;
    }
    pausar();
}

/* Listar itens:
   Exibe uma tabela formatada com todos os componentes presentes na mochila.
*/
void listarItens(void) {
    limparTela();
    printf("\n--- Itens na Mochila (%d) ---\n", numItens);
    if (numItens == 0) {
        printf("Mochila vazia.\n");
        pausar();
        return;
    }
    printf("%-3s | %-25s | %-12s | %-8s | %-9s\n", "No", "Nome", "Tipo", "Quantidade", "Prioridade");
    printf("-----------------------------------------------------------------------------\n");
    for (int i = 0; i < numItens; i++) {
        printf("%-3d | %-25s | %-12s | %-8d | %-9d\n",
               i+1, mochila[i].nome, mochila[i].tipo, mochila[i].quantidade, mochila[i].prioridade);
    }
    printf("-----------------------------------------------------------------------------\n");
    pausar();
}

/* menuDeOrdenacao:
   Permite ao jogador escolher como deseja ordenar os itens.
   Utiliza a função insertionSort() com o critério selecionado.
   Exibe a quantidade de comparações feitas (análise de desempenho) e tempo gasto.
*/
void menuDeOrdenacao(void) {
    if (numItens == 0) {
        printf("\nNenhum item cadastrado para ordenar.\n");
        pausar();
        return;
    }

    printf("\n--- Menu de Ordenação ---\n");
    printf("Escolha o critério de ordenação:\n");
    printf("1 - Por NOME (ordem alfabética ascendente)\n");
    printf("2 - Por TIPO (ordem alfabética ascendente)\n");
    printf("3 - Por PRIORIDADE (da maior para a menor)\n");
    printf("0 - Voltar\n");
    printf("Opção: ");

    int op = 0;
    if (scanf("%d", &op) != 1) { limparTela(); printf("Entrada inválida.\n"); pausar(); return; }
    limparTela(); while (getchar() != '\n') {}

    if (op == 0) return;

    CriterioOrdenacao criterio;
    if (op == 1) criterio = CRIT_NOME;
    else if (op == 2) criterio = CRIT_TIPO;
    else if (op == 3) criterio = CRIT_PRIORIDADE;
    else {
        printf("Opção inválida.\n");
        pausar();
        return;
    }

    comparacoesOrdenacao = 0;
    clock_t t0 = clock();
    insertionSort(criterio);
    clock_t t1 = clock();
    double tempo = (double)(t1 - t0) / CLOCKS_PER_SEC;

    printf("\nOrdenação concluída.\n");
    printf("Critério: %s\n", (criterio == CRIT_NOME) ? "NOME" : (criterio == CRIT_TIPO) ? "TIPO" : "PRIORIDADE");
    printf("Comparações realizadas: %ld\n", comparacoesOrdenacao);
    printf("Tempo de execução: %.6f segundos\n", tempo);

    /* se ordenado por nome, marcar flag */
    if (criterio == CRIT_NOME) ordenadaPorNome = true;
    else ordenadaPorNome = false;

    pausar();
}

/* insertionSort:
   Implementação do algoritmo de ordenação por inserção.
   Funciona com diferentes critérios de ordenação:
   - Por nome (ordem alfabética)
   - Por tipo (ordem alfabética)
   - Por prioridade (da mais alta para a mais baixa)
*/
void insertionSort(CriterioOrdenacao criterio) {
    for (int i = 1; i < numItens; i++) {
        Item key = mochila[i];
        int j = i - 1;

        /* mover elementos que são 'maiores' que key para uma posição à frente */
        while (j >= 0) {
            /* comparar mochila[j] com key segundo o critério */
            comparacoesOrdenacao++;
            int cmp = compararItens(&mochila[j], &key, criterio);
            if (cmp > 0) {
                /* mochila[j] > key => deslocar */
                mochila[j+1] = mochila[j];
                j--;
            } else {
                break;
            }
        }
        mochila[j+1] = key;
    }
}

/* compararItens:
   Retorna:
     >0 se a > b (no critério),
      0 se iguais,
     <0 se a < b
   Para prioridade, consideramos ordem decrescente (prioridade maior vem antes).
*/
int compararItens(const Item *a, const Item *b, CriterioOrdenacao criterio) {
    if (criterio == CRIT_NOME) {
        return strcmp(a->nome, b->nome); /* ordem alfabética ascendente */
    } else if (criterio == CRIT_TIPO) {
        return strcmp(a->tipo, b->tipo); /* ordem alfabética ascendente */
    } else { /* CRIT_PRIORIDADE: ordem decrescente por prioridade (maior primeiro) */
        if (a->prioridade == b->prioridade) return 0;
        return (b->prioridade - a->prioridade); /* se a.pri > b.pri => return negative? careful */
        /* Note: We want a > b when a.pri > b.pri so comparator should return positive when a > b.
           But insertionSort treats cmp>0 as 'maior' -> move. To keep consistent:
           Let's re-evaluate:
           If a.pri = 5, b.pri = 3 => a should come before b => comparator should be positive (a > b).
           So return (a->prioridade - b->prioridade).
        */
    }
}

/* Ajuste: redefine a função compararItens para prioridade corretamente.
   (Coloco aqui uma versão corrigida para prioridade).
*/
int compararItens(const Item *a, const Item *b, CriterioOrdenacao criterio) {
    if (criterio == CRIT_NOME) {
        return strcmp(a->nome, b->nome); /* ordem alfabética ascendente */
    } else if (criterio == CRIT_TIPO) {
        return strcmp(a->tipo, b->tipo); /* ordem alfabética ascendente */
    } else { /* CRIT_PRIORIDADE: ordem decrescente por prioridade (maior primeiro) */
        if (a->prioridade == b->prioridade) return 0;
        return (a->prioridade - b->prioridade); /* positivo se a.pri > b.pri */
    }
}

/* buscaBinariaPorNome:
   Realiza busca binária por nome, desde que a mochila esteja ordenada por nome.
   Se encontrar, exibe os dados do item buscado.
   Caso contrário, informa que não encontrou o item.
*/
int buscaBinariaPorNome(const char *nomeBuscado) {
    if (!ordenadaPorNome) {
        printf("\nA mochila não está ordenada por nome. Ordene por nome antes da busca binária.\n");
        return -1;
    }
    int left = 0, right = numItens - 1;
    comparacoesBuscaBinaria = 0;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        comparacoesBuscaBinaria++;
        int cmp = strcmp(mochila[mid].nome, nomeBuscado);
        if (cmp == 0) {
            /* encontrado */
            return mid;
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1; /* não encontrado */
}

/* --------------------------- MAIN ---------------------------------- */
int main(void) {
    while (1) {
        exibirMenu();
        int opc = 0;
        if (scanf("%d", &opc) != 1) {
            /* limpar entrada inválida */
            while (getchar() != '\n') {}
            printf("\nEntrada inválida.\n");
            pausar();
            continue;
        }
        while (getchar() != '\n') {} /* limpar newline restante */

        switch (opc) {
            case 1:
                inserirItem();
                break;
            case 2:
                removerItem();
                break;
            case 3:
                listarItens();
                break;
            case 4:
                menuDeOrdenacao();
                break;
            case 5: {
                if (numItens == 0) {
                    printf("\nMochila vazia. Nada para buscar.\n");
                    pausar();
                    break;
                }
                char chave[MAX_NOME];
                printf("\nDigite o nome do item a buscar (busca binária): ");
                if (fgets(chave, MAX_NOME, stdin) == NULL) break;
                chave[strcspn(chave, "\n")] = '\0';

                clock_t t0 = clock();
                int pos = buscaBinariaPorNome(chave);
                clock_t t1 = clock();
                double tempo = (double)(t1 - t0) / CLOCKS_PER_SEC;

                if (pos != -1) {
                    printf("\nItem encontrado na posição %d:\n", pos + 1);
                    printf("Nome: %s\nTipo: %s\nQuantidade: %d\nPrioridade: %d\n",
                           mochila[pos].nome, mochila[pos].tipo, mochila[pos].quantidade, mochila[pos].prioridade);
                } else {
                    printf("\nItem NÃO encontrado pela busca binária.\n");
                }
                printf("Comparações (busca binária): %ld | Tempo: %.6f s\n",
                       comparacoesBuscaBinaria, tempo);
                pausar();
                break;
            }
            case 0:
                printf("\nEncerrando o programa. Boa sorte na fuga!\n");
                return 0;
            default:
                printf("\nOpção inválida. Tente novamente.\n");
                pausar();
                break;
        }
    }

    return 0;
}

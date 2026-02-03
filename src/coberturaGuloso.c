#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <math.h>

#define MAX_PONTOS 1000
#define MAX_INTERVALOS 1000
#define MAX_PATH 1024

typedef struct
{
    int inicio;
    int fim;
} Intervalo;

typedef struct
{
    int id;
    int posicao;
} Ponto;

typedef struct
{
    Intervalo *intervalos;
    int n_intervalos;
    Ponto *pontos;
    int n_pontos;
    int *pontos_cobertos;
    int n_pontos_cobertos;
    Intervalo *solucao;
    int n_solucao;
    double tempo_execucao;
    long memoria_utilizada;
    double qualidade;
} Problema;

typedef struct
{
    double tempo;
    long memoria;
    double qualidade;
    int n_solucao;
} Metricas;

void inicializar_problema(Problema *problema)
{
    problema->intervalos = NULL;
    problema->pontos = NULL;
    problema->pontos_cobertos = NULL;
    problema->solucao = NULL;
    problema->n_intervalos = 0;
    problema->n_pontos = 0;
    problema->n_pontos_cobertos = 0;
    problema->n_solucao = 0;
    problema->tempo_execucao = 0.0;
    problema->memoria_utilizada = 0;
    problema->qualidade = 0.0;
}

void liberar_problema(Problema *problema)
{
    if (problema->intervalos != NULL)
    {
        free(problema->intervalos);
        problema->intervalos = NULL;
    }
    if (problema->pontos != NULL)
    {
        free(problema->pontos);
        problema->pontos = NULL;
    }
    if (problema->pontos_cobertos != NULL)
    {
        free(problema->pontos_cobertos);
        problema->pontos_cobertos = NULL;
    }
    if (problema->solucao != NULL)
    {
        free(problema->solucao);
        problema->solucao = NULL;
    }
}

int comparar_intervalos(const void *a, const void *b)
{
    int resultado = 0;
    Intervalo *intervalo_a = (Intervalo *)a;
    Intervalo *intervalo_b = (Intervalo *)b;

    if (intervalo_a->fim < intervalo_b->fim)
    {
        resultado = -1;
    }
    else if (intervalo_a->fim > intervalo_b->fim)
    {
        resultado = 1;
    }
    else
    {
        if (intervalo_a->inicio < intervalo_b->inicio)
        {
            resultado = -1;
        }
        else if (intervalo_a->inicio > intervalo_b->inicio)
        {
            resultado = 1;
        }
        else
        {
            resultado = 0;
        }
    }

    return resultado;
}

int comparar_pontos(const void *a, const void *b)
{
    int resultado = 0;
    Ponto *ponto_a = (Ponto *)a;
    Ponto *ponto_b = (Ponto *)b;

    if (ponto_a->posicao < ponto_b->posicao)
    {
        resultado = -1;
    }
    else if (ponto_a->posicao > ponto_b->posicao)
    {
        resultado = 1;
    }
    else
    {
        resultado = 0;
    }

    return resultado;
}

// Cenário PEQUENO - Determinístico
void configurar_cenario_pequeno(Problema *problema) {
    // 8 pontos fixos
    problema->n_pontos = 8;
    problema->pontos = (Ponto *)malloc(8 * sizeof(Ponto));
    problema->pontos[0] = (Ponto){1, 5};
    problema->pontos[1] = (Ponto){2, 10};
    problema->pontos[2] = (Ponto){3, 15};
    problema->pontos[3] = (Ponto){4, 20};
    problema->pontos[4] = (Ponto){5, 25};
    problema->pontos[5] = (Ponto){6, 30};
    problema->pontos[6] = (Ponto){7, 35};
    problema->pontos[7] = (Ponto){8, 40};
    
    // 10 intervalos fixos que cobrem todos os pontos
    problema->n_intervalos = 10;
    problema->intervalos = (Intervalo *)malloc(10 * sizeof(Intervalo));
    problema->intervalos[0] = (Intervalo){0, 12};
    problema->intervalos[1] = (Intervalo){8, 22};
    problema->intervalos[2] = (Intervalo){15, 28};
    problema->intervalos[3] = (Intervalo){25, 42};
    problema->intervalos[4] = (Intervalo){5, 18};
    problema->intervalos[5] = (Intervalo){18, 32};
    problema->intervalos[6] = (Intervalo){30, 45};
    problema->intervalos[7] = (Intervalo){3, 15};
    problema->intervalos[8] = (Intervalo){20, 35};
    problema->intervalos[9] = (Intervalo){12, 26};
    
    qsort(problema->intervalos, problema->n_intervalos, sizeof(Intervalo), comparar_intervalos);
}

// Cenário MÉDIO - Determinístico
void configurar_cenario_medio(Problema *problema) {
    // 10 pontos fixos
    problema->n_pontos = 10;
    problema->pontos = (Ponto *)malloc(10 * sizeof(Ponto));
    problema->pontos[0] = (Ponto){1, 8};
    problema->pontos[1] = (Ponto){2, 16};
    problema->pontos[2] = (Ponto){3, 24};
    problema->pontos[3] = (Ponto){4, 32};
    problema->pontos[4] = (Ponto){5, 40};
    problema->pontos[5] = (Ponto){6, 48};
    problema->pontos[6] = (Ponto){7, 56};
    problema->pontos[7] = (Ponto){8, 64};
    problema->pontos[8] = (Ponto){9, 72};
    problema->pontos[9] = (Ponto){10, 80};
    
    // 12 intervalos fixos que cobrem todos os pontos
    problema->n_intervalos = 12;
    problema->intervalos = (Intervalo *)malloc(12 * sizeof(Intervalo));
    problema->intervalos[0] = (Intervalo){0, 20};
    problema->intervalos[1] = (Intervalo){12, 35};
    problema->intervalos[2] = (Intervalo){25, 45};
    problema->intervalos[3] = (Intervalo){38, 58};
    problema->intervalos[4] = (Intervalo){50, 75};
    problema->intervalos[5] = (Intervalo){65, 85};
    problema->intervalos[6] = (Intervalo){5, 28};
    problema->intervalos[7] = (Intervalo){20, 42};
    problema->intervalos[8] = (Intervalo){35, 55};
    problema->intervalos[9] = (Intervalo){48, 68};
    problema->intervalos[10] = (Intervalo){60, 82};
    problema->intervalos[11] = (Intervalo){15, 38};
    
    qsort(problema->intervalos, problema->n_intervalos, sizeof(Intervalo), comparar_intervalos);
}

// Cenário GRANDE - Determinístico
void configurar_cenario_grande(Problema *problema) {
    // 12 pontos fixos
    problema->n_pontos = 12;
    problema->pontos = (Ponto *)malloc(12 * sizeof(Ponto));
    problema->pontos[0] = (Ponto){1, 5};
    problema->pontos[1] = (Ponto){2, 12};
    problema->pontos[2] = (Ponto){3, 18};
    problema->pontos[3] = (Ponto){4, 25};
    problema->pontos[4] = (Ponto){5, 32};
    problema->pontos[5] = (Ponto){6, 38};
    problema->pontos[6] = (Ponto){7, 45};
    problema->pontos[7] = (Ponto){8, 52};
    problema->pontos[8] = (Ponto){9, 58};
    problema->pontos[9] = (Ponto){10, 65};
    problema->pontos[10] = (Ponto){11, 72};
    problema->pontos[11] = (Ponto){12, 80};
    
    // 15 intervalos fixos que cobrem todos os pontos
    problema->n_intervalos = 15;
    problema->intervalos = (Intervalo *)malloc(15 * sizeof(Intervalo));
    problema->intervalos[0] = (Intervalo){0, 15};
    problema->intervalos[1] = (Intervalo){10, 28};
    problema->intervalos[2] = (Intervalo){20, 35};
    problema->intervalos[3] = (Intervalo){30, 48};
    problema->intervalos[4] = (Intervalo){40, 58};
    problema->intervalos[5] = (Intervalo){50, 68};
    problema->intervalos[6] = (Intervalo){60, 85};
    problema->intervalos[7] = (Intervalo){5, 20};
    problema->intervalos[8] = (Intervalo){15, 32};
    problema->intervalos[9] = (Intervalo){25, 42};
    problema->intervalos[10] = (Intervalo){35, 52};
    problema->intervalos[11] = (Intervalo){45, 62};
    problema->intervalos[12] = (Intervalo){55, 75};
    problema->intervalos[13] = (Intervalo){70, 85};
    problema->intervalos[14] = (Intervalo){8, 26};
    
    qsort(problema->intervalos, problema->n_intervalos, sizeof(Intervalo), comparar_intervalos);
}

int ponto_coberto_por_intervalo(Ponto ponto, Intervalo intervalo)
{
    int resultado = 0;
    if (ponto.posicao >= intervalo.inicio && ponto.posicao <= intervalo.fim)
    {
        resultado = 1;
    }
    else
    {
        resultado = 0;
    }
    return resultado;
}

void marcar_pontos_cobertos(Problema *problema, Intervalo intervalo)
{
    for (int i = 0; i < problema->n_pontos; i++)
    {
        if (problema->pontos_cobertos[i] == 0)
        {
            if (ponto_coberto_por_intervalo(problema->pontos[i], intervalo))
            {
                problema->pontos_cobertos[i] = 1;
                problema->n_pontos_cobertos++;
            }
        }
    }
}

int todos_pontos_cobertos(Problema *problema)
{
    int resultado = 0;
    if (problema->n_pontos_cobertos == problema->n_pontos)
    {
        resultado = 1;
    }
    else
    {
        resultado = 0;
    }
    return resultado;
}

int obter_proximo_ponto_nao_coberto(Problema *problema)
{
    int resultado = -1;
    for (int i = 0; i < problema->n_pontos; i++)
    {
        if (problema->pontos_cobertos[i] == 0)
        {
            resultado = i;
            break;
        }
    }
    return resultado;
}

int encontrar_melhor_intervalo(Problema *problema, int indice_ponto)
{
    int melhor_intervalo = -1;
    int max_pontos_cobertos = 0;
    Ponto ponto_atual = problema->pontos[indice_ponto];

    for (int i = 0; i < problema->n_intervalos; i++)
    {
        if (ponto_coberto_por_intervalo(ponto_atual, problema->intervalos[i]))
        {
            int pontos_cobertos_potencial = 0;

            for (int j = 0; j < problema->n_pontos; j++)
            {
                if (problema->pontos_cobertos[j] == 0)
                {
                    if (ponto_coberto_por_intervalo(problema->pontos[j], problema->intervalos[i]))
                    {
                        pontos_cobertos_potencial++;
                    }
                }
            }

            if (pontos_cobertos_potencial > max_pontos_cobertos)
            {
                max_pontos_cobertos = pontos_cobertos_potencial;
                melhor_intervalo = i;
            }
            else if (pontos_cobertos_potencial == max_pontos_cobertos && pontos_cobertos_potencial > 0)
            {
                if (melhor_intervalo == -1)
                {
                    melhor_intervalo = i;
                }
                else
                {
                    int tamanho_melhor = problema->intervalos[melhor_intervalo].fim - problema->intervalos[melhor_intervalo].inicio;
                    int tamanho_atual = problema->intervalos[i].fim - problema->intervalos[i].inicio;

                    if (tamanho_atual < tamanho_melhor)
                    {
                        melhor_intervalo = i;
                    }
                }
            }
        }
    }

    return melhor_intervalo;
}

Metricas resolver_guloso(Problema *problema)
{
    Metricas metricas;
    struct timespec inicio, fim;
    struct rusage uso_memoria;
    int indice_ponto;
    int indice_intervalo;

    metricas.tempo = 0.0;
    metricas.memoria = 0;
    metricas.qualidade = 0.0;
    metricas.n_solucao = 0;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    problema->pontos_cobertos = (int *)calloc(problema->n_pontos, sizeof(int));
    if (problema->pontos_cobertos == NULL)
    {
        clock_gettime(CLOCK_MONOTONIC, &fim);
        problema->tempo_execucao = (fim.tv_sec - inicio.tv_sec) * 1000.0 + (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
        metricas.tempo = problema->tempo_execucao;
        getrusage(RUSAGE_SELF, &uso_memoria);
        problema->memoria_utilizada = uso_memoria.ru_maxrss;
        metricas.memoria = problema->memoria_utilizada;
        return metricas;
    }

    problema->n_pontos_cobertos = 0;

    problema->solucao = (Intervalo *)malloc(problema->n_intervalos * sizeof(Intervalo));
    if (problema->solucao == NULL)
    {
        free(problema->pontos_cobertos);
        clock_gettime(CLOCK_MONOTONIC, &fim);
        problema->tempo_execucao = (fim.tv_sec - inicio.tv_sec) * 1000.0 + (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
        metricas.tempo = problema->tempo_execucao;
        getrusage(RUSAGE_SELF, &uso_memoria);
        problema->memoria_utilizada = uso_memoria.ru_maxrss;
        metricas.memoria = problema->memoria_utilizada;
        return metricas;
    }

    problema->n_solucao = 0;

    while (todos_pontos_cobertos(problema) == 0)
    {
        indice_ponto = obter_proximo_ponto_nao_coberto(problema);

        if (indice_ponto == -1)
        {
            break;
        }

        indice_intervalo = encontrar_melhor_intervalo(problema, indice_ponto);

        if (indice_intervalo == -1)
        {
            break;
        }

        problema->solucao[problema->n_solucao] = problema->intervalos[indice_intervalo];
        problema->n_solucao++;

        marcar_pontos_cobertos(problema, problema->intervalos[indice_intervalo]);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    problema->tempo_execucao = (fim.tv_sec - inicio.tv_sec) * 1000.0 + (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
    metricas.tempo = problema->tempo_execucao;

    getrusage(RUSAGE_SELF, &uso_memoria);
    problema->memoria_utilizada = uso_memoria.ru_maxrss;
    metricas.memoria = problema->memoria_utilizada;

    if (problema->n_intervalos > 0)
    {
        problema->qualidade = 1.0 - ((double)problema->n_solucao / problema->n_intervalos);
        metricas.qualidade = problema->qualidade;
    }
    else
    {
        problema->qualidade = 0.0;
        metricas.qualidade = 0.0;
    }

    metricas.n_solucao = problema->n_solucao;

    return metricas;
}

void exibir_solucao(Problema *problema)
{
    printf("Solucao encontrada (%d intervalos):\n", problema->n_solucao);
    for (int i = 0; i < problema->n_solucao; i++)
    {
        printf("  Intervalo %d: [%d, %d]\n", i + 1, problema->solucao[i].inicio, problema->solucao[i].fim);
    }

    printf("\nPontos cobertos (%d de %d):\n", problema->n_pontos_cobertos, problema->n_pontos);
    for (int i = 0; i < problema->n_pontos; i++)
    {
        if (problema->pontos_cobertos[i])
        {
            printf("  Ponto %d: %d\n", problema->pontos[i].id, problema->pontos[i].posicao);
        }
    }
}

void exibir_metricas(Problema *problema)
{
    printf("\n=== METRICAS DO ALGORITMO GULOSO ===\n");
    printf("Tempo de execucao: %.4f ms\n", problema->tempo_execucao);
    printf("Memoria utilizada: %ld KB\n", problema->memoria_utilizada);
    printf("Numero de intervalos na solucao: %d\n", problema->n_solucao);
    printf("Qualidade (1 - solucao/total): %.4f\n", problema->qualidade);
    printf("====================================\n\n");
}

void salvar_csv(Metricas *metricas_pequeno, Metricas *metricas_medio, Metricas *metricas_grande)
{
    FILE *arquivo = NULL;
    char caminho[MAX_PATH];

    snprintf(caminho, MAX_PATH, "../results/guloso/file/metricas_guloso.csv");

    arquivo = fopen(caminho, "w");
    if (arquivo == NULL)
    {
        printf("Erro ao criar arquivo CSV. Verifique se a pasta ../results/guloso existe.\n");
        return;
    }

    fprintf(arquivo, "cenario,tempo_ms,memoria_kb,qualidade,n_intervalos_solucao\n");
    fprintf(arquivo, "pequeno,%.4f,%ld,%.4f,%d\n",
            metricas_pequeno->tempo, metricas_pequeno->memoria, metricas_pequeno->qualidade, metricas_pequeno->n_solucao);
    fprintf(arquivo, "medio,%.4f,%ld,%.4f,%d\n",
            metricas_medio->tempo, metricas_medio->memoria, metricas_medio->qualidade, metricas_medio->n_solucao);
    fprintf(arquivo, "grande,%.4f,%ld,%.4f,%d\n",
            metricas_grande->tempo, metricas_grande->memoria, metricas_grande->qualidade, metricas_grande->n_solucao);

    fclose(arquivo);
    printf("Metricas salvas em: %s\n", caminho);
}

void executar_teste(Problema *problema, const char *nome_cenario, Metricas *metricas)
{
    printf("\n=== EXECUTANDO CENARIO %s ===\n", nome_cenario);
    printf("Pontos: %d, Intervalos: %d\n", problema->n_pontos, problema->n_intervalos);

    *metricas = resolver_guloso(problema);

    exibir_solucao(problema);
    exibir_metricas(problema);
}

void executar_todos_testes()
{
    Problema problema_pequeno, problema_medio, problema_grande;
    Metricas metricas_pequeno, metricas_medio, metricas_grande;

    printf("=== EXECUTANDO TODOS OS TESTES (GULOSO) ===\n");

    inicializar_problema(&problema_pequeno);
    inicializar_problema(&problema_medio);
    inicializar_problema(&problema_grande);

    configurar_cenario_pequeno(&problema_pequeno);
    configurar_cenario_medio(&problema_medio);
    configurar_cenario_grande(&problema_grande);

    executar_teste(&problema_pequeno, "PEQUENO", &metricas_pequeno);
    executar_teste(&problema_medio, "MEDIO", &metricas_medio);
    executar_teste(&problema_grande, "GRANDE", &metricas_grande);

    salvar_csv(&metricas_pequeno, &metricas_medio, &metricas_grande);

    liberar_problema(&problema_pequeno);
    liberar_problema(&problema_medio);
    liberar_problema(&problema_grande);
}

// void exibir_menu()
// {
//     printf("\n=== PROBLEMA DA COBERTURA DE PONTOS COM INTERVALOS ===\n");
//     printf("ALGORITMO: GULOSO\n");
//     printf("\nMenu de opcoes:\n");
//     printf("1. Executar cenario PEQUENO (20 pontos, 15 intervalos)\n");
//     printf("2. Executar cenario MEDIO (100 pontos, 50 intervalos)\n");
//     printf("3. Executar cenario GRANDE (500 pontos, 200 intervalos)\n");
//     printf("4. Executar TODOS os cenarios e gerar CSV\n");
//     printf("5. Sair\n");
//     printf("\nEscolha uma opcao: ");
// }

void exibir_menu() {
    printf("\n=== PROBLEMA DA COBERTURA DE PONTOS COM INTERVALOS ===\n");
    printf("ALGORITMO: GULOSO\n");
    printf("\nMenu de opcoes:\n");
    printf("1. Executar cenario PEQUENO (8 pontos, 10 intervalos)\n");
    printf("2. Executar cenario MEDIO (10 pontos, 12 intervalos)\n");
    printf("3. Executar cenario GRANDE (12 pontos, 15 intervalos)\n");
    printf("4. Executar TODOS os cenarios e gerar CSV\n");
    printf("5. Sair\n");
    printf("\nEscolha uma opcao: ");
}

int main()
{
    int opcao = 0;
    int executando = 1;

    while (executando)
    {
        exibir_menu();

        if (scanf("%d", &opcao) != 1)
        {
            while (getchar() != '\n')
            {
                continue;
            }
            printf("Entrada invalida. Tente novamente.\n");
            continue;
        }

        switch (opcao)
        {
        case 1:
        {
            Problema problema;
            Metricas metricas;
            inicializar_problema(&problema);
            configurar_cenario_pequeno(&problema);
            executar_teste(&problema, "PEQUENO", &metricas);
            liberar_problema(&problema);
            break;
        }
        case 2:
        {
            Problema problema;
            Metricas metricas;
            inicializar_problema(&problema);
            configurar_cenario_medio(&problema);
            executar_teste(&problema, "MEDIO", &metricas);
            liberar_problema(&problema);
            break;
        }
        case 3:
        {
            Problema problema;
            Metricas metricas;
            inicializar_problema(&problema);
            configurar_cenario_grande(&problema);
            executar_teste(&problema, "GRANDE", &metricas);
            liberar_problema(&problema);
            break;
        }
        case 4:
        {
            executar_todos_testes();
            break;
        }
        case 5:
        {
            printf("Encerrando programa...\n");
            executando = 0;
            break;
        }
        default:
        {
            printf("Opcao invalida. Tente novamente.\n");
            break;
        }
        }
    }

    return 0;
}
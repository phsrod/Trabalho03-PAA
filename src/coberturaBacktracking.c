#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <math.h>
#include <limits.h>

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
    Intervalo *solucao_atual;
    int n_solucao_atual;
    Intervalo *melhor_solucao;
    int n_melhor_solucao;
    double tempo_execucao;
    long memoria_utilizada;
    double qualidade;
    int nos_visitados;
} ProblemaBacktracking;

typedef struct
{
    double tempo;
    long memoria;
    double qualidade;
    int n_solucao;
    int nos_visitados;
} MetricasBacktracking;

void inicializar_problema_backtracking(ProblemaBacktracking *problema)
{
    problema->intervalos = NULL;
    problema->pontos = NULL;
    problema->pontos_cobertos = NULL;
    problema->solucao_atual = NULL;
    problema->melhor_solucao = NULL;
    problema->n_intervalos = 0;
    problema->n_pontos = 0;
    problema->n_pontos_cobertos = 0;
    problema->n_solucao_atual = 0;
    problema->n_melhor_solucao = INT_MAX;
    problema->tempo_execucao = 0.0;
    problema->memoria_utilizada = 0;
    problema->qualidade = 0.0;
    problema->nos_visitados = 0;
}

void liberar_problema_backtracking(ProblemaBacktracking *problema)
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
    if (problema->solucao_atual != NULL)
    {
        free(problema->solucao_atual);
        problema->solucao_atual = NULL;
    }
    if (problema->melhor_solucao != NULL)
    {
        free(problema->melhor_solucao);
        problema->melhor_solucao = NULL;
    }
}

int comparar_intervalos_backtracking(const void *a, const void *b)
{
    int resultado = 0;
    Intervalo *intervalo_a = (Intervalo *)a;
    Intervalo *intervalo_b = (Intervalo *)b;

    int tamanho_a = intervalo_a->fim - intervalo_a->inicio;
    int tamanho_b = intervalo_b->fim - intervalo_b->inicio;

    if (tamanho_a > tamanho_b)
    {
        resultado = -1;
    }
    else if (tamanho_a < tamanho_b)
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

int comparar_pontos_backtracking(const void *a, const void *b)
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

void carregar_pontos_aleatorios_backtracking(ProblemaBacktracking *problema, int n_pontos, int min_pos, int max_pos)
{
    problema->pontos = (Ponto *)malloc(n_pontos * sizeof(Ponto));
    if (problema->pontos != NULL)
    {
        problema->n_pontos = n_pontos;
        for (int i = 0; i < n_pontos; i++)
        {
            problema->pontos[i].id = i + 1;
            problema->pontos[i].posicao = min_pos + rand() % (max_pos - min_pos + 1);
        }
        qsort(problema->pontos, n_pontos, sizeof(Ponto), comparar_pontos_backtracking);
    }
}

void carregar_intervalos_aleatorios_backtracking(ProblemaBacktracking *problema, int n_intervalos, int min_tamanho, int max_tamanho, int max_pos)
{
    problema->intervalos = (Intervalo *)malloc(n_intervalos * sizeof(Intervalo));
    if (problema->intervalos != NULL)
    {
        problema->n_intervalos = n_intervalos;
        for (int i = 0; i < n_intervalos; i++)
        {
            int tamanho = min_tamanho + rand() % (max_tamanho - min_tamanho + 1);
            int inicio = rand() % (max_pos - tamanho + 1);
            problema->intervalos[i].inicio = inicio;
            problema->intervalos[i].fim = inicio + tamanho;
        }
        qsort(problema->intervalos, n_intervalos, sizeof(Intervalo), comparar_intervalos_backtracking);
    }
}

void configurar_cenario_pequeno_backtracking(ProblemaBacktracking* problema) {
    srand(42);  // Mantém a mesma semente para reprodutibilidade
    carregar_pontos_aleatorios_backtracking(problema, 8, 0, 50);      // 8 pontos (reduzido)
    carregar_intervalos_aleatorios_backtracking(problema, 6, 5, 20, 50); // 6 intervalos (reduzido)
}

void configurar_cenario_medio_backtracking(ProblemaBacktracking* problema) {
    srand(43);  // Mantém a mesma semente
    carregar_pontos_aleatorios_backtracking(problema, 12, 0, 100);    // 12 pontos (reduzido)
    carregar_intervalos_aleatorios_backtracking(problema, 10, 10, 30, 100); // 10 intervalos (reduzido)
}

void configurar_cenario_grande_backtracking(ProblemaBacktracking* problema) {
    srand(44);  // Mantém a mesma semente
    carregar_pontos_aleatorios_backtracking(problema, 15, 0, 150);    // 15 pontos (reduzido)
    carregar_intervalos_aleatorios_backtracking(problema, 12, 15, 40, 150); // 12 intervalos (reduzido)
}

int ponto_coberto_por_intervalo_backtracking(Ponto ponto, Intervalo intervalo)
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

int verificar_cobertura_parcial(ProblemaBacktracking *problema)
{
    int resultado = 0;
    int *cobertura_temp = NULL;
    int pontos_cobertos = 0;

    cobertura_temp = (int *)calloc(problema->n_pontos, sizeof(int));
    if (cobertura_temp == NULL)
    {
        resultado = 0;
    }
    else
    {
        for (int i = 0; i < problema->n_solucao_atual; i++)
        {
            for (int j = 0; j < problema->n_pontos; j++)
            {
                if (cobertura_temp[j] == 0)
                {
                    if (ponto_coberto_por_intervalo_backtracking(problema->pontos[j], problema->solucao_atual[i]))
                    {
                        cobertura_temp[j] = 1;
                        pontos_cobertos++;
                    }
                }
            }
        }

        free(cobertura_temp);

        if (pontos_cobertos == problema->n_pontos)
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

void copiar_solucao(ProblemaBacktracking *problema)
{
    if (problema->melhor_solucao == NULL)
    {
        problema->melhor_solucao = (Intervalo *)malloc(problema->n_intervalos * sizeof(Intervalo));
    }

    if (problema->melhor_solucao != NULL)
    {
        for (int i = 0; i < problema->n_solucao_atual; i++)
        {
            problema->melhor_solucao[i] = problema->solucao_atual[i];
        }
        problema->n_melhor_solucao = problema->n_solucao_atual;
    }
}

void backtracking_recursivo(ProblemaBacktracking *problema, int indice_intervalo)
{
    problema->nos_visitados++;

    if (indice_intervalo >= problema->n_intervalos)
    {
        return;
    }

    if (problema->n_solucao_atual >= problema->n_melhor_solucao)
    {
        return;
    }

    int pode_podar = 0;
    if (problema->n_solucao_atual + 1 >= problema->n_melhor_solucao)
    {
        pode_podar = 1;
    }

    if (pode_podar == 0)
    {
        problema->solucao_atual[problema->n_solucao_atual] = problema->intervalos[indice_intervalo];
        problema->n_solucao_atual++;

        if (verificar_cobertura_parcial(problema))
        {
            if (problema->n_solucao_atual < problema->n_melhor_solucao)
            {
                copiar_solucao(problema);
            }
        }
        else
        {
            backtracking_recursivo(problema, indice_intervalo + 1);
        }

        problema->n_solucao_atual--;
    }

    backtracking_recursivo(problema, indice_intervalo + 1);
}

MetricasBacktracking resolver_backtracking(ProblemaBacktracking *problema)
{
    MetricasBacktracking metricas;
    struct timespec inicio, fim;
    struct rusage uso_memoria;

    metricas.tempo = 0.0;
    metricas.memoria = 0;
    metricas.qualidade = 0.0;
    metricas.n_solucao = 0;
    metricas.nos_visitados = 0;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    problema->solucao_atual = (Intervalo *)malloc(problema->n_intervalos * sizeof(Intervalo));
    if (problema->solucao_atual == NULL)
    {
        clock_gettime(CLOCK_MONOTONIC, &fim);
        problema->tempo_execucao = (fim.tv_sec - inicio.tv_sec) * 1000.0 + (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
        metricas.tempo = problema->tempo_execucao;
        getrusage(RUSAGE_SELF, &uso_memoria);
        problema->memoria_utilizada = uso_memoria.ru_maxrss;
        metricas.memoria = problema->memoria_utilizada;
        return metricas;
    }

    problema->n_solucao_atual = 0;
    problema->nos_visitados = 0;

    backtracking_recursivo(problema, 0);

    clock_gettime(CLOCK_MONOTONIC, &fim);

    problema->tempo_execucao = (fim.tv_sec - inicio.tv_sec) * 1000.0 + (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
    metricas.tempo = problema->tempo_execucao;

    getrusage(RUSAGE_SELF, &uso_memoria);
    problema->memoria_utilizada = uso_memoria.ru_maxrss;
    metricas.memoria = problema->memoria_utilizada;

    if (problema->n_intervalos > 0)
    {
        problema->qualidade = 1.0 - ((double)problema->n_melhor_solucao / problema->n_intervalos);
        metricas.qualidade = problema->qualidade;
    }
    else
    {
        problema->qualidade = 0.0;
        metricas.qualidade = 0.0;
    }

    metricas.n_solucao = problema->n_melhor_solucao;
    metricas.nos_visitados = problema->nos_visitados;

    return metricas;
}

void exibir_solucao_backtracking(ProblemaBacktracking *problema)
{
    int *pontos_cobertos = NULL;
    int total_cobertos = 0;

    printf("Melhor solucao encontrada (%d intervalos):\n", problema->n_melhor_solucao);

    if (problema->n_melhor_solucao == INT_MAX)
    {
        printf("  Nenhuma solucao completa encontrada!\n");
    }
    else
    {
        for (int i = 0; i < problema->n_melhor_solucao; i++)
        {
            printf("  Intervalo %d: [%d, %d]\n", i + 1, problema->melhor_solucao[i].inicio, problema->melhor_solucao[i].fim);
        }

        pontos_cobertos = (int *)calloc(problema->n_pontos, sizeof(int));
        if (pontos_cobertos != NULL)
        {
            for (int i = 0; i < problema->n_melhor_solucao; i++)
            {
                for (int j = 0; j < problema->n_pontos; j++)
                {
                    if (pontos_cobertos[j] == 0)
                    {
                        if (ponto_coberto_por_intervalo_backtracking(problema->pontos[j], problema->melhor_solucao[i]))
                        {
                            pontos_cobertos[j] = 1;
                            total_cobertos++;
                        }
                    }
                }
            }

            printf("\nPontos cobertos (%d de %d):\n", total_cobertos, problema->n_pontos);
            for (int i = 0; i < problema->n_pontos; i++)
            {
                if (pontos_cobertos[i])
                {
                    printf("  Ponto %d: %d\n", problema->pontos[i].id, problema->pontos[i].posicao);
                }
            }

            free(pontos_cobertos);
        }
    }
}

void exibir_metricas_backtracking(ProblemaBacktracking *problema)
{
    printf("\n=== METRICAS DO ALGORITMO BACKTRACKING ===\n");
    printf("Tempo de execucao: %.4f ms\n", problema->tempo_execucao);
    printf("Memoria utilizada: %ld KB\n", problema->memoria_utilizada);
    printf("Numero de intervalos na solucao: %d\n", problema->n_melhor_solucao);
    printf("Qualidade (1 - solucao/total): %.4f\n", problema->qualidade);
    printf("Nos visitados na arvore de busca: %d\n", problema->nos_visitados);
    printf("=========================================\n\n");
}

void salvar_csv_backtracking(MetricasBacktracking *metricas_pequeno, MetricasBacktracking *metricas_medio, MetricasBacktracking *metricas_grande)
{
    FILE *arquivo = NULL;
    char caminho[MAX_PATH];

    snprintf(caminho, MAX_PATH, "../results/backtracking/metricas_backtracking.csv");

    arquivo = fopen(caminho, "w");
    if (arquivo == NULL)
    {
        printf("Erro ao criar arquivo CSV. Verifique se a pasta ../results/backtracking existe.\n");
    }
    else
    {
        fprintf(arquivo, "cenario,tempo_ms,memoria_kb,qualidade,n_intervalos_solucao,nos_visitados\n");
        fprintf(arquivo, "pequeno,%.4f,%ld,%.4f,%d,%d\n",
                metricas_pequeno->tempo, metricas_pequeno->memoria, metricas_pequeno->qualidade,
                metricas_pequeno->n_solucao, metricas_pequeno->nos_visitados);
        fprintf(arquivo, "medio,%.4f,%ld,%.4f,%d,%d\n",
                metricas_medio->tempo, metricas_medio->memoria, metricas_medio->qualidade,
                metricas_medio->n_solucao, metricas_medio->nos_visitados);
        fprintf(arquivo, "grande,%.4f,%ld,%.4f,%d,%d\n",
                metricas_grande->tempo, metricas_grande->memoria, metricas_grande->qualidade,
                metricas_grande->n_solucao, metricas_grande->nos_visitados);

        fclose(arquivo);
        printf("Metricas salvas em: %s\n", caminho);
    }
}

void executar_teste_backtracking(ProblemaBacktracking *problema, const char *nome_cenario, MetricasBacktracking *metricas)
{
    printf("\n=== EXECUTANDO CENARIO %s ===\n", nome_cenario);
    printf("Pontos: %d, Intervalos: %d\n", problema->n_pontos, problema->n_intervalos);

    *metricas = resolver_backtracking(problema);

    exibir_solucao_backtracking(problema);
    exibir_metricas_backtracking(problema);
}

void executar_todos_testes_backtracking()
{
    ProblemaBacktracking problema_pequeno, problema_medio, problema_grande;
    MetricasBacktracking metricas_pequeno, metricas_medio, metricas_grande;

    printf("=== EXECUTANDO TODOS OS TESTES (BACKTRACKING) ===\n");

    inicializar_problema_backtracking(&problema_pequeno);
    inicializar_problema_backtracking(&problema_medio);
    inicializar_problema_backtracking(&problema_grande);

    configurar_cenario_pequeno_backtracking(&problema_pequeno);
    configurar_cenario_medio_backtracking(&problema_medio);
    configurar_cenario_grande_backtracking(&problema_grande);

    executar_teste_backtracking(&problema_pequeno, "PEQUENO", &metricas_pequeno);
    executar_teste_backtracking(&problema_medio, "MEDIO", &metricas_medio);
    executar_teste_backtracking(&problema_grande, "GRANDE", &metricas_grande);

    salvar_csv_backtracking(&metricas_pequeno, &metricas_medio, &metricas_grande);

    liberar_problema_backtracking(&problema_pequeno);
    liberar_problema_backtracking(&problema_medio);
    liberar_problema_backtracking(&problema_grande);
}

void exibir_menu_backtracking() {
    printf("\n=== PROBLEMA DA COBERTURA DE PONTOS COM INTERVALOS ===\n");
    printf("ALGORITMO: BACKTRACKING\n");
    printf("\nMenu de opcoes:\n");
    printf("1. Executar cenario PEQUENO (8 pontos, 6 intervalos)\n");
    printf("2. Executar cenario MEDIO (12 pontos, 10 intervalos)\n");
    printf("3. Executar cenario GRANDE (15 pontos, 12 intervalos)\n");
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
        exibir_menu_backtracking();

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
            ProblemaBacktracking problema;
            MetricasBacktracking metricas;
            inicializar_problema_backtracking(&problema);
            configurar_cenario_pequeno_backtracking(&problema);
            executar_teste_backtracking(&problema, "PEQUENO", &metricas);
            liberar_problema_backtracking(&problema);
            break;
        }
        case 2:
        {
            ProblemaBacktracking problema;
            MetricasBacktracking metricas;
            inicializar_problema_backtracking(&problema);
            configurar_cenario_medio_backtracking(&problema);
            executar_teste_backtracking(&problema, "MEDIO", &metricas);
            liberar_problema_backtracking(&problema);
            break;
        }
        case 3:
        {
            ProblemaBacktracking problema;
            MetricasBacktracking metricas;
            inicializar_problema_backtracking(&problema);
            configurar_cenario_grande_backtracking(&problema);
            executar_teste_backtracking(&problema, "GRANDE", &metricas);
            liberar_problema_backtracking(&problema);
            break;
        }
        case 4:
        {
            executar_todos_testes_backtracking();
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
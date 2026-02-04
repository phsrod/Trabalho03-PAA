#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <math.h>

#define MAX_PONTOS 1000
#define MAX_INTERVALOS 1000
#define MAX_PATH 1024

/**
 * @struct Intervalo
 * @brief Representa um intervalo fechado na reta numérica.
 *
 * Esta estrutura é utilizada pelo algoritmo guloso para modelar
 * intervalos que podem cobrir pontos em uma dimensão linear.
 * Um ponto é considerado coberto se sua posição estiver entre
 * o início e o fim do intervalo (inclusive).
 *
 * No problema da cobertura de pontos, o objetivo é selecionar
 * um subconjunto mínimo desses intervalos capaz de cobrir
 * todos os pontos definidos.
 */
typedef struct
{
    int inicio; /**< Limite inicial do intervalo (inclusive). */
    int fim; /**< Limite final do intervalo (inclusive). */
} Intervalo;

/**
 * @struct Ponto
 * @brief Representa um ponto na reta numérica a ser coberto por intervalos.
 *
 * Cada ponto possui um identificador único e uma posição inteira
 * na reta numérica. No problema da cobertura de pontos, um ponto
 * é considerado coberto se existir ao menos um intervalo cujo
 * início e fim englobem sua posição.
 *
 * Essa estrutura é utilizada pelo algoritmo guloso para verificar
 * a cobertura dos pontos e auxiliar na escolha dos intervalos
 * mais adequados durante a execução do algoritmo.
 */
typedef struct
{
    int id; /**< Identificador único do ponto. */
    int posicao; /**< Posição do ponto na reta numérica. */
} Ponto;

/**
 * @struct Problema
 * @brief Estrutura que encapsula todos os dados e métricas do problema
 *        de cobertura de pontos com intervalos utilizando algoritmo guloso.
 *
 * Esta estrutura armazena:
 * - Os pontos que devem ser cobertos;
 * - Os intervalos disponíveis para realizar a cobertura;
 * - O estado atual da cobertura durante a execução do algoritmo guloso;
 * - A solução construída incrementalmente;
 * - As métricas de desempenho obtidas ao final da execução.
 *
 * No algoritmo guloso, os intervalos são selecionados iterativamente
 * com base em um critério de escolha (por exemplo, o intervalo que
 * cobre o maior número de pontos ainda não cobertos), até que todos
 * os pontos estejam cobertos ou não haja mais intervalos viáveis.
 */
typedef struct
{
    Intervalo *intervalos; /**< Vetor de intervalos disponíveis para cobertura. */
    int n_intervalos; /**< Quantidade total de intervalos disponíveis. */
    Ponto *pontos; /**< Vetor de pontos que devem ser cobertos. */
    int n_pontos; /**< Quantidade total de pontos. */
    int *pontos_cobertos; /**< Vetor auxiliar que indica se cada ponto já foi coberto (0 ou 1). */
    int n_pontos_cobertos; /**< Número total de pontos já cobertos durante a execução. */
    Intervalo *solucao; /**< Vetor de intervalos escolhidos pelo algoritmo guloso. */
    int n_solucao; /**< Quantidade de intervalos presentes na solução gulosa. */
    double tempo_execucao; /**< Tempo total de execução do algoritmo, em milissegundos. */
    long memoria_utilizada; /**< Memória utilizada pelo algoritmo, em kilobytes. */
    double qualidade; /**< Métrica de qualidade da solução obtida pelo algoritmo guloso. */
} Problema;

/**
 * @struct Metricas
 * @brief Estrutura que armazena as métricas de desempenho do algoritmo guloso.
 *
 * Esta estrutura é utilizada para registrar e exportar os resultados
 * obtidos após a execução do algoritmo guloso no problema de cobertura
 * de pontos com intervalos.
 *
 * As métricas permitem avaliar:
 * - O desempenho temporal do algoritmo;
 * - O consumo de memória;
 * - A qualidade da solução encontrada;
 * - O tamanho da solução gerada.
 *
 * Esses dados são úteis para análises comparativas com outros algoritmos,
 * como o backtracking.
 */
typedef struct
{
    double tempo; /**< Tempo total de execução do algoritmo, em milissegundos. */
    long memoria; /**< Memória utilizada durante a execução, em kilobytes. */
    double qualidade; /**< Qualidade da solução gulosa obtida. */
    int n_solucao; /**< Número de intervalos selecionados na solução final. */
} Metricas;

/**
 * @brief Inicializa a estrutura do problema para o algoritmo guloso.
 *
 * Esta função prepara a estrutura `Problema` para uso, inicializando
 * todos os ponteiros como NULL e zerando os contadores e métricas.
 *
 * Ela deve ser chamada antes de configurar qualquer cenário ou executar
 * o algoritmo guloso, garantindo que o estado inicial do problema seja
 * consistente e evitando acessos indevidos à memória.
 *
 * @param problema Ponteiro para a estrutura `Problema` a ser inicializada.
 */
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

/**
 * @brief Libera toda a memória alocada dinamicamente pelo problema guloso.
 *
 * Esta função é responsável por desalocar todos os vetores associados
 * à estrutura `Problema`, incluindo intervalos, pontos, controle de
 * cobertura e solução construída pelo algoritmo guloso.
 *
 * Após a liberação, todos os ponteiros são definidos como NULL para
 * evitar ponteiros pendentes (dangling pointers) e possíveis acessos
 * inválidos à memória.
 *
 * Essa função deve ser chamada ao final da execução de cada cenário
 * para garantir a correta liberação de recursos.
 *
 * @param problema Ponteiro para a estrutura `Problema` cujos recursos
 *        serão liberados.
 */
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

/**
 * @brief Função de comparação entre intervalos para ordenação no algoritmo guloso.
 *
 * Esta função define o critério de ordenação dos intervalos utilizado
 * pelo algoritmo guloso. Os intervalos são ordenados primeiramente
 * pelo valor de término (`fim`) em ordem crescente.
 *
 * Em caso de empate no valor de término, o desempate é feito pelo
 * valor de início (`inicio`), também em ordem crescente.
 *
 * Esse critério é fundamental para a estratégia gulosa adotada,
 * pois prioriza intervalos que terminam mais cedo, aumentando a
 * chance de cobrir o maior número de pontos com menos intervalos.
 *
 * A função é projetada para ser utilizada com a função `qsort`.
 *
 * @param a Ponteiro para o primeiro intervalo a ser comparado.
 * @param b Ponteiro para o segundo intervalo a ser comparado.
 *
 * @return Valor negativo se `a` deve vir antes de `b`,
 *         valor positivo se `a` deve vir depois de `b`,
 *         ou zero se ambos forem considerados equivalentes
 *         segundo o critério de ordenação.
 */
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

/**
 * @brief Compara dois pontos com base na posição no eixo.
 *
 * Ordena os pontos em ordem crescente de posição. Essa ordenação
 * facilita a identificação do próximo ponto ainda não coberto
 * durante a execução do algoritmo guloso.
 *
 * Função compatível com `qsort`.
 *
 * @param a Ponteiro para o primeiro ponto.
 * @param b Ponteiro para o segundo ponto.
 * @return Valor negativo, positivo ou zero conforme a ordem relativa.
 */
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

/**
 * @brief Configura o cenário pequeno do problema de cobertura de pontos.
 *
 * Inicializa um conjunto determinístico de 8 pontos e 10 intervalos,
 * garantindo que todos os pontos possam ser cobertos.
 *
 * Os intervalos são ordenados conforme o critério guloso antes da
 * execução do algoritmo.
 *
 * @param problema Ponteiro para a estrutura do problema.
 */
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

/**
 * @brief Configura o cenário médio do problema de cobertura de pontos.
 *
 * Inicializa um conjunto determinístico de 10 pontos e 12 intervalos,
 * com maior complexidade em relação ao cenário pequeno.
 *
 * Os intervalos são ordenados de acordo com o critério guloso.
 *
 * @param problema Ponteiro para a estrutura do problema.
 */
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

/**
 * @brief Configura o cenário grande do problema de cobertura de pontos.
 *
 * Inicializa um conjunto determinístico de 12 pontos e 15 intervalos,
 * representando o cenário mais complexo avaliado.
 *
 * Os intervalos são ordenados previamente para aplicação do algoritmo
 * guloso.
 *
 * @param problema Ponteiro para a estrutura do problema.
 */
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

/**
 * @brief Verifica se um ponto está coberto por um intervalo.
 *
 * Determina se a posição do ponto está dentro dos limites do intervalo,
 * incluindo as extremidades.
 *
 * @param ponto Estrutura representando o ponto.
 * @param intervalo Estrutura representando o intervalo.
 * @return 1 se o ponto estiver coberto, 0 caso contrário.
 */
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

/**
 * @brief Marca os pontos cobertos por um determinado intervalo.
 *
 * Atualiza o vetor de controle de cobertura, marcando como cobertos
 * todos os pontos ainda não cobertos que pertencem ao intervalo
 * selecionado.
 *
 * Também atualiza o contador total de pontos cobertos.
 *
 * @param problema Ponteiro para a estrutura do problema.
 * @param intervalo Intervalo selecionado pelo algoritmo guloso.
 */
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

/**
 * @brief Verifica se todos os pontos do problema já foram cobertos.
 *
 * Compara o número de pontos cobertos com o total de pontos existentes.
 *
 * @param problema Ponteiro para a estrutura do problema.
 * @return 1 se todos os pontos estiverem cobertos, 0 caso contrário.
 */
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

/**
 * @brief Obtém o índice do próximo ponto ainda não coberto.
 *
 * Percorre o vetor de pontos cobertos e retorna o índice do primeiro
 * ponto que ainda não foi coberto por nenhum intervalo selecionado.
 *
 * @param problema Ponteiro para a estrutura do problema.
 * @return Índice do ponto não coberto ou -1 se todos estiverem cobertos.
 */
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

/**
 * @brief Seleciona o melhor intervalo segundo a estratégia gulosa.
 *
 * A função avalia todos os intervalos que cobrem o ponto atualmente
 * não coberto e seleciona aquele que cobre o maior número de pontos
 * ainda descobertos.
 *
 * Em caso de empate, o intervalo de menor tamanho é escolhido como
 * critério de desempate.
 *
 * Essa estratégia busca maximizar o ganho local a cada escolha,
 * característica fundamental do algoritmo guloso.
 *
 * @param problema Ponteiro para a estrutura do problema.
 * @param indice_ponto Índice do ponto atualmente não coberto.
 * @return Índice do melhor intervalo ou -1 se nenhum for adequado.
 */
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

/**
 * @brief Resolve o problema da cobertura de pontos usando um algoritmo guloso.
 *
 * A estratégia gulosa consiste em:
 * - Selecionar o próximo ponto ainda não coberto
 * - Escolher o intervalo que cobre esse ponto e se estende o mais longe possível
 * - Marcar todos os pontos cobertos por esse intervalo
 *
 * O processo se repete até que todos os pontos estejam cobertos ou
 * não seja possível avançar.
 *
 * @param problema Ponteiro para a estrutura do problema
 * @return Estrutura contendo as métricas da execução
 */
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

/**
 * @brief Exibe a solução encontrada pelo algoritmo guloso.
 *
 * Mostra os intervalos selecionados e os pontos efetivamente cobertos.
 *
 * @param problema Ponteiro para a estrutura do problema
 */
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

/**
 * @brief Exibe as métricas do algoritmo guloso.
 *
 * Mostra tempo de execução, memória utilizada,
 * tamanho da solução e qualidade obtida.
 *
 * @param problema Ponteiro para a estrutura do problema
 */
void exibir_metricas(Problema *problema)
{
    printf("\n=== METRICAS DO ALGORITMO GULOSO ===\n");
    printf("Tempo de execucao: %.4f ms\n", problema->tempo_execucao);
    printf("Memoria utilizada: %ld KB\n", problema->memoria_utilizada);
    printf("Numero de intervalos na solucao: %d\n", problema->n_solucao);
    printf("Qualidade (1 - solucao/total): %.4f\n", problema->qualidade);
    printf("====================================\n\n");
}

/**
 * @brief Salva as métricas dos cenários em um arquivo CSV.
 *
 * O arquivo gerado permite a análise comparativa dos cenários
 * pequeno, médio e grande.
 *
 * @param metricas_pequeno Métricas do cenário pequeno
 * @param metricas_medio   Métricas do cenário médio
 * @param metricas_grande  Métricas do cenário grande
 */
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

/**
 * @brief Executa um teste individual do algoritmo guloso.
 *
 * Resolve o problema para um cenário específico,
 * exibindo a solução e suas métricas.
 *
 * @param problema Ponteiro para a estrutura do problema
 * @param nome_cenario Nome identificador do cenário
 * @param metricas Ponteiro para a estrutura de métricas
 */
void executar_teste(Problema *problema, const char *nome_cenario, Metricas *metricas)
{
    printf("\n=== EXECUTANDO CENARIO %s ===\n", nome_cenario);
    printf("Pontos: %d, Intervalos: %d\n", problema->n_pontos, problema->n_intervalos);

    *metricas = resolver_guloso(problema);

    exibir_solucao(problema);
    exibir_metricas(problema);
}

/**
 * @brief Executa todos os cenários disponíveis.
 *
 * Roda os cenários pequeno, médio e grande,
 * coleta as métricas e gera o arquivo CSV.
 */
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

/**
 * @brief Exibe o menu do algoritmo guloso.
 *
 * Permite ao usuário selecionar cenários,
 * executar todos os testes ou encerrar o programa.
 */
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

/**
 * @brief Função principal do programa.
 *
 * Controla o fluxo de execução do sistema,
 * exibindo o menu e processando as escolhas do usuário.
 *
 * @return Código de encerramento do programa
 */
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
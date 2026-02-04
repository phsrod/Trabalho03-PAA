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

/**
 * @brief Representa um intervalo numérico fechado.
 *
 * Esta estrutura define um intervalo contínuo na linha dos números inteiros,
 * indo do valor `inicio` até o valor `fim`. No contexto do problema de
 * cobertura de pontos, um intervalo é usado para verificar quais pontos
 * estão contidos dentro de seus limites.
 *
 * Um ponto é considerado coberto se sua posição satisfaz:
 * inicio ≤ posição ≤ fim.
 */
typedef struct
{
    int inicio;  /**< Limite inferior (início) do intervalo */
    int fim; /**< Limite superior (fim) do intervalo */
} Intervalo;

/**
 * @brief Representa um ponto em uma linha numérica.
 *
 * Esta estrutura modela um ponto identificado por um `id` e associado
 * a uma posição inteira na linha dos números. No problema de cobertura
 * de pontos, cada ponto deve ser coberto por pelo menos um intervalo.
 *
 * Um ponto é considerado coberto quando sua posição está dentro dos
 * limites de algum intervalo selecionado.
 */
typedef struct
{
    int id; /**< Identificador único do ponto (usado para exibição e controle) */
    int posicao; /**< Posição do ponto na linha numérica */
} Ponto;

/**
 * @brief Estrutura principal do problema de cobertura de pontos usando backtracking.
 *
 * Esta estrutura centraliza todos os dados necessários para a execução
 * do algoritmo de backtracking, incluindo:
 * - os pontos que devem ser cobertos;
 * - os intervalos disponíveis para cobertura;
 * - a solução em construção (parcial);
 * - a melhor solução encontrada até o momento;
 * - métricas de desempenho e qualidade.
 *
 * Ela permite organizar o estado do problema durante a busca na árvore
 * de soluções, facilitando a aplicação de podas e a coleta de métricas.
 */
typedef struct
{
    Intervalo *intervalos; /**< Vetor com todos os intervalos disponíveis */
    int n_intervalos; /**< Quantidade total de intervalos */
    Ponto *pontos;  /**< Vetor com os pontos que devem ser cobertos */
    int n_pontos;  /**< Quantidade total de pontos */
    int *pontos_cobertos; /**< Vetor auxiliar para marcar pontos já cobertos */
    int n_pontos_cobertos; /**< Quantidade de pontos atualmente cobertos */
    Intervalo *solucao_atual; /**< Conjunto de intervalos da solução parcial */
    int n_solucao_atual;  /**< Tamanho da solução parcial atual */
    Intervalo *melhor_solucao; /**< Melhor solução completa encontrada */
    int n_melhor_solucao; /**< Quantidade de intervalos da melhor solução */
    double tempo_execucao;  /**< Tempo total de execução do algoritmo (em ms) */
    long memoria_utilizada; /**< Memória máxima utilizada pelo processo (em KB) */
    double qualidade;  /**< Qualidade da solução encontrada */
    int nos_visitados; /**< Número de nós visitados na árvore de busca */
} ProblemaBacktracking;

/**
 * @brief Estrutura que armazena as métricas de desempenho do algoritmo de backtracking.
 *
 * Esta estrutura é utilizada para registrar e transportar os resultados
 * da execução do algoritmo, permitindo analisar seu desempenho em
 * diferentes cenários de teste (pequeno, médio e grande).
 *
 * As métricas incluem tempo de execução, uso de memória, qualidade da
 * solução encontrada e o esforço computacional medido pelo número de
 * nós visitados.
 */
typedef struct
{
    double tempo; /**< Tempo total de execução do algoritmo (em milissegundos) */
    long memoria; /**< Memória máxima utilizada durante a execução (em KB) */
    double qualidade;  /**< Qualidade da solução (1 - intervalos_usados / intervalos_totais) */
    int n_solucao;  /**< Número de intervalos da solução final encontrada */
    int nos_visitados; /**< Quantidade de nós visitados na árvore de busca */
} MetricasBacktracking;

/**
 * @brief Inicializa a estrutura do problema de backtracking.
 *
 * Esta função prepara a estrutura `ProblemaBacktracking` para uso,
 * garantindo que todos os ponteiros estejam nulos e que os contadores
 * e métricas iniciem com valores seguros.
 *
 * Em especial, o campo `n_melhor_solucao` é inicializado com `INT_MAX`
 * para permitir que qualquer solução válida encontrada seja considerada
 * melhor na primeira comparação.
 *
 * Essa inicialização é essencial antes de configurar cenários ou
 * executar o algoritmo de backtracking, evitando erros de memória
 * e comportamentos indefinidos.
 *
 * @param problema Ponteiro para a estrutura do problema a ser inicializada.
 */
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

/**
 * @brief Libera toda a memória alocada dinamicamente no problema de backtracking.
 *
 * Esta função é responsável por desalocar corretamente todos os vetores
 * alocados dinamicamente dentro da estrutura `ProblemaBacktracking`.
 * Após a liberação, os ponteiros são ajustados para `NULL`, evitando
 * acessos inválidos e possíveis erros de dupla liberação.
 *
 * Essa função deve ser chamada ao final da execução de cada cenário
 * ou quando o problema não for mais utilizado, garantindo boas
 * práticas de gerenciamento de memória em C.
 *
 * @param problema Ponteiro para a estrutura do problema cujos recursos
 *                 alocados devem ser liberados.
 */
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

/**
 * @brief Função de comparação de intervalos para ordenação.
 *
 * Esta função é utilizada pelo `qsort` para ordenar os intervalos
 * antes da execução do algoritmo de backtracking.
 *
 * O critério de ordenação é:
 * 1. Intervalos maiores vêm primeiro (ordem decrescente de tamanho);
 * 2. Em caso de empate, o intervalo com menor valor de início vem primeiro.
 *
 * A ordenação por tamanho é uma heurística importante, pois intervalos
 * maiores tendem a cobrir mais pontos, o que pode reduzir a profundidade
 * da árvore de busca e melhorar a eficiência das podas no backtracking.
 *
 * @param a Ponteiro genérico para o primeiro intervalo.
 * @param b Ponteiro genérico para o segundo intervalo.
 * @return Valor negativo se `a` deve vir antes de `b`,
 *         valor positivo se `a` deve vir depois de `b`,
 *         ou zero se ambos forem considerados equivalentes.
 */
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

/**
 * @brief Função de comparação de pontos para ordenação.
 *
 * Esta função é utilizada pelo `qsort` para ordenar os pontos
 * em ordem crescente de posição na linha numérica.
 *
 * A ordenação dos pontos facilita operações de verificação
 * de cobertura e análise dos resultados, tornando o
 * comportamento do algoritmo mais previsível e organizado.
 *
 * @param a Ponteiro genérico para o primeiro ponto.
 * @param b Ponteiro genérico para o segundo ponto.
 * @return Valor negativo se `a` deve vir antes de `b`,
 *         valor positivo se `a` deve vir depois de `b`,
 *         ou zero se ambos tiverem a mesma posição.
 */
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

/**
 * @brief Configura o cenário pequeno para o algoritmo de backtracking.
 *
 * Este cenário é determinístico e foi projetado para testes iniciais
 * e validação do funcionamento do algoritmo.
 *
 * Ele contém:
 * - 8 pontos fixos, distribuídos ao longo da linha numérica;
 * - 10 intervalos fixos, construídos de forma a garantir que todos
 *   os pontos possam ser cobertos.
 *
 * Por ser um cenário de tamanho reduzido, ele permite observar
 * com clareza o comportamento do backtracking, as decisões de poda
 * e a formação da solução ótima.
 *
 * Ao final da configuração, os intervalos são ordenados por tamanho
 * (do maior para o menor), utilizando uma heurística que tende a
 * melhorar o desempenho do algoritmo.
 *
 * @param problema Ponteiro para a estrutura do problema a ser configurada.
 */
void configurar_cenario_pequeno_backtracking(ProblemaBacktracking* problema) {
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
    
    qsort(problema->intervalos, problema->n_intervalos, sizeof(Intervalo), comparar_intervalos_backtracking);
}

/**
 * @brief Configura o cenário médio para o algoritmo de backtracking.
 *
 * Este cenário é determinístico e representa um nível intermediário
 * de complexidade em relação ao cenário pequeno.
 *
 * Ele contém:
 * - 10 pontos fixos, distribuídos de forma uniforme na linha numérica;
 * - 12 intervalos fixos, projetados para garantir a cobertura de todos
 *   os pontos.
 *
 * O cenário médio permite avaliar o impacto do aumento do espaço de
 * busca no desempenho do algoritmo, especialmente no número de nós
 * visitados e no tempo de execução.
 *
 * Assim como nos demais cenários, os intervalos são ordenados por
 * tamanho (ordem decrescente), utilizando uma heurística que tende a
 * melhorar a eficiência das podas no backtracking.
 *
 * @param problema Ponteiro para a estrutura do problema a ser configurada.
 */
void configurar_cenario_medio_backtracking(ProblemaBacktracking* problema) {
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
    
    qsort(problema->intervalos, problema->n_intervalos, sizeof(Intervalo), comparar_intervalos_backtracking);
}

/**
 * @brief Configura um cenário grande para o problema de cobertura de pontos usando backtracking.
 *
 * Este cenário é utilizado para testar o algoritmo em uma instância
 * de maior complexidade, com mais pontos e intervalos, aumentando
 * o espaço de busca explorado pelo backtracking.
 *
 * O cenário contém:
 * - 12 pontos fixos distribuídos ao longo do eixo numérico
 * - 15 intervalos fixos, todos capazes de cobrir os pontos
 *
 * Ao final da configuração, os intervalos são ordenados em ordem
 * decrescente de tamanho (e critério secundário de início),
 * o que pode ajudar a reduzir a profundidade da busca ao priorizar
 * intervalos mais abrangentes.
 *
 * @param problema Ponteiro para a estrutura do problema de backtracking
 */
void configurar_cenario_grande_backtracking(ProblemaBacktracking* problema) {
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
    
    qsort(problema->intervalos, problema->n_intervalos, sizeof(Intervalo), comparar_intervalos_backtracking);
}

/**
 * @brief Verifica se um ponto é coberto por um intervalo.
 *
 * Esta função testa se a posição de um ponto está contida
 * dentro dos limites de um intervalo fechado [início, fim].
 *
 * Ela é utilizada como operação básica do algoritmo de
 * backtracking para decidir se um ponto já foi coberto
 * por um determinado intervalo escolhido na solução.
 *
 * @param ponto Estrutura que representa o ponto a ser verificado.
 * @param intervalo Estrutura que representa o intervalo considerado.
 * @return 1 se o ponto estiver dentro do intervalo (coberto),
 *         ou 0 caso contrário.
 */
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

/**
 * @brief Verifica se a solução parcial atual cobre todos os pontos do problema.
 *
 * Esta função analisa os intervalos presentes na solução parcial
 * (`solucao_atual`) e verifica se, juntos, eles cobrem todos os
 * pontos definidos no problema.
 *
 * Para isso, é criado um vetor auxiliar de cobertura, onde cada
 * posição indica se um ponto já foi coberto (1) ou não (0).
 * Esse vetor é utilizado apenas temporariamente durante a verificação.
 *
 * A função é usada pelo algoritmo de backtracking para decidir
 * se uma solução parcial já é válida (cobre todos os pontos)
 * ou se ainda é necessário adicionar mais intervalos.
 *
 * @param problema Ponteiro para a estrutura que representa o problema.
 * @return 1 se todos os pontos estiverem cobertos pela solução atual,
 *         ou 0 caso contrário.
 */
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

/**
 * @brief Copia a solução atual para a melhor solução encontrada.
 *
 * Esta função é chamada quando o algoritmo de backtracking
 * encontra uma solução válida melhor do que a melhor solução
 * armazenada até o momento.
 *
 * Ela realiza uma cópia dos intervalos da solução atual
 * (`solucao_atual`) para o vetor `melhor_solucao`, garantindo
 * que a melhor combinação de intervalos seja preservada
 * mesmo após o retorno das chamadas recursivas.
 *
 * A memória para a melhor solução é alocada apenas uma vez,
 * na primeira vez em que uma solução válida é encontrada.
 *
 * @param problema Ponteiro para a estrutura que representa o problema.
 */
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

/**
 * @brief Função recursiva principal do algoritmo de backtracking.
 *
 * Esta função explora o espaço de soluções do problema de cobertura
 * de pontos, decidindo recursivamente se cada intervalo será
 * incluído ou não na solução atual.
 *
 * A cada chamada, o algoritmo:
 * - conta um novo nó visitado (métrica de complexidade);
 * - verifica condições de parada;
 * - aplica critérios de poda para reduzir o espaço de busca;
 * - testa se a solução parcial cobre todos os pontos;
 * - atualiza a melhor solução encontrada.
 *
 * @param problema Ponteiro para a estrutura que representa o problema.
 * @param indice_intervalo Índice do intervalo atualmente considerado.
 */
void backtracking_recursivo(ProblemaBacktracking *problema, int indice_intervalo)
{   
    /**
     * Incrementa o contador de nós visitados.
     * Essa métrica indica quantos estados da árvore de busca
     * foram efetivamente explorados pelo algoritmo.
     */
    problema->nos_visitados++;

    /**
     * Condição de parada:
     * se todos os intervalos já foram considerados,
     * não há mais decisões a serem tomadas.
     */
    if (indice_intervalo >= problema->n_intervalos)
    {
        return;
    }

    /**
     * Poda baseada na melhor solução encontrada até o momento.
     * Se a solução atual já é maior ou igual à melhor solução,
     * não faz sentido continuar explorando esse ramo.
     */
    if (problema->n_solucao_atual >= problema->n_melhor_solucao)
    {
        return;
    }

    /**
     * Verifica se a próxima escolha já torna impossível
     * melhorar a melhor solução atual.
     */
    int pode_podar = 0;
    if (problema->n_solucao_atual + 1 >= problema->n_melhor_solucao)
    {
        pode_podar = 1;
    }

    /**
     * Caso não seja possível podar, tenta incluir o intervalo atual
     * na solução.
     */
    if (pode_podar == 0)
    {   
        /**
         * Inclui o intervalo atual na solução parcial.
         */
        problema->solucao_atual[problema->n_solucao_atual] = problema->intervalos[indice_intervalo];
        problema->n_solucao_atual++;

        /**
         * Verifica se a solução parcial já cobre todos os pontos.
         */
        if (verificar_cobertura_parcial(problema))
        {   
            /**
             * Caso a solução seja válida e melhor que a anterior,
             * ela passa a ser a melhor solução encontrada.
             */
            if (problema->n_solucao_atual < problema->n_melhor_solucao)
            {
                copiar_solucao(problema);
            }
        }
        else
        {   
            /**
             * Caso ainda não cubra todos os pontos, continua a busca
             * considerando o próximo intervalo.
             */
            backtracking_recursivo(problema, indice_intervalo + 1);
        }

        /**
         * Remove o intervalo atual da solução (backtrack),
         * restaurando o estado anterior antes de testar
         * a próxima possibilidade.
         */
        problema->n_solucao_atual--;
    }

    /**
     * Explora o ramo onde o intervalo atual NÃO é incluído
     * na solução.
     */
    backtracking_recursivo(problema, indice_intervalo + 1);
}

/**
 * @brief Resolve o problema de cobertura de pontos utilizando backtracking.
 *
 * Esta função é responsável por:
 * - inicializar estruturas auxiliares do algoritmo;
 * - executar o algoritmo de backtracking recursivo;
 * - medir tempo de execução e uso de memória;
 * - calcular a qualidade da solução encontrada;
 * - retornar todas essas informações na forma de métricas.
 *
 * Ela funciona como a função principal do algoritmo de backtracking,
 * encapsulando a execução e a avaliação da solução.
 *
 * @param problema Ponteiro para a estrutura que representa o problema.
 * @return Estrutura contendo as métricas de desempenho e qualidade
 *         da solução encontrada.
 */
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

/**
 * @brief Exibe a melhor solução encontrada pelo algoritmo de backtracking.
 *
 * Esta função imprime:
 * - os intervalos que compõem a melhor solução encontrada;
 * - a quantidade total de intervalos utilizados;
 * - os pontos efetivamente cobertos por essa solução.
 *
 * Além de apresentar o resultado final, a função também
 * realiza uma verificação explícita de cobertura dos pontos,
 * tornando a saída mais didática e fácil de interpretar.
 *
 * @param problema Ponteiro para a estrutura que representa o problema.
 */
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

/**
 * @brief Exibe as métricas de desempenho do algoritmo de backtracking.
 *
 * Esta função imprime, de forma organizada, as principais métricas
 * coletadas durante a execução do algoritmo, permitindo avaliar
 * tanto o desempenho computacional quanto a qualidade da solução.
 *
 * As métricas apresentadas incluem:
 * - tempo total de execução;
 * - uso máximo de memória;
 * - número de intervalos da melhor solução encontrada;
 * - qualidade da solução;
 * - número de nós visitados na árvore de busca.
 *
 * @param problema Ponteiro para a estrutura que contém os dados
 *        e métricas do problema de backtracking.
 */
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

/**
 * @brief Salva em arquivo CSV as métricas do algoritmo de backtracking.
 *
 * Esta função cria um arquivo CSV contendo as métricas coletadas
 * durante a execução do algoritmo de backtracking para três cenários
 * distintos: pequeno, médio e grande.
 *
 * O arquivo gerado pode ser utilizado posteriormente para:
 * - análise comparativa de desempenho;
 * - geração de gráficos;
 * - inclusão direta em relatórios acadêmicos.
 *
 * O formato do arquivo CSV é:
 * cenario, tempo_ms, memoria_kb, qualidade, n_intervalos_solucao, nos_visitados
 *
 * @param metricas_pequeno Ponteiro para as métricas do cenário pequeno.
 * @param metricas_medio   Ponteiro para as métricas do cenário médio.
 * @param metricas_grande  Ponteiro para as métricas do cenário grande.
 */
void salvar_csv_backtracking(MetricasBacktracking *metricas_pequeno, MetricasBacktracking *metricas_medio, MetricasBacktracking *metricas_grande)
{
    FILE *arquivo = NULL;
    char caminho[MAX_PATH];

    snprintf(caminho, MAX_PATH, "../results/backtracking/file/metricas_backtracking.csv");

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

/**
 * @brief Executa um teste completo do algoritmo de backtracking para um cenário específico.
 *
 * Esta função organiza o fluxo de execução de um cenário de teste:
 * - Exibe informações iniciais do cenário (nome, quantidade de pontos e intervalos);
 * - Executa o algoritmo de backtracking;
 * - Armazena as métricas retornadas pela execução;
 * - Exibe a melhor solução encontrada;
 * - Exibe as métricas de desempenho do algoritmo.
 *
 * É uma função de alto nível, criada para fins experimentais e didáticos,
 * facilitando a comparação entre diferentes cenários de entrada.
 *
 * @param problema Ponteiro para a estrutura do problema de backtracking a ser resolvido.
 * @param nome_cenario String identificadora do cenário (ex.: "pequeno", "medio", "grande").
 * @param metricas Ponteiro para a estrutura onde as métricas da execução serão armazenadas.
 */
void executar_teste_backtracking(ProblemaBacktracking *problema, const char *nome_cenario, MetricasBacktracking *metricas)
{
    printf("\n=== EXECUTANDO CENARIO %s ===\n", nome_cenario);
    printf("Pontos: %d, Intervalos: %d\n", problema->n_pontos, problema->n_intervalos);

    *metricas = resolver_backtracking(problema);

    exibir_solucao_backtracking(problema);
    exibir_metricas_backtracking(problema);
}

/**
 * @brief Executa todos os cenários de teste do algoritmo de backtracking.
 *
 * Esta função coordena a execução completa dos experimentos do algoritmo
 * de backtracking, contemplando três cenários distintos:
 * - Pequeno
 * - Médio
 * - Grande
 *
 * O fluxo executado é:
 * - Inicialização das estruturas do problema;
 * - Configuração específica de cada cenário;
 * - Execução do algoritmo de backtracking para cada cenário;
 * - Exibição das soluções e métricas obtidas;
 * - Salvamento das métricas em arquivo CSV para análise posterior;
 * - Liberação de toda a memória alocada.
 *
 * A função foi projetada com foco didático e experimental, permitindo
 * a comparação do comportamento do algoritmo em diferentes escalas
 * de entrada.
 */
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

/**
 * @brief Exibe o menu interativo do algoritmo de backtracking.
 *
 * Esta função apresenta no terminal um menu de opções que permite
 * ao usuário selecionar qual cenário do Problema da Cobertura de Pontos
 * com Intervalos será executado utilizando o algoritmo de backtracking.
 *
 * As opções disponíveis incluem:
 * - Execução individual dos cenários pequeno, médio ou grande;
 * - Execução de todos os cenários em sequência, com geração de arquivo CSV
 *   contendo as métricas coletadas;
 * - Encerramento do programa.
 *
 * A função não realiza leitura de entrada nem processamento lógico,
 * sendo responsável exclusivamente pela exibição textual do menu.
 */

void exibir_menu_backtracking() {
    printf("\n=== PROBLEMA DA COBERTURA DE PONTOS COM INTERVALOS ===\n");
    printf("ALGORITMO: BACKTRACKING\n");
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
 * Controla a execução do Problema da Cobertura de Pontos com Intervalos
 * utilizando o algoritmo de backtracking por meio de um menu interativo.
 *
 * A função apresenta ao usuário opções para:
 * - Executar individualmente os cenários pequeno, médio ou grande;
 * - Executar todos os cenários em sequência e gerar um arquivo CSV com métricas;
 * - Encerrar a execução do programa.
 *
 * O fluxo principal consiste em:
 * - Exibir o menu de opções;
 * - Ler e validar a entrada do usuário;
 * - Inicializar, configurar, executar e liberar os recursos do problema
 *   conforme o cenário selecionado.
 *
 * A execução continua em loop até que o usuário escolha a opção de saída.
 *
 * @return Retorna 0 após o encerramento normal do programa.
 */
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
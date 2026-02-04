# 📋 Trabalho – Algoritmos Gulosos e Backtracking

**Disciplina:** Projeto e Análise de Algoritmos  
**Professor:** Raí Araújo de Miranda  
**Curso:** Sistemas de Informação  
**Período:** 5º  
**Ano/Semestre:** 2025.4  
**Instituição:** Universidade Federal do Piauí – UFPI
**Campus:** Senador Helvídio Nunes de Barros – Picos  

---

## 👥 Equipe (Tema 02)

| Nome | Responsabilidade |
|------|------------------|
| Tiago Lima de Moura | Implementação / Análise    |
| [Pedro Henrique Silva Rodrigues] | Implementação / Análise |
| [Luciano Sousa Barbosa] | Implementação / Análise |

> Todos os integrantes devem participar da apresentação e do desenvolvimento do código.

---

## 🎯 Objetivo do Trabalho

Comparar as estratégias de projeto de algoritmos **Gulosa** e **Tentativa e Erro (Backtracking)** aplicadas ao mesmo problema. A análise foca em três pilares fundamentais:
* **Qualidade da solução:** Diferença entre a solução ótima e a aproximada.
* **Desempenho:** Mensuração do tempo de execução.
* **Recursos:** Mensuração do consumo de memória.

---

## 🚀 Como Rodar os Testes

### 📋 Pré-requisitos
* **WSL2** configurado no Windows
* **Ubuntu 24.04.3 LTS** (ou similar) instalado no WSL
* **Compilador GCC** instalado no sistema
* **Valgrind** para medição de memória (Linux/WSL)

### 🔨 Compilação

No terminal WSL, navegue até a pasta raiz do projeto e compile os códigos:

```bash
# Compilar o algoritmo Backtracking
gcc src/coberturaBacktracking.c -o cb -lm

# Compilar o algoritmo Guloso
gcc src/coberturaGuloso.c -o cg -lm
```

### ▶️ Execução dos Testes

**Executar Backtracking:**
```bash
./cb
```

**Executar Guloso:**
```bash
./cg
```

Após iniciar, escolha uma das opções do menu:
* **1, 2 ou 3:** Executa um cenário específico (Pequeno, Médio ou Grande)
* **4:** Executa TODOS os cenários e gera o arquivo CSV com métricas
* **5:** Sair

### 📊 Medição de Memória com Valgrind

Para obter dados precisos de consumo de memória, execute com valgrind:

```bash
# Backtracking
valgrind --leak-check=full ./cb

# Guloso
valgrind --leak-check=full ./cg
```

**Dica:** Procure pela linha `total heap usage` na saída do valgrind para ver a quantidade de bytes alocados.

### 📁 Arquivos de Saída

Os resultados são salvos automaticamente em:
* `results/backtracking/file/metricas_backtracking.csv`
* `results/guloso/file/metricas_guloso.csv`

Cada CSV contém as colunas:
* **cenario:** nome do cenário (pequeno, medio, grande)
* **tempo_ms:** tempo de execução em milissegundos
* **memoria_kb:** uso de memória (use valgrind para preencher)
* **qualidade:** métrica de qualidade da solução (1 - n_solucao/n_intervalos)
* **n_intervalos_solucao:** quantidade de intervalos usados
* **nos_visitados:** *(apenas backtracking)* nós explorados na árvore de busca

---

## 🧠 Tema Sorteado

**Tema 02:** Cobertura de pontos com intervalos (minimizar $n^{\circ}$ de intervalos).  
**Problema:** Cobrir pontos de uma reta com o menor número possível de intervalos.  

**Técnicas Comparadas:**
* **Algoritmo Guloso:** Escolhas locais imediatas buscando a otimização.
* **Backtracking (Tentativa e Erro):** Exploração sistemática de possibilidades para garantir a solução ótima.

---

## 💻 Ambiente de Desenvolvimento

| Item | Especificação |
|------|---------------|
| **Linguagem** | C  |
| **Sistema Operacional** | Windows 11 (Executado no Ubuntu 22.04.5 LTS via WSL2) |
| **Hardware** | Intel Core i3-1215U (12ª Gen, 6 núcleos, 8 threads, 1.20 GHz), 8 GB RAM |
| **Editor/IDE** | Visual Studio Code |

---

## 🧩 Descrição do Problema

O problema consiste em receber um conjunto de pontos em uma reta e determinar a quantidade mínima de intervalos de comprimento fixo necessários para que todos os pontos estejam contidos em pelo menos um intervalo.

---

## 🧪 Metodologia Experimental

### 🔹 Implementações
* As duas abordagens devem ser implementadas na **mesma linguagem de programação**.
* O código deve ser organizado, legível e possuir instruções claras para execução.

### 🔹 Cenários de Teste
* **Entradas Pequenas:** Ambas as versões testadas com o mesmo conjunto para comparação de qualidade.
* **Entradas Maiores:** Caso o backtracking se torne inviável (custo exponencial), serão utilizadas configurações diferentes com a devida justificativa técnica.

### 🔹 Métricas Avaliadas (Medições Obrigatórias)
* ⏱️ **Tempo de execução**.
* 💾 **Memória consumida**.
* 🎯 **Qualidade da solução** (Comparação entre o valor final obtido por cada método).

---

## 📊 Resultados e Análise

O trabalho contém tabelas e gráficos comparativos abordando:
* Tempo x Tamanho da entrada.
* Memória x Tamanho da entrada.
* Qualidade da solução x Tamanho da entrada.

---

## 🎤 Apresentação

* **Data:** 04/02/2026.
* **Duração:** 15 a 30 minutos.
* **Demonstração:** Obrigatória com exemplos pequenos para mostrar o funcionamento real dos algoritmos (Vale 3 pontos).
* **Defesa:** Um aluno será sorteado na hora para demonstrar a implementação.

---

## 📁 Estrutura do Projeto

```text
TRABALHO03-PAA/
├── docs/                        # Documentação e materiais de suporte
|   └── Apresentação de Slides  
├── results/                     # Resultados e análises experimentais
│   ├── backtracking/            # Dados da abordagem Backtracking
│   │   ├── file/                # Arquivos de saída/logs
│   │   └── graphics/            # Gráficos gerados para Backtracking
│   └── guloso/                  # Dados da abordagem Gulosa
│       ├── file/                # Arquivos de saída/logs
│       └── graphics/            # Gráficos gerados para Guloso
├── src/                         # Código-fonte do projeto
│   ├── coberturaBacktracking.c  # Implementação em C (Backtracking)
│   ├── coberturaGuloso.c        # Implementação em C (Algoritmo Guloso)
│   └── gerar_graficos.c         # Script Python para geração de gráficos
├── .gitignore                   # Configuração de arquivos ignorados pelo Git
├── LICENSE                      # Licença do projeto
└── README.md                    # Este documento
```

---

## ✅ Checklist de Progresso

x -> para marcar

### 🔧 Implementação
- [x] Escolha da linguagem de programação única para ambas as versões.
- [x] Organização do código em pastas e arquivos coerentes.
- [x] Implementação do Algoritmo Guloso (estratégia de escolha local).
- [x] Implementação do Algoritmo de Backtracking (tentativa e erro).
- [ ] Garantia de legibilidade e instruções claras para execução.

### 🧪 Testes e Medições
- [x] Definição de um conjunto de testes comum para ambas as versões.
- [x] Medição do tempo de execução para diferentes tamanhos de entrada.
- [x] Medição do consumo de memória para diferentes tamanhos de entrada.
- [x] Coleta de dados sobre a qualidade da solução (valor ótimo vs. aproximado).
- [x] Justificativa para possíveis limitações de testes no backtracking em entradas grandes.

### 📊 Análise e Documentação
- [x] Criação de tabelas comparativas (Tempo, Memória e Qualidade).
- [x] Geração de gráficos: Tempo x Tamanho da Entrada.
- [x] Geração de gráficos: Memória x Tamanho da Entrada.
- [x] Geração de gráficos: Qualidade da Solução x Tamanho da Entrada.
- [x] Análise crítica: A solução gulosa é ótima ou apenas boa?.
- [x] Análise crítica: O backtracking encontra a solução ótima?.

### 📝 Apresentação e Entrega
- [x] Elaboração de slides com a explicação do problema e abordagens.
- [x] Inclusão dos resultados experimentais (tabelas e gráficos) nos slides.
- [ ] Preparação da demonstração obrigatória com exemplos pequenos (vale 3 pontos).
- [ ] Revisão do conteúdo para garantir clareza e profundidade.
- [ ] Divisão equilibrada da fala entre todos os integrantes.

---

## 📌 Critérios de Avaliação

A nota total de 10 pontos será distribuída da seguinte forma :

* **Apresentação, Estrutura e Organização (70%):** 
  * Clareza, profundidade e domínio do conteúdo .
  * Qualidade visual dos slides e organização do conteúdo .
  * Gestão do tempo e participação equilibrada de todos os membros .
* **Demonstração da Implementação (30%):** 
  * Um integrante do grupo será sorteado na hora para realizar a defesa .
  * Organização, clareza e legibilidade do código .
  * Execução correta e funcionamento dos algoritmos com exemplos práticos .
  * Reprodutibilidade (instruções claras para rodar o projeto) .

---

## 🔗 Links

* 📊 **Slides:** [https://docs.google.com/presentation/d/1272kWK6xde8cYom5E9TRkFTx6LDDNSe_ajNH-1xo99c/edit?usp=sharing]
* 💻 **Repositório:** [https://github.com/phsrod/Trabalho03-PAA]

---

**📌 Última atualização:** 04/02/2026

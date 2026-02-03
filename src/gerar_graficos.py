import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path


def carregar_dados(caminho_csv: Path) -> pd.DataFrame:
    if not caminho_csv.exists():
        raise FileNotFoundError(f"Arquivo não encontrado: {caminho_csv}")
    return pd.read_csv(caminho_csv)


def gerar_graficos_implantacao(
    df: pd.DataFrame,
    metricas: list[str],
    nome_implantacao: str,
    pasta_saida: Path
):
    pasta_saida.mkdir(parents=True, exist_ok=True)

    for metrica in metricas:
        if metrica not in df.columns:
            continue

        plt.figure()
        plt.plot(df["cenario"], df[metrica], marker="o")

        plt.xlabel("Cenário")
        plt.ylabel(metrica)
        plt.title(f"{metrica} - {nome_implantacao.capitalize()}")
        plt.grid(True)

        plt.savefig(pasta_saida / f"{metrica}.png", bbox_inches="tight")
        plt.close()


def main():
    raiz_projeto = Path(__file__).resolve().parent.parent
    pasta_results = raiz_projeto / "results"

    implementacoes = {
        "guloso": {
            "csv": pasta_results / "guloso/file" / "metricas_guloso.csv",
            "metricas": [
                "tempo_ms",
                "memoria_kb",
                "qualidade",
                "n_intervalos_solucao",
            ],
        },
        "backtracking": {
            "csv": pasta_results / "backtracking/file" / "metricas_backtracking.csv",
            "metricas": [
                "tempo_ms",
                "memoria_kb",
                "qualidade",
                "n_intervalos_solucao",
                "nos_visitados",
            ],
        },
    }

    for nome, info in implementacoes.items():
        df = carregar_dados(info["csv"])
        pasta_graficos = pasta_results / nome / "graphics"

        gerar_graficos_implantacao(
            df,
            info["metricas"],
            nome,
            pasta_graficos
        )

    print("Gráficos gerados com sucesso.")


if __name__ == "__main__":
    main()

import subprocess
import os
import glob
import re
import matplotlib.pyplot as plt
import sys

# Configurações
EXECUTAVEL = "trab_grafos.exe" if os.name == 'nt' else "./trab_grafos"
PASTA_GRAFOS = "grafos"
CMD_COMPILAR = ["make"]

def compilar_codigo():
    print("Compilando projeto com Make...")
    try:
        # Tenta rodar 'make clean' antes para garantir (opcional)
        if os.name == 'nt':
            subprocess.run(["make", "clean"], shell=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        
        resultado = subprocess.run(CMD_COMPILAR, shell=True, capture_output=True, text=True)
        
        if resultado.returncode != 0:
            print("Erro na compilação:")
            print(resultado.stderr)
            sys.exit(1)
            
        if not os.path.exists(EXECUTAVEL):
            print(f"O executável '{EXECUTAVEL}' não foi encontrado após a compilação.")
            sys.exit(1)
            
        print("Compilação com sucesso.")
        
    except FileNotFoundError:
        print("Erro: O comando 'make' não foi encontrado. Instale o Make ou MinGW.")
        sys.exit(1)

def ler_instancias():
    # Procura pares de arquivos NodesX.csv e EdgesX.csv
    padrao_nos = os.path.join(PASTA_GRAFOS, "Nodes*.csv")
    arquivos_nos = glob.glob(padrao_nos)
    instancias = []

    for arq_nos in arquivos_nos:
        # Tenta achar o Edges correspondente
        base = os.path.basename(arq_nos)
        numero = re.search(r'\d+', base).group()
        arq_arestas = os.path.join(os.path.dirname(arq_nos), f"Edges{numero}.csv")
        
        if os.path.exists(arq_arestas):
            instancias.append((int(numero), arq_nos, arq_arestas))
    
    # Ordena pelo número da instância
    instancias.sort(key=lambda x: x[0])
    return instancias

def extrair_tempo(saida, algoritmo):
    # Procura pela marcação de tempo
    regex = rf"--> Tempo \({algoritmo}\): (\d+\.\d+)"
    match = re.search(regex, saida)
    if match:
        return float(match.group(1))
    return None

def main():
    compilar_codigo()
    
    instancias = ler_instancias()
    if not instancias:
        # Tenta na pasta atual se não achou na pasta 'grafos'
        global PASTA_GRAFOS
        PASTA_GRAFOS = "."
        instancias = ler_instancias()
        if not instancias:
            print("Nenhuma instância encontrada (NodesX.csv e EdgesX.csv).")
            return

    print(f"\nEncontradas {len(instancias)} instâncias.")
    
    resultados_kruskal = []
    resultados_prim = []
    eixo_x = []

    print(f"{'Instância':<10} | {'Kruskal (s)':<12} | {'Prim (s)':<12}")
    print("-" * 40)

    for num, nos, arestas in instancias:
        try:
            cmd = [EXECUTAVEL, nos, arestas]
            # No Windows, pode precisar de shell=True se o executável não for achado direto
            processo = subprocess.run(cmd, capture_output=True, text=True)
            
            if processo.returncode != 0:
                print(f"Erro na instância {num}: {processo.stderr}")
                continue

            tempo_k = extrair_tempo(processo.stdout, "Kruskal")
            tempo_p = extrair_tempo(processo.stdout, "Prim")
            
            if tempo_k is not None and tempo_p is not None:
                print(f"{num:<10} | {tempo_k:<12.4f} | {tempo_p:<12.4f}")
                resultados_kruskal.append(tempo_k)
                resultados_prim.append(tempo_p)
                eixo_x.append(f"Inst {num}")
            else:
                print(f"Falha ao ler tempos da instância {num}")

        except Exception as e:
            print(f"Erro ao executar instância {num}: {e}")

    # Gerar Gráfico
    if resultados_kruskal:
        plt.figure(figsize=(10, 6))
        plt.plot(eixo_x, resultados_kruskal, marker='o', label='Kruskal', color='blue')
        plt.plot(eixo_x, resultados_prim, marker='s', label='Prim', color='red', linestyle='--')
        
        plt.title('Comparativo de Desempenho: Prim vs Kruskal')
        plt.ylabel('Tempo (segundos)')
        plt.xlabel('Instâncias')
        plt.legend()
        plt.grid(True)
        
        nome_grafico = "grafico_desempenho.png"
        plt.savefig(nome_grafico)
        print(f"\nGráfico salvo como '{nome_grafico}'")
        plt.show()

if __name__ == "__main__":
    main()
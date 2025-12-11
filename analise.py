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
        print("Erro: O comando 'make' não foi encontrado.")
        sys.exit(1)

def ler_instancias():
    padrao_nos = os.path.join(PASTA_GRAFOS, "Nodes*.csv")
    arquivos_nos = glob.glob(padrao_nos)
    instancias = []

    for arq_nos in arquivos_nos:
        base = os.path.basename(arq_nos)
        numero = re.search(r'\d+', base).group()
        arq_arestas = os.path.join(os.path.dirname(arq_nos), f"Edges{numero}.csv")
        
        if os.path.exists(arq_arestas):
            instancias.append((int(numero), arq_nos, arq_arestas))
    
    instancias.sort(key=lambda x: x[0])
    return instancias

# ========= EXTRATORES ==========

def extrair_tempo(saida, algoritmo):
    regex = rf"--> Tempo \({algoritmo}\): (\d+\.\d+)"
    match = re.search(regex, saida)
    if match:
        return float(match.group(1))
    return None

def extrair_memoria(saida, algoritmo):
    regex = rf"--> Memória Usada \({algoritmo}\): (\d+)"
    match = re.search(regex, saida)
    if match:
        return int(match.group(1))
    return None

# ============================================

def main():
    compilar_codigo()
    
    instancias = ler_instancias()
    if not instancias:
        global PASTA_GRAFOS
        PASTA_GRAFOS = "."
        instancias = ler_instancias()
        if not instancias:
            print("Nenhuma instância encontrada (NodesX.csv / EdgesX.csv).")
            return

    print(f"\nEncontradas {len(instancias)} instâncias.")
    
    resultados_kruskal = []
    resultados_prim = []
    memoria_kruskal = []
    memoria_prim = []
    eixo_x = []

    print(f"{'Instância':<10} | {'Kruskal (s)':<12} | {'Prim (s)':<12} | {'Mem K (kB)':<12} | {'Mem P (kB)':<12}")
    print("-" * 70)

    for num, nos, arestas in instancias:
        try:
            cmd = [EXECUTAVEL, nos, arestas]
            processo = subprocess.run(cmd, capture_output=True, text=True)
            
            if processo.returncode != 0:
                print(f"Erro na instância {num}: {processo.stderr}")
                continue

            saida = processo.stdout

            tempo_k = extrair_tempo(saida, "Kruskal")
            tempo_p = extrair_tempo(saida, "Prim")

            mem_k = extrair_memoria(saida, "Kruskal")
            mem_p = extrair_memoria(saida, "Prim")
            
            if None not in (tempo_k, tempo_p, mem_k, mem_p):
                print(f"{num:<10} | {tempo_k:<12.4f} | {tempo_p:<12.4f} | {mem_k:<12} | {mem_p:<12}")
                resultados_kruskal.append(tempo_k)
                resultados_prim.append(tempo_p)
                memoria_kruskal.append(mem_k)
                memoria_prim.append(mem_p)
                eixo_x.append(f"Inst {num}")
            else:
                print(f"Falha ao ler dados da instância {num}")

        except Exception as e:
            print(f"Erro ao executar instância {num}: {e}")

    # =======================
    # GRÁFICO DE TEMPO
    # =======================
    if resultados_kruskal:
        plt.figure(figsize=(10, 6))
        plt.plot(eixo_x, resultados_kruskal, marker='o', label='Kruskal', color='blue')
        plt.plot(eixo_x, resultados_prim, marker='s', label='Prim', color='red', linestyle='--')
        
        plt.title('Comparativo de Tempo: Prim vs Kruskal')
        plt.ylabel('Tempo (segundos)')
        plt.xlabel('Instâncias')
        plt.legend()
        plt.grid(True)
        
        plt.savefig("grafico_tempo.png")
        print("\nGráfico de tempo salvo como 'grafico_tempo.png'")
        plt.show()

    # =======================
    # GRÁFICO DE MEMÓRIA
    # =======================
    if memoria_kruskal:
        plt.figure(figsize=(10, 6))
        plt.plot(eixo_x, memoria_kruskal, marker='o', label='Kruskal', color='blue')
        plt.plot(eixo_x, memoria_prim, marker='s', label='Prim', color='red', linestyle='--')
        
        plt.title('Uso de Memória: Prim vs Kruskal')
        plt.ylabel('Memória (kB)')
        plt.xlabel('Instâncias')
        plt.legend()
        plt.grid(True)
        
        plt.savefig("grafico_memoria.png")
        print("Gráfico de memória salvo como 'grafico_memoria.png'")
        plt.show()

if __name__ == "__main__":
    main()

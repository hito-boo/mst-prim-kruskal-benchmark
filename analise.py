import os
import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import re

# Configurações
GRAFOS_DIR = "grafos"
EXECUTAVEL = "./main"
LOG_FILE = "resultados.csv"

def compilar_c():
    """Compila o código C com flags de otimização."""
    print("Compilando código C...")
    cmd = ["gcc", "-O2", "-Wall", "-Wextra", "main.c", "-o", "main", "-lm"]
    ret = subprocess.run(cmd)
    if ret.returncode != 0:
        print("Erro na compilação.")
        exit(1)
    print("Compilação com sucesso.\n")

def natural_sort_key(s):
    """Para ordenar arquivos como Edges1, Edges2, Edges10 corretamente."""
    return [int(text) if text.isdigit() else text.lower()
            for text in re.split('([0-9]+)', s)]

def rodar_experimentos():
    resultados = []
    
    arquivos = os.listdir(GRAFOS_DIR)
    edges_files = sorted([f for f in arquivos if f.startswith("Edges") and f.endswith(".csv")], 
                         key=natural_sort_key)
    
    print(f"Encontrados {len(edges_files)} arquivos de arestas.\n")

    erros_validacao = []
    grafos_desconexos = []

    for edge_file in edges_files:
        match = re.search(r'Edges(\d+)\.csv', edge_file)
        if not match: 
            continue
        
        num_id = match.group(1)
        node_file = f"Nodes{num_id}.csv"
        
        path_edge = os.path.join(GRAFOS_DIR, edge_file)
        path_node = os.path.join(GRAFOS_DIR, node_file)
        
        if not os.path.exists(path_node):
            print(f"⚠ Arquivo {node_file} não encontrado para {edge_file}. Pulando.")
            continue

        print(f"Processando Instância {num_id}...", end=" ", flush=True)
        
        try:
            result = subprocess.run([EXECUTAVEL, path_node, path_edge], 
                                    capture_output=True, text=True, timeout=300)
            
            # Captura avisos e erros do stderr
            if result.stderr:
                stderr_lines = result.stderr.strip().split('\n')
                for line in stderr_lines:
                    if 'desconexo' in line.lower():
                        grafos_desconexos.append((num_id, line))
            
            if result.returncode != 0:
                print("❌ FALHOU")
                erros_validacao.append((num_id, "Código de retorno != 0"))
                continue

            # Saída esperada: V,E,CostPrim,TimePrim,CostKruskal,TimeKruskal,IsConnected,ValidationPassed
            output = result.stdout.strip()
            
            parts = output.split(',')
            if len(parts) != 8:
                print(f"❌ Formato de saída inválido")
                continue

            v, e, c_prim, t_prim, c_kruskal, t_kruskal, is_connected, validation = parts
            
            validation_passed = int(validation)
            is_connected_bool = int(is_connected)
            
            resultados.append({
                "Instancia": int(num_id),
                "V": int(v),
                "E": int(e),
                "Custo_Prim": float(c_prim),
                "Tempo_Prim": float(t_prim),
                "Custo_Kruskal": float(c_kruskal),
                "Tempo_Kruskal": float(t_kruskal),
                "Conexo": is_connected_bool,
                "Validacao_OK": validation_passed
            })
            
            if validation_passed:
                status = "✓ OK"
                if not is_connected_bool:
                    status += " (desconexo)"
            else:
                status = "❌ FALHOU VALIDAÇÃO"
                erros_validacao.append((num_id, "Custos divergentes"))
            
            print(status)
            
        except subprocess.TimeoutExpired:
            print("❌ TIMEOUT (>5min)")
            erros_validacao.append((num_id, "Timeout"))
        except Exception as ex:
            print(f"❌ Exceção: {ex}")
            erros_validacao.append((num_id, str(ex)))

    # Salvar Log
    df = pd.DataFrame(resultados)
    df.to_csv(LOG_FILE, index=False)
    print(f"\n{'='*60}")
    print(f"Resultados salvos em {LOG_FILE}")
    print(f"{'='*60}\n")
    
    # Relatório de validação
    print("📊 RELATÓRIO DE VALIDAÇÃO")
    print(f"{'='*60}")
    print(f"Total de instâncias processadas: {len(resultados)}")
    
    if len(resultados) > 0:
        validacoes_ok = df['Validacao_OK'].sum()
        grafos_conexos = df['Conexo'].sum()
        
        print(f"Validações bem-sucedidas: {validacoes_ok}/{len(resultados)}")
        print(f"Grafos conexos: {grafos_conexos}/{len(resultados)}")
        print(f"Grafos desconexos: {len(resultados) - grafos_conexos}/{len(resultados)}")
        
        if validacoes_ok < len(resultados):
            print(f"\n⚠ ATENÇÃO: {len(resultados) - validacoes_ok} instância(s) falharam na validação!")
            print("\nInstâncias com erro:")
            for inst_id, erro in erros_validacao:
                print(f"  - Instância {inst_id}: {erro}")
        else:
            print("\n✓ Todas as instâncias passaram na validação!")
        
        if grafos_desconexos:
            print(f"\nGrafos desconexos detectados: {len(grafos_desconexos)}")
            for inst_id, msg in grafos_desconexos:
                print(f"  - Instância {inst_id}")
    
    print(f"{'='*60}\n")
    
    return df

def gerar_graficos(df):
    if df.empty:
        print("Sem dados para gerar gráficos.")
        return

    # Separar grafos conexos e desconexos
    df_conexos = df[df['Conexo'] == 1].copy()
    df_desconexos = df[df['Conexo'] == 0].copy()
    
    print(f"Gerando gráficos...")
    print(f"  - Grafos conexos: {len(df_conexos)}")
    print(f"  - Grafos desconexos: {len(df_desconexos)}")

    # Ordenar por tamanho
    df_conexos = df_conexos.sort_values(by="V")
    
    # GRÁFICO 1: Tempo x Tamanho (apenas grafos conexos)
    if not df_conexos.empty:
        plt.figure(figsize=(12, 6))
        plt.plot(df_conexos["V"], df_conexos["Tempo_Prim"], 
                marker='o', label='Prim (Heap)', linewidth=2, markersize=6)
        plt.plot(df_conexos["V"], df_conexos["Tempo_Kruskal"], 
                marker='s', label='Kruskal (Union-Find)', linewidth=2, markersize=6)
        plt.title('Comparação de Desempenho: Prim vs Kruskal (Grafos Conexos)', fontsize=14, fontweight='bold')
        plt.xlabel('Número de Vértices (V)', fontsize=12)
        plt.ylabel('Tempo de Execução (s)', fontsize=12)
        plt.legend(fontsize=11)
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig("grafico_tempo_v.png", dpi=300)
        print("✓ Gráfico 'grafico_tempo_v.png' gerado.")
    
    # GRÁFICO 2: Validação de Custos
    df_custos = df.sort_values(by="Instancia")
    
    plt.figure(figsize=(12, 6))
    plt.plot(df_custos["Instancia"], df_custos["Custo_Prim"], 
            label='Custo Prim', linewidth=4, alpha=0.6, color='blue')
    plt.plot(df_custos["Instancia"], df_custos["Custo_Kruskal"], 
            label='Custo Kruskal', linestyle='--', linewidth=2, color='red')
    
    # Marcar grafos desconexos
    if not df_desconexos.empty:
        plt.scatter(df_desconexos["Instancia"], df_desconexos["Custo_Prim"],
                   color='orange', s=100, marker='x', label='Desconexo', zorder=5)
    
    plt.title('Validação de Corretude (Custos devem ser idênticos)', fontsize=14, fontweight='bold')
    plt.xlabel('ID da Instância', fontsize=12)
    plt.ylabel('Custo da MST', fontsize=12)
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig("grafico_custos.png", dpi=300)
    print("✓ Gráfico 'grafico_custos.png' gerado.")
    
    # GRÁFICO 3: Análise de diferença de custos (se houver)
    df_custos['Diferenca'] = abs(df_custos['Custo_Prim'] - df_custos['Custo_Kruskal'])
    max_diff = df_custos['Diferenca'].max()
    
    if max_diff > 0.01:
        plt.figure(figsize=(12, 6))
        plt.bar(df_custos["Instancia"], df_custos["Diferenca"], color='red', alpha=0.7)
        plt.axhline(y=0.01, color='green', linestyle='--', label='Tolerância (0.01)')
        plt.title('Diferença Absoluta entre Custos Prim e Kruskal', fontsize=14, fontweight='bold')
        plt.xlabel('ID da Instância', fontsize=12)
        plt.ylabel('|Custo_Prim - Custo_Kruskal|', fontsize=12)
        plt.legend(fontsize=11)
        plt.grid(True, alpha=0.3, axis='y')
        plt.tight_layout()
        plt.savefig("grafico_diferencas.png", dpi=300)
        print("✓ Gráfico 'grafico_diferencas.png' gerado.")
        print(f"  ⚠ Diferença máxima encontrada: {max_diff:.6f}")

    print("\n✓ Todos os gráficos foram gerados com sucesso!")

if __name__ == "__main__":
    compilar_c()
    dados = rodar_experimentos()
    gerar_graficos(dados)
# Variáveis de Compilação
CC = gcc
CFLAGS = -Wall -Wextra -g    # -Wall e -Wextra para avisos, -g para debug
LDFLAGS = -lm                # Linker flags (necessário para math.h)

# Arquivos-fonte do projeto
SRC = main.c grafo.c heap.c union_find.c algoritmos.c

# Gera a lista de objetos (.o) substituindo .c por .o
OBJ = $(SRC:.c=.o)

# Nome do executável final
EXEC = trab_grafos

# Regras de compilação

# Regra padrão
all: $(EXEC)

# Regra para linkar os objetos e criar o executável
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Regra genérica para compilar qualquer .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Comandos utilitários

# Limpa os arquivos temporários e o executável
clean:
	del *.o *.exe

# Atalho para compilar e rodar com os arquivos de teste (ajustar nomes se necessário)
run: all
	./$(EXEC) Nodes1.csv Edges1.csv
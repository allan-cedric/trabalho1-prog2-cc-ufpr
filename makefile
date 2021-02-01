CC = gcc # Compilador
CFLAGS = -Wall -Wextra -g # Flags de compilação
LDLIBS = -lm # Ligação das bibliotecas
OBJS = mosaico.o parsing/parsing.o	# Arquivos objetos

# Regra padrão
all: mosaico

# Regras de ligação
mosaico: $(OBJS)

# Regras de compilação
mosaico.o: mosaico.c parsing/parsing.h
parsing/parsing.o: parsing/parsing.c parsing/parsing.h

# Remove arquivos temporários
clean:
	-rm -f $(OBJS) *~

# Remove tudo o que não for o código-fonte
purge: clean
	-rm -f mosaico
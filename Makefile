CC = gcc
CFLAGS = -Wall

all: shell

shell: shell.c
	$(CC) $(CFLAGS) -o shell shell.c

clean:
	rm -f shell

test: shell
	@echo "=== Teste 1: comando simples ==="
	echo "ls" | ./shell
	@echo "=== Teste 2: multiplos comandos ==="
	echo "ls ; pwd ; echo hello" | ./shell
	@echo "=== Teste 3: pipe ==="
	echo "ls -l | sort -k5n" | ./shell
	@echo "=== Teste 4: redirecionamento ==="
	echo "date > /tmp/teste.txt" | ./shell
	cat /tmp/teste.txt
	@echo "=== Teste 5: batch ==="
	./shell teste.txt

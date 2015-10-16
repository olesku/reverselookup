# vim: set noet
CC=gcc

reverselookup:reverselookup.c
	$(CC) reverselookup.c -o reverselookup

clean:
	rm -f reverselookup

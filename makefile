typtest: typtest.c
	clang -Wall typtest.c -o /usr/bin/typetest -lncurses -lcyaml

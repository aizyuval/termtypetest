typtest: typtest.c
	clang -Wall typtest.c schema/schema.c -o /usr/bin/typetest -lncurses -lcyaml

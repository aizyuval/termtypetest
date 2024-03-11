typtest: typtest.c
	gcc -Wall typtest.c schema/schema.c -o /usr/bin/typetest -lncurses -lcyaml

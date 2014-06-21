bison -d -t -v syntax.y
flex lexicalTest.l
gcc main.c M_allocate.c syntax.tab.c util.c semantic/sem.c semantic/symbol.c semantic/printerr.c A_construct.c -lfl -g -o parser

bison -d -t -v syntax.y
flex lexicalTest.l
gcc test_var.c syntax.tab.c util.c semantic/sem.c semantic/symbol.c semantic/printerr.c -lfl -g -o parser

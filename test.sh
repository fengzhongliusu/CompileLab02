bison -d -t -v syntax.y
flex lexicalTest.l
gcc main.c syntax.tab.c util.c -lfl -g -o scanner

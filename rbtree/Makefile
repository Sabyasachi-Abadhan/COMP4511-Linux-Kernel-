all: rbtree_main.o rbtree.o rbtree_print.o
	gcc -o rbtree rbtree_main.o rbtree.o rbtree_print.o

rbtree.o: rbtree.c
	gcc -c rbtree.c

rbtree_print.o: rbtree_print.c
	gcc -c rbtree_print.c

rbtree_main.o: rbtree_main.c
	gcc -c rbtree_main.c

clean:
	rm *.o rbtree

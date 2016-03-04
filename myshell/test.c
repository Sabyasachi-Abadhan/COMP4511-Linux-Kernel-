#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

int main( void )
{
char *dup = strdup(getenv("PATH"));
char *s = dup;
char *p = NULL;
do {
    printf("%s\n", s);
    p = strchr(s, ':');
    if (p != NULL) {
        p[0] = 0;
    }
    printf("Path in $PATH: %s\n", s);
    s = p + 1;
} while (p != NULL);

free(dup);
}
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    char aux[2048];

    memset(&aux, 0, 2048);
    fgets(aux, sizeof(aux), stdin);
    printf("%s\n", aux);
}
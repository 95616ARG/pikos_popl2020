#include <stdio.h>
int a[10];
int main(int argc, char *argv[]) {
    size_t i = 0;
    for (;i < 10; i++) {
        a[i] = i;
    }
    a[i] = i;
    printf("%i", a[i]);
}

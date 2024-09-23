#include <stdio.h>

int main()
{
    int a;
    char b[10];
    int c = 0;
    sscanf("123 123 hihihi", "%d%d", &a, &c);
    printf("result: %d %d\n", a, c);
    return 0;
}
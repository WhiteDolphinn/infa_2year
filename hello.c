#include <stdio.h>

void f(char* s);
void b(char* s);

int main()
{
	printf("Hello world.");
	f(NULL);
}

void f(char* s)
{
	b(s);
}

void b(char *s)
{
	*s = 'x';
}

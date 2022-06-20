#include <stdio.h>

int main(const int argc, const char * argv[]) {
	extern char ** environ;
	printf("Arguments:\n");
	for (int i = 0; i < argc; ++i) printf("%s\n", argv[i]);
	printf("\nEnvironment:\n");
	for (size_t i = 0; environ[i] != NULL; ++i) printf("%s\n", environ[i]);
}

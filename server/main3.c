#include <stdio.h>

int main(int argc, char ** argv) {
	printf("Content-Type: text/plain\n\n");
	printf("Arguments:\n");
	for (size_t i = 0; argv[i] != NULL; ++i) {
		printf("%s\n", argv[i]);
	}
	extern char ** environ;
	for (size_t i = 0; environ[i] != NULL; ++i) {
		printf("%s\n", environ[i]);
	}
	printf("Done\n");
}

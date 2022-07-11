#include <stdio.h>
#include <unistd.h>

int main(int argc, char ** argv) {
	printf("Content-Type: text/plain\n\n");
	printf("Arguments:\n");
	for (size_t i = 0; argv[i] != NULL; ++i) {
		sleep(1);
		printf("%s\n", argv[i]);
	}
	sleep(10);
	extern char ** environ;
	for (size_t i = 0; environ[i] != NULL; ++i) {
		usleep(250);
		printf("%s\n", environ[i]);
	}
	sleep(10);
	printf("Done\n");
}

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
	printf("Status: 200 OK\r\n");
	printf("Transfer-Encoding: chunked\r\n");
	printf("Content-Type: text/plain\r\n");
	printf("\r\n");

	printf("4\r\n");
	printf("@see\r\n");
	sleep(1);
	printf("5\r\n");
	printf(" wiki\r\n");
	sleep(1);
	printf("3\r\n");
	printf("ped\r\n");
	sleep(1);
	printf("6\r\n");
	printf("ia.com\r\n");
	char * pwd = getcwd(NULL, 0);
	printf("%lu\r\n", strlen(pwd));
	printf("%s\r\n", pwd);
	free(pwd);
	printf("0\r\n\r\n");
}

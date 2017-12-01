#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXCHARS 80

void main(){
	char escritura[MAXCHARS];
	char * arg1;
	char * arg2;
	char * arg3;
	
	printf("Esperando a que respondas... >");
	fgets(escritura, MAXCHARS, stdin);
	printf("Debug\n\n");
	printf("Escritura = %s\n\n", escritura);
	
	arg1 = strtok(escritura, " ");
	arg2 = strtok(NULL, " ");
	arg3 = strtok(NULL, " ");
	
	printf("Arg1 = %s\n\n", arg1);
	printf("Arg2 = %s\n\n", arg2);
	printf("Arg3 = %s\n\n", arg3);
	
}
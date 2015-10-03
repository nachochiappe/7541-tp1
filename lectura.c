#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* chomp(char* str) { 
	if (*str && str[strlen(str)-1] == '\n') {
		str[strlen(str)-1] = '\0'; 
	}
	return str; 

} 

char* leer_linea() {
	size_t tam = 0;
	char *linea = NULL;

	if (getline(&linea, &tam, stdin) != -1) {
		return chomp(linea);
	} else {
		free(linea);
		return NULL;
	}

}
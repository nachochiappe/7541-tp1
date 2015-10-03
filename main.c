#include "lectura.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
	char* linea1 = leer_linea();
	char* linea2 = leer_linea();
	printf("Linea: %s\n", linea1);
	printf("Linea: %s\n", linea2);
	free(linea1);
	free(linea2);
}
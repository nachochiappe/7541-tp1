#include "pila.h"
#include <stdlib.h>

#define TAM_INICIAL 5
#define MULTIPLICADOR 2

/* Definición del struct pila proporcionado por la cátedra. */
struct pila {
    void **datos;
    size_t tam;
    size_t largo;
};

// Redimensiona una pila, ya sea agrandándola o achicándola.
// Pre: la pila está creada.
// Post: la pila fue redimensionada, o permanece igual en caso de falle realloc.
bool redimensionar(pila_t *pila, size_t nuevo_tam) {
	void **aux = realloc(pila->datos, nuevo_tam * sizeof(void*));
	if (!aux) return NULL;
	pila->datos = aux;
	pila->tam = nuevo_tam;
	return true;
}

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

pila_t* pila_crear() {
	pila_t* pila = malloc(sizeof(pila_t));
	if (!pila) return NULL;
	pila->datos = malloc(TAM_INICIAL * sizeof(void*));
	if (!pila->datos) {
		free(pila);
		return NULL;
	}
	pila->tam = TAM_INICIAL;
	pila->largo = 0;
    return pila;
}

void pila_destruir(pila_t *pila) {
	free(pila->datos);
	free(pila);
}

bool pila_esta_vacia(const pila_t *pila) {
	return (pila->largo == 0);
}

bool pila_apilar(pila_t *pila, void* valor) {
	if (pila->largo == pila->tam) {
		if (!redimensionar(pila, pila->tam * MULTIPLICADOR)) return false;
	}
	*(pila->datos + pila->largo) = valor;
	pila->largo += 1;
	return true;
}

void* pila_ver_tope(const pila_t *pila) {
	if (pila->largo == 0) return NULL;
	return pila->datos[pila->largo - 1];
}

void* pila_desapilar(pila_t *pila) {
	void* elemento = pila_ver_tope(pila);
	if (!pila_esta_vacia(pila)) {
		pila->largo -= 1;
		if ((pila->largo > 0) && (pila->largo < pila->tam / MULTIPLICADOR)) redimensionar(pila, pila->tam / MULTIPLICADOR);
	}
	return elemento;
}

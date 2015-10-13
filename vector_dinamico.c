#include "vector_dinamico.h"


// Funciones del alumno.

/* Devuelve el tamanio del vector segun el valor del miembro 'tam' de la 
 * estructura 'vector'.*/
size_t vector_obtener_tamanio(vector_t* vector){
    return vector->tam;
}

/* Devuelve la cantidad de elementos dentro del vector.*/
size_t vector_cantidad_elementos(vector_t* vector){
    return vector->elementos;
}

/* Libera la memoria del miembro "datos" de la estructura "vector" pasada
 * por parametro para luego liberar la de esta misma.*/
void vector_destruir(vector_t* vector, void destruir_dato(void*)) {
    if (destruir_dato != NULL){
        size_t i = 0;
        while (i != vector_obtener_tamanio(vector) && i < vector->elementos){
            destruir_dato(vector->datos[i]);
            i++;
        }
    }
    free(vector->datos);
    free(vector);
}

/* Compara el tamanio con la posicion pasada por parametro. Si esta es
 * mayor o igual, devuelve false, pero si es menor, asigna el valor del
 * vector en esa posicion a la variable 'valor'. Finalmente, devuelve true.*/
void* vector_obtener(vector_t* vector, size_t pos){
    if (pos >= vector_obtener_tamanio(vector))
        return NULL;
    return vector->datos[pos];
}

/* Compara el tamanio del vector con la posicion pasada por parametro. Si
 * esta es mayor o igual, devuelve false, mientras que si es menor, asigna el
 * valor pasado por parametro a la posicion 'pos' del vector. Devuelve true.*/
bool vector_guardar(vector_t* vector, size_t pos, void* valor){
    if (pos >= vector_obtener_tamanio(vector))
        return false;
    vector->datos[pos] = valor;
    if (vector->elementos <= pos)
        vector->elementos++;
    return true;
}



// Funciones implementadas por la catedra.

vector_t* vector_crear(size_t tam) {
    vector_t* vector = malloc(sizeof(vector_t));

    if (vector == NULL) {
	    return NULL;
    }
    vector->datos = malloc(tam * sizeof(void*));

    if (tam > 0 && vector->datos == NULL) {
        free(vector);
        return NULL;
    }
    vector->tam = tam;
    vector->elementos = 0;
    return vector;
}

bool vector_redimensionar(vector_t* vector, size_t tam_nuevo) {
    void** datos_nuevo = realloc(vector->datos, tam_nuevo * sizeof(void*));

    // Cuando tam_nuevo es 0, es correcto si se devuelve NULL.
    // En toda otra situación significa que falló el realloc.
    if (tam_nuevo > 0 && datos_nuevo == NULL) {
        return false;
    }

    vector->datos = datos_nuevo;
    vector->tam = tam_nuevo;
    return true;
}

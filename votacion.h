#include "parser.h"
#include "lectura.h"
#include "cola.h"
#include "pila.h"
#include "lista.h"
#include "vector_dinamico.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// DEFINIR PRIMITIVAS Y ESTRUCTURAS AUXILIARES

struct parametros;
typedef struct parametros parametros_t;

struct mesa;
typedef struct mesa mesa_t;

struct votante;
typedef struct votante votante_t;

struct partido;
typedef struct partido partido_t;


// PRIMITIVAS DE ESTRUCTURAS AUXILIARES

/*   MESA   */

mesa_t* mesa_crear(void);

bool mesa_esta_abierta(mesa_t* mesa);

void mesa_destruir(mesa_t *mesa, void destruir_dato(void*));

/* VOTANTE  */

votante_t* votante_crear(char* tipo_doc, char* nro_doc);

void votante_destruir(votante_t *votante, void destruir_dato(void*));

/* PARTIDO  */

partido_t* partido_crear(char id_partido, char* nombre_partido, char* presidente, char* gobernador, char* intendente);

void partido_destruir(partido_t *partido, void destruir_dato(void*));


// PRIMITIVAS PRINCIPALES

char abrir(mesa_t* mesa, parametros_t* parametros);

char ingresar(mesa_t* mesa, parametros_t* parametros);

// Funcion de votar. Verifica que los parametros pasados sean
// correctos, y segun si lo son o no, realiza un cierto
// proceso, devolviendo el numero de error (o cero) en donde corresponda.
char votar(mesa_t* mesa, parametros_t* parametros);

// Verifica que la mesa pueda cerrarse (si todos los votantes
// en espera ya votaron), e imprime el resultado de las elecciones.
char cerrar(mesa_t* mesa);

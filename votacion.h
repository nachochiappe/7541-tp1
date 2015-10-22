#include "parser.h"
#include "lectura.h"
#include "cola.h"
#include "pila.h"
#include "lista.h"
#include "vector_dinamico.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ARCHIVO_LISTA "lista_candidatos.csv"
#define ARCHIVO_PADRON "padron.csv"
#define MIN_CANT_PARTIDOS 5
#define CANT_COLUMNAS_CANDIDATOS 5
#define COLUMNA_ID 0
#define COLUMNA_NOMBRE 1
#define COLUMNA_PRESI 2
#define COLUMNA_GOBER 3
#define COLUMNA_INTEN 4
#define CANT_COLUMNAS_PADRON 2
#define COLUMNA_TIPO 0
#define COLUMNA_NRO 1
#define OK 0 // En caso de no haber errores
#define ERROR1 1 // Si hubo un error en la lectura de los archivos (o archivos inexistentes).
#define ERROR2 2 // Si la mesa ya estaba previamente abierta.
#define ERROR3 3 // Si la mesa no está abierta.
#define ERROR4 4 // Si el número de DNI es menor o igual a 0.
#define ERROR5 5 // Si la persona ya votó antes.
#define ERROR6 6 // Si el número y tipo de documento no está en el archivo padrón.csv.
#define ERROR7 7 // En caso de que no existan votantes en espera.
#define ERROR8 8 // En caso de que no existan operaciones para deshacer.
#define ERROR9 9 // En caso de que no se hayan elegido todas las categorías.
#define ERROR10 10 // En cualquier otro caso no contemplado.
#define ERROR11 11 // En caso de que aún queden votantes ingresados sin emitir su voto (al cerrar la mesa).
#define PRESIDENTE 1
#define GOBERNADOR 2
#define INTENDENTE 3


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

partido_t* partido_crear(char* id_partido, char* nombre_partido, char* presidente, char* gobernador, char* intendente);

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
char cerrar(mesa_t* mesa, parametros_t* parametros);

#include "votacion.h"

/* TIPOS DE ERRORES

ERROR1: si hubo un error en la lectura de los archivos (o archivos inexistentes).
ERROR2: si la mesa ya estaba previamente abierta -> (mesa_esta_abierta(mesa))
ERROR3: si la mesa no está abierta ->  (!mesa_esta_abierta(mesa))
ERROR4: si el número de DNI es menor o igual a 0.
ERROR5: si la persona ya votó antes.
ERROR6: si el número y tipo de documento no está en el archivo padrón.csv.
ERROR7: en caso de que no existan votantes en espera.
ERROR8: en caso de que no existan operaciones para deshacer.
ERROR9: en caso de que no se hayan elegido todas las categorías.
ERROR10: en cualquier otro caso no contemplado.
ERROR11: en caso de que aún queden votantes ingresados sin emitir su voto.

*/

struct mesa {
	vector_t boletas;
	lista_t votantes;
	cola_t votantes_en_espera;
	bool abierta;
};

struct votante {
	char* tipo_doc;
	int nro_doc;
	bool ya_voto;
	pila_t operaciones;
};

/*************************************
*          PRIMITIVAS MESA           *
**************************************/

mesa_t* mesa_crear(void) {
	mesa_t* mesa = malloc(sizeof(mesa_t));
	if (!mesa)
		return NULL;
	mesa->boletas = NULL;
	mesa->votantes = NULL;
	mesa->votantes_en_espera = NULL;
	mesa->operaciones = NULL;
	mesa->abierta = false;
	return mesa;
}

bool mesa_esta_abierta(mesa_t* mesa) {
	return mesa->abierta;
}

void mesa_destruir(mesa_t *mesa, void destruir_dato(void*)) {
	/*void* elemento;
	while (!cola_esta_vacia(cola)) {
		elemento = cola_desencolar(cola);
		if (destruir_dato)
			destruir_dato(elemento);
	}*/
	free(mesa);
}

/*************************************
*         PRIMITIVAS VOTANTE         *
**************************************/

votante_t* votante_crear(void) {
	votante_t* votante = malloc(sizeof(votante_t));
	if (!votante)
		return NULL;
	votante->tipo_doc = tipo_doc;
	votante->nro_doc = nro_doc;
	votante->ya_voto = false;
	votante->operaciones = NULL;
	return votante;
}

void votante_destruir(votante_t *votante, void destruir_dato(void*)) {
	/*void* elemento;
	while (!cola_esta_vacia(cola)) {
		elemento = cola_desencolar(cola);
		if (destruir_dato)
			destruir_dato(elemento);
	}*/
	free(votante);
}

/*************************************
*        FUNCIONES AUXILIARES        *
**************************************/

char* obtener_parametro(const char* linea, char nro_param) {
	const char delim[2] = " ";
	char* param;
	// Hago una copia de la línea para poder truncarla
	char *linea_copia = strcpy(malloc(strlen(linea) + 1), linea);
	// Obtengo el primer valor (comando)
	param = strtok(linea_copia, delim);
	if (nro_param == 0) {
		free(linea_copia);
		return param;
	}
	while (param) {
		param = strtok(NULL, delim);
		// Devuelvo el segundo valor (primer parámetro)
		if (nro_param == 1) {
			free(linea_copia);
			return param;
		}
	}
	// Devuelvo el tercer valor (segundo parámetro)
	free(linea_copia);
	return param;
}

/*************************************
*       PRIMITIVAS PRINCIPALES       *
**************************************/

char abrir(const char* linea, mesa_t* mesa) {
	char* archivo_lista = obtener_parametro(linea, 1);
	char* archivo_padron = obtener_parametro(linea, 2);
	if (!archivo_lista || !archivo_padron) return 1;
	if (/*no existe uno de los archivos*/) {
		return 1;
	} else if (mesa_esta_abierta(mesa)) return 2;
	// seguir con la apertura de la mesa
}

char ingresar(const char* linea, mesa_t* mesa) {
	
}

char votar(const char* linea, mesa_t* mesa) {
	
}

char cerrar(const char* linea, mesa_t* mesa) {
	
}

int main(void) {
	mesa_t* mesa = mesa_crear();
	char* linea = leer_linea();
	char comando = obtener_parametro(linea, 0);
	char resultado;
	switch(comando) {
		case 1:
			resultado = abrir(linea, mesa);
			break;
		case 2:
			resultado = ingresar(linea, mesa);
			break;
		case 3:
			resultado = votar(linea, mesa);
			break;
		case 4:
			resultado = cerrar(void);
			break;
		default:
			printf("Comando erróneo. Ingrese nuevamente.\n");
	}
	switch(resultado) {
		case 0:
			printf("OK\n");
		default:
			printf("ERROR%d\n", resultado);
	}
	free(linea);
	mesa_destruir(mesa);
	return 0;
}
#include "votacion.h"

#define ARCHIVO_LISTA "lista_candidatos.csv"
#define ARCHIVO_PADRON "padron.csv"
#define MIN_CANT_PARTIDOS 5

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

struct parametros {
	char* comando;
	char* param1;
	char* param2;
};

struct mesa {
	vector_t* boletas;
	lista_t* votantes;
	cola_t* votantes_en_espera;
	bool abierta;
};

struct votante {
	char* tipo_doc;
	char* nro_doc;
	//long int nro_doc;
	bool ya_voto;
	pila_t* operaciones;
};

struct partido {
	char id_partido;
	char* nombre_partido;
	char* presidente;
	char* gobernador;
	char* intendente;
};

struct voto {
	char id_partido_pres;
	char id_partido_gob;
	char id_partido_int;
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

votante_t* votante_crear(char* tipo_doc, char* nro_doc) {
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
*         PRIMITIVAS PARTIDO         *
**************************************/

partido_t* partido_crear(char id_partido, char* nombre_partido, char* presidente, char* gobernador, char* intendente) {
	partido_t* partido = malloc(sizeof(partido_t));
	if (!partido)
		return NULL;
	partido->id_partido = id_partido;
	partido->nombre_partido = nombre_partido;
	partido->presidente = presidente;
	partido->gobernador = gobernador;
	partido->intendente = intendente;
	return partido;
}

void partido_destruir(partido_t *partido, void destruir_dato(void*)) {
	/*void* elemento;
	while (!cola_esta_vacia(cola)) {
		elemento = cola_desencolar(cola);
		if (destruir_dato)
			destruir_dato(elemento);
	}*/
	free(partido);
}

/*************************************
*        FUNCIONES AUXILIARES        *
**************************************/

parametros_t* obtener_parametros(char* linea) {
	const char delim[2] = " ";
	char* param;
	parametros_t* parametros = malloc(sizeof(parametros_t));
	if (!parametros) return NULL;
	parametros->comando = NULL;
	parametros->param1 = NULL;
	parametros->param2 = NULL;
	param = strtok(linea, delim);
	parametros->comando = param;
	if (param) {
		param = strtok(NULL, delim);
		parametros->param1 = param;
	}
	param = strtok(NULL, delim);
	if (param) parametros->param2 = param;
	return parametros;
}

pila_t* deshacer_voto(mesa_t* mesa, pila_t* pila_votacion) {
	// FALTA TODO
	return pila_votacion;
}

char iniciar_votacion(mesa_t* mesa, votante_t* votante_cola) {
	char* linea = leer_linea(stdin);
	if (!linea || strcmp(linea, "") == 0) {
		free(linea);
		break;
	}
	parametros_t* parametros = obtener_parametros(linea);
	
	// Si el comando NO es 'votar' devuelvo ERROR10
	if (strcmp(parametros->comando, "votar") != 0) {
		free(linea);
		free(parametros);
		return 10;
	}
	
	// Creo la pila correspondiente al proceso de votación
	pila_t* pila_votacion = pila_crear();
	if (!pila_votacion) return 10;
	while (strcmp(parametros->param1, "fin") != 0) {
		if (strcmp(parametros->param1, "deshacer") == 0) {
			// Si no existen operaciones para deshacer imprimo ERROR8
			// No hago 'return' porque debería liberar la pila y la pierdo
			if (pila_esta_vacia(pila_votacion)) printf("ERROR8\n");
			else pila_votacion = deshacer_voto(mesa, pila_votacion);
		}
		char* linea = leer_linea(stdin);
		if (!linea || strcmp(linea, "") == 0) break;
		parametros_t* parametros = obtener_parametros(linea);
	}
	// FALTA FINALIZAR VOTACION CUANDO ESCRIBE 'FIN'
	free(linea);
	free(parametros);
}

/*************************************
*       PRIMITIVAS PRINCIPALES       *
**************************************/

char abrir(mesa_t* mesa, parametros_t* parametros) {
	if ((!parametros->param1) || (!parametros->param2)) return 1;
	char* archivo_lista_csv = parametros->param1;
	char* archivo_padron_csv = parametros->param2;
	if ((strcmp(archivo_lista_csv, ARCHIVO_LISTA) != 0) || (strcmp(archivo_padron_csv, ARCHIVO_PADRON) != 0)) return 1;
	// Confirmé que la lectura de parámetros fue correcta
	
	if (mesa_esta_abierta(mesa)) return 2;
	
	FILE *csv_lista = fopen(ARCHIVO_LISTA, "r");
	if (!csv_lista) return 1;
	FILE *csv_padron = fopen(ARCHIVO_PADRON, "r");
	if (!csv_padron) return 1;
	// Confirmé que ambos archivos existen
	
	// Proceso el archivo de lista de boletas
	char* linea_lista = leer_linea(csv_lista);
	// Creo un vector dinámico de tamaño MIN_CANT_PARTIDOS
	// Lo voy a redimensionar en caso de que haya más de 5 partidos
	vector_t* vector_boletas = vector_crear(MIN_CANT_PARTIDOS);
	size_t cant_partidos = 0;
	// Hago un primer parseo, lo descarto porque es la cabecera
	fila_csv_t* linea_lista_parseada = parsear_linea_csv(linea_lista, 5);
	while (linea_lista) {
		linea_lista_parseada = parsear_linea_csv(linea_lista, 5);
		char* id_partido = obtener_columna(linea_lista_parseada, 1);
		/*char* nombre_partido = obtener_columna(linea_lista_parseada, 2);
		char* presidente = obtener_columna(linea_lista_parseada, 3);
		char* gobernador = obtener_columna(linea_lista_parseada, 4);
		char* intendente = obtener_columna(linea_lista_parseada, 5);
		partido_t* partido = partido_crear(id_partido, nombre_partido, presidente, gobernador, intendente);*/
		if (cant_partidos == vector_obtener_tamanio(vector_boletas)) vector_redimensionar(vector_boletas, cant_partidos + MIN_CANT_PARTIDOS);
		vector_guardar(vector_boletas, cant_partidos, *id_partido);
		cant_partidos++;
		linea_lista = leer_linea(csv_lista);
	}
	// Cierro el archivo de lista porque ya terminé de trabajar con él
	fclose(csv_lista);
	
	// Proceso el archivo de padrones
	char* linea_padron = leer_linea(csv_padron);
	lista_t* lista_padrones = lista_crear();
	// Hago un primer parseo, lo descarto porque es la cabecera
	fila_csv_t* linea_padron_parseada = parsear_linea_csv(linea_padron, 2);
	while (linea_padron) {
		linea_padron_parseada = parsear_linea_csv(linea_padron, 2);
		char* tipo_doc = obtener_columna(linea_padron_parseada, 0);
		char* nro_doc = obtener_columna(linea_padron_parseada, 1);
		votante_t* votante = votante_crear(tipo_doc, nro_doc);
		lista_insertar_primero(lista_padrones, votante);
		linea_padron = leer_linea(csv_padron);
	}
	// Cierro el archivo de padrones porque ya terminé de trabajar con él
	fclose(csv_padron);
	mesa->votantes = lista_padrones;
	cola_t* cola_votantes = cola_crear();
	mesa->votantes_en_espera = cola_votantes;
	mesa->abierta = true;
	return 0;
}

char ingresar(mesa_t* mesa, parametros_t* parametros) {
	if (!mesa_esta_abierta(mesa)) return 3;
	if ((!parametros->param1) || (!parametros->param2)) return 4;
	char* tipo_doc = parametros->param1;
	char* nro_doc = parametros->param2;
	if (atoi(nro_doc) <= 0) return 4;
	votante_t* votante = votante_crear(tipo_doc, nro_doc);
	cola_encolar(mesa->votantes_en_espera, votante);
	return 0;
}

char votar(mesa_t* mesa, parametros_t* parametros) {
	if (!mesa_esta_abierta(mesa)) return 3;
	// Si el comando es 'inicio', inicia un ciclo de cargos para votar.
	if (strcmp(parametros->param1, "inicio") == 0) {
		votante_t* votante_cola = cola_desencolar(mesa->votantes_en_espera);
		// Si no hay votantes en espera, devuelvo ERROR7
		if (!votante_cola) return 7;
		// Verifico si el votante está en el padrón
		lista_iter_t* votantes_iter = lista_iter_crear(mesa->votantes);
		while (!lista_iter_al_final(votantes_iter)) {
			votante_t* votante_iter = lista_iter_ver_actual(votantes_iter);
			// Si está en el padrón, inicio el proceso de votación
			if ((strcmp(votante_iter->tipo_doc, votante_cola->tipo_doc) == 0) && (strcmp(votante_iter->nro_doc, votante_cola->nro_doc) == 0)) {
				char resultado = iniciar_votacion(mesa, votante_cola);
				free(votante_cola);
				lista_iter_destruir(votantes_iter);
				return resultado;
			}
			lista_iter_avanzar(votantes_iter);
		}
		lista_iter_destruir(votantes_iter);
		// Si el número y tipo de documento no está en el archivo padron.csv devuelvo ERROR6
		return 6;
	}
	// En cualquier otro caso no contemplado devuelvo ERROR10
	return 10;
}

char cerrar(mesa_t* mesa) {
	return 0;
}

int main(void) {
	mesa_t* mesa = mesa_crear();
	char resultado;
	bool fin = false;
	do {
		char* linea = leer_linea(stdin);
		if (!linea || strcmp(linea, "") == 0) break;
		parametros_t* parametros = obtener_parametros(linea);
		if (strcmp(parametros->comando, "abrir") == 0) resultado = abrir(mesa, parametros);
			else if (strcmp(parametros->comando, "ingresar") == 0) resultado = ingresar(mesa, parametros);
				else if (strcmp(parametros->comando, "votar") == 0) resultado = votar(mesa, parametros);
					else if (strcmp(parametros->comando, "cerrar") == 0) resultado = cerrar(mesa);
						else {
							parametros->comando = NULL;
							printf("Comando erróneo. Ingrese nuevamente.\n");
						}
		if (parametros->comando) {
			if (resultado == 0) printf("OK\n");
			else printf("ERROR%d\n", resultado);
			if ((strcmp(parametros->comando, "cerrar") == 0) && (resultado == 0)) fin = true;
		}
		free(linea);
		free(parametros);
	// Repite el ciclo hasta que el comando que se haya podido cerrar la mesa correctamente
	} while (!fin);
	mesa_destruir(mesa, free);
	return 0;
}
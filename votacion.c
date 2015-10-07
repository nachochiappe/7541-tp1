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

struct mesa {
	vector_t* boletas;
	lista_t* votantes;
	cola_t* votantes_en_espera;
	bool abierta;
};

struct votante {
	char* tipo_doc;
	char* nro_doc;
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
	char id_partido;
	char* nombre_partido;
	char* nombre_candidato;
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

char* obtener_parametro(const char* linea, char nro_param) {
	const char delim[2] = " ";
	char* param;
	// Hago una copia de la línea para poder truncarla
	char* linea_copia = strdup(linea);
	// Obtengo el primer valor (comando)
	param = strtok(linea_copia, delim);
	if (nro_param == 0) {
		free(linea_copia);
		printf("%s\n", param);
		return param;
	}
	while (param) {
		param = strtok(NULL, delim);
		// Devuelvo el segundo valor (primer parámetro)
		if (nro_param == 1) {
			free(linea_copia);
			printf("%s\n", param);
			return param;
		}
	}
	// Devuelvo el tercer valor (segundo parámetro)
	free(linea_copia);
	printf("%s\n", param);
	return param;
}

/*************************************
*       PRIMITIVAS PRINCIPALES       *
**************************************/

char abrir(const char* linea, mesa_t* mesa) {
	if (mesa_esta_abierta(mesa)) return 2;
	
	char* archivo_lista_csv = obtener_parametro(linea, 1);
	char* archivo_padron_csv = obtener_parametro(linea, 2);
	if ((strcmp(archivo_lista_csv, ARCHIVO_LISTA) != 0) || (strcmp(archivo_padron_csv, ARCHIVO_PADRON) != 0)) return 2;
	// Confirmé que la lectura de parámetros fue correcta
	
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
	while (linea_lista) {
		fila_csv_t* linea_lista_parseada = parsear_linea_csv(linea_lista, 5);
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
	while (linea_padron) {
		fila_csv_t* linea_padron_parseada = parsear_linea_csv(linea_padron, 2);
		char* tipo_doc = obtener_columna(linea_padron_parseada, 1);
		char* nro_doc = obtener_columna(linea_padron_parseada, 2);
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

char ingresar(const char* linea, mesa_t* mesa) {
	return 0;
}

char votar(const char* linea, mesa_t* mesa) {
	return 0;
}

char cerrar(mesa_t* mesa) {
	return 0;
}

int main(void) {
	mesa_t* mesa = mesa_crear();
	char resultado;
	char* comando;
	do {
		char* linea = leer_linea(stdin);
		comando = obtener_parametro(linea, 0);
		switch(*comando) {
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
				resultado = cerrar(mesa);
				break;
			default:
				printf("Comando erróneo. Ingrese nuevamente.\n");
		}
		if (resultado == 0) {
			printf("OK\n");
		} else printf("ERROR%d\n", resultado);
		free(linea);
	// Repite el ciclo hasta que el comando que se haya podido cerrar la mesa correctamente
	} while (*comando != 4 && resultado != 0);
	mesa_destruir(mesa, free);
	return 0;
}
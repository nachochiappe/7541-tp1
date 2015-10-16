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
	int en_cola;
	bool ya_voto;
	pila_t* operaciones;
};

struct partido {
	char* id_partido;
	char* nombre_partido;
	char* presidente;
	char* gobernador;
	char* intendente;
	int votos_presi;
	int votos_gober;
	int votos_inten;
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

void destruir_partido(void* dato){
	partido_destruir((partido_t*) dato, NULL);
}

void destruir_votante(void* dato){
	votante_destruir((votante_t*) dato, NULL);
}


void mesa_destruir(mesa_t *mesa, void destruir_dato(void*)) {
	vector_destruir(mesa->boletas, destruir_partido);
	lista_destruir(mesa->votantes, destruir_votante);
	cola_destruir(mesa->votantes_en_espera, destruir_votante);
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
	votante->en_cola = 0;
	votante->operaciones = pila_crear();
	return votante;
}

void votante_destruir(votante_t *votante, void destruir_dato(void*)) {
	pila_destruir(votante->operaciones);
	free(votante->tipo_doc);
	free(votante->nro_doc);
	free(votante);
}

/*************************************
*         PRIMITIVAS PARTIDO         *
**************************************/

partido_t* partido_crear(char* id_partido, char* nombre_partido, char* presidente, char* gobernador, char* intendente) {
	partido_t* partido = malloc(sizeof(partido_t));
	if (!partido)
		return NULL;
	partido->id_partido = id_partido;
	partido->nombre_partido = nombre_partido;
	partido->presidente = presidente;
	partido->gobernador = gobernador;
	partido->intendente = intendente;
	partido->votos_presi = 0;
	partido->votos_gober = 0;
	partido->votos_inten = 0;
	return partido;
}

void partido_destruir(partido_t *partido, void destruir_dato(void*)){
	free(partido->id_partido);
	free(partido->nombre_partido);
	free(partido->presidente);
	free(partido->gobernador);
	free(partido->intendente);
	free(partido);
}

/*************************************
*        FUNCIONES AUXILIARES        *
**************************************/

parametros_t* obtener_parametros(char* linea) {
	const char delim[2] = " ";
	parametros_t* parametros = malloc(sizeof(parametros_t));
	if (!parametros) return NULL;
	parametros->comando = NULL;
	parametros->param1 = NULL;
	parametros->param2 = NULL;
	char* param = strtok(linea, delim);
	parametros->comando = param;
	if (param) {
		param = strtok(NULL, delim);
		parametros->param1 = param;
	}
	param = strtok(NULL, delim);
	if (param) parametros->param2 = param;
	return parametros;
}

// Imprime, con un formato especifico, el nombre y candidato de todos los 
// partidos en el archivo ARCHIVO_LISTA, segun los valores pasados por parametro.
void imprimir_candidatos(mesa_t* mesa, size_t cantidad_partidos, int cargo){
	for (size_t i = 0; i < cantidad_partidos; i++){
		partido_t* partido = vector_obtener(mesa->boletas, i);
		char* nombre_partido = partido->nombre_partido;
		if (cargo == 1)
			printf ("%zu: %s: %s\n", i+1, nombre_partido, partido->presidente);
		else if (cargo == 2)
			printf ("%zu: %s: %s\n", i+1, nombre_partido, partido->gobernador);
		else
			printf ("%zu: %s: %s\n", i+1, nombre_partido, partido->intendente);
	}
}

// Imprime todos los candidatos de un cierto cargo, segun el numero
// 'num_operacion' pasado por parametro.
void votar_candidato(mesa_t* mesa, size_t num_operacion){
	size_t cantidad_partidos = vector_cantidad_elementos(mesa->boletas);
	if (num_operacion == 1){
		printf ("Cargo: Presidente\n");
		imprimir_candidatos(mesa, cantidad_partidos, 1);
	}
	else if (num_operacion == 2){
		printf ("Cargo: Gobernador\n");
		imprimir_candidatos(mesa, cantidad_partidos, 2);
	}
	else{
		printf ("Cargo: Intendente\n");
		imprimir_candidatos(mesa, cantidad_partidos, 3);
	}
}

// Imprime el resultado de la votacion, para cada partido
// y sus respectivos candidatos.
void imprimir_resultado(mesa_t* mesa){
	size_t cantidad_partidos = vector_cantidad_elementos(mesa->boletas);
	for (size_t i = 0; i < cantidad_partidos; i++){
		partido_t* partido = vector_obtener(mesa->boletas, i);
		char* nombre_partido = partido->nombre_partido;
		printf ("%s:\nPresidente: %i votos\nGobernador: %ivotos\nIntendente: %ivotos\n", 
				nombre_partido, partido->votos_presi, partido->votos_gober, partido->votos_inten);
	}
}

/* Devuelve true o false segun si el votante pasado por
 * parametro esta o no en el archivo ARCHIVO_PADRON.*/ 
bool verificar_votante_en_padron(votante_t* votante){
	bool en_padron = false;
	FILE *csv_padron = fopen(ARCHIVO_PADRON, "r");
	char* linea_padron = leer_linea(csv_padron);
	free(linea_padron);
	// Hago una nueva lectura, ya que descarto la primera porque es la cabecera
	linea_padron = leer_linea(csv_padron);
	fila_csv_t* linea_padron_parseada;
	while (linea_padron) {
		linea_padron_parseada = parsear_linea_csv(linea_padron, 2);
		char* tipo_doc = obtener_columna(linea_padron_parseada, 0);
		char* nro_doc = obtener_columna(linea_padron_parseada, 1);
		if (strcmp(tipo_doc, votante->tipo_doc) == 0){
			if (strcmp(nro_doc, votante->nro_doc) == 0){
				en_padron = true;
				destruir_fila_csv(linea_padron_parseada, true);
				break;
			}
		}
		free(linea_padron);
		linea_padron = leer_linea(csv_padron);
		destruir_fila_csv(linea_padron_parseada, true);
	}
	free(linea_padron);
	// Cierro el archivo de padrones porque ya terminé de trabajar con él
	fclose(csv_padron);
	return en_padron;
}

/*************************************
*       PRIMITIVAS PRINCIPALES       *
**************************************/

char abrir(mesa_t* mesa, parametros_t* parametros) {
	// Confirmo que la lectura de parámetros fue correcta
	if ((!parametros->param1) || (!parametros->param2)) return 1;
	if (mesa_esta_abierta(mesa)) return 2;
	char* archivo_lista_csv = parametros->param1;
	char* archivo_padron_csv = parametros->param2;
	if ((strcmp(archivo_lista_csv, ARCHIVO_LISTA) != 0) || (strcmp(archivo_padron_csv, ARCHIVO_PADRON) != 0)) return 1;
	
	// Confirmo que ambos archivos existen
	FILE *csv_lista = fopen(ARCHIVO_LISTA, "r");
	if (!csv_lista) return 1;
	FILE *csv_padron = fopen(ARCHIVO_PADRON, "r");
	if (!csv_padron) return 1;
	
	// Proceso el archivo de lista de boletas
	char* linea_lista = leer_linea(csv_lista);
	free(linea_lista);
	// Creo un vector dinámico de tamaño MIN_CANT_PARTIDOS
	// Lo voy a redimensionar en caso de que haya más de 5 partidos
	vector_t* vector_boletas = vector_crear(MIN_CANT_PARTIDOS);
	size_t cant_partidos = 0;
	// Hago una nueva lectura, ya que descarto la primera porque es la cabecera
	linea_lista = leer_linea(csv_lista);
	fila_csv_t* linea_lista_parseada;
	while (linea_lista) {
		linea_lista_parseada = parsear_linea_csv(linea_lista, 5);
		char* id_partido = obtener_columna(linea_lista_parseada, 0);
		char* nombre_partido = obtener_columna(linea_lista_parseada, 1);
		char* presidente = obtener_columna(linea_lista_parseada, 2);
		char* gobernador = obtener_columna(linea_lista_parseada, 3);
		char* intendente = obtener_columna(linea_lista_parseada, 4);
		partido_t* partido = partido_crear(id_partido, nombre_partido, presidente, gobernador, intendente);
		if (cant_partidos == vector_obtener_tamanio(vector_boletas)){ 
			if (!vector_redimensionar(vector_boletas, cant_partidos + MIN_CANT_PARTIDOS)) {
				fclose(csv_lista);
				fclose(csv_padron);
				free(linea_lista);
				destruir_fila_csv(linea_lista_parseada, false);
				return 10; // Fallo la redimension
			}
		}
		vector_guardar(vector_boletas, cant_partidos, partido); // Guarda en un vector dinamico de punteros genericos, un puntero al partido.
		cant_partidos++;
		destruir_fila_csv(linea_lista_parseada, false);
		free(linea_lista);
		linea_lista = leer_linea(csv_lista);
	}
	// Cierro el archivo de lista porque ya terminé de trabajar con él
	fclose(csv_lista);
	
	// Proceso el archivo de padrones
	char* linea_padron = leer_linea(csv_padron);
	free(linea_padron);
	lista_t* lista_padrones = lista_crear();
	// Hago una nueva lectura, ya que descarto la primera porque es la cabecera
	linea_padron = leer_linea(csv_padron);
	fila_csv_t* linea_padron_parseada;
	while (linea_padron) {
		linea_padron_parseada = parsear_linea_csv(linea_padron, 2);
		char* tipo_doc = obtener_columna(linea_padron_parseada, 0);
		char* nro_doc = obtener_columna(linea_padron_parseada, 1);
		votante_t* votante = votante_crear(tipo_doc, nro_doc);
		lista_insertar_primero(lista_padrones, votante);
		free(linea_padron);
		linea_padron = leer_linea(csv_padron);
		destruir_fila_csv(linea_padron_parseada, false);
	}
	// Cierro el archivo de padrones porque ya terminé de trabajar con él
	fclose(csv_padron);
	mesa->boletas = vector_boletas;
	mesa->votantes = lista_padrones;
	cola_t* cola_votantes = cola_crear();
	mesa->votantes_en_espera = cola_votantes;
	mesa->abierta = true;
	return 0;
}

char ingresar(mesa_t* mesa, parametros_t* parametros) {
	if (!mesa_esta_abierta(mesa)) return 3;
	if ((!parametros->param1) || (!parametros->param2)) return 4;
	if (atoi(parametros->param2) <= 0) return 4;
	char* tipo_doc = strcpy(malloc(strlen(parametros->param1) + 1), parametros->param1); // Si no se copian, le pasa la direccion de memoria de los parametros
	char* nro_doc = strcpy(malloc(strlen(parametros->param2) + 1), parametros->param2); // a votante_crear, y terminaba guardando los parametros en el tipo de documento.
	votante_t* votante_en_espera = votante_crear(tipo_doc, nro_doc);
	lista_iter_t* votantes_iter = lista_iter_crear(mesa->votantes);
	while (!lista_iter_al_final(votantes_iter)) {
		votante_t* votante = lista_iter_ver_actual(votantes_iter);
		if ((strcmp(votante->tipo_doc, tipo_doc) == 0) && (strcmp(votante->nro_doc, nro_doc) == 0)) {
			if (votante->en_cola > 0){ // Si el votante ya estuvo en la cola, seguramente ya haya votado,
				votante_en_espera->ya_voto = true; // dado a que se lo desencola una vez que ya voto.
				break;
			}
			votante->en_cola++;
			break;
		}
		lista_iter_avanzar(votantes_iter);
	}
	lista_iter_destruir(votantes_iter);
	cola_encolar(mesa->votantes_en_espera, votante_en_espera);
	return 0;
}


// Funcion de votar. Verifica que los parametros pasados sean
// correctos, y segun si lo son o no, realiza un cierto
// proceso, devolviendo el numero de error (o cero) en donde corresponda.
char votar(mesa_t* mesa, parametros_t* parametros) {
	if (!mesa_esta_abierta(mesa)) return 3;
	if (parametros->param2 != NULL || !parametros->param1) return 10; // 'votar' lleva solamente un parametro (no puede ser mas de uno, tampoco ninguno).
	if (strcmp(parametros->param1, "inicio") == 0){
		if (cola_esta_vacia(mesa->votantes_en_espera)) return 7;
		votante_t* votante = cola_ver_primero(mesa->votantes_en_espera);
		if (votante->ya_voto) return 5;
		bool en_padron = verificar_votante_en_padron(votante);
		if (en_padron){
			char* operacion = "presidente";
			pila_apilar(votante->operaciones, operacion); // "La persona esta votando al presidente".
			votar_candidato(mesa, 1);
			return -1;
		}
		return 6;
	}
	else if (strcmp(parametros->param1, "deshacer") == 0){
		if (cola_esta_vacia(mesa->votantes_en_espera)) return 8;
		votante_t* votante = cola_ver_primero(mesa->votantes_en_espera);
		char* operacion = pila_ver_tope(votante->operaciones);
		if (!operacion) return 10;
		if (strcmp(operacion, "presidente") == 0) return 8;
		size_t id_partido = 0;
		while (id_partido != 1){ // Habia un problema aca por apilar enteros (size_t) en vez de una cadena.
			if (!pila_ver_tope(votante->operaciones)) return 8;
			id_partido = strlen((char*) pila_desapilar(votante->operaciones));
		}
		partido_t* partido = vector_obtener(mesa->boletas, id_partido - 1);
		if (strcmp((char*) pila_ver_tope(votante->operaciones), "presidente") == 0){
			partido->votos_presi--;
			printf("OK\n");
			votar_candidato(mesa, 1);
		}
		else if (strcmp((char*) pila_ver_tope(votante->operaciones), "gobernador") == 0){
			partido->votos_gober--;
			printf("OK\n");
			votar_candidato(mesa, 2);
		}
		else{
			votante->ya_voto = false;
			partido->votos_inten--;
			printf("OK\n");
			votar_candidato(mesa, 3);
		}
		return -1;
	}
	else if (strcmp(parametros->param1, "fin") == 0){
		votante_t* votante = cola_ver_primero(mesa->votantes_en_espera);
		if (!votante) return 9;
		char* operacion = pila_ver_tope(votante->operaciones);
		if (!operacion) return 10;
		if (!votante->ya_voto) return 9;
		votante = cola_desencolar(mesa->votantes_en_espera);
		votante_destruir(votante, NULL);
		return 0;
	}
	else if ((0 < atoi(parametros->param1)) <= vector_cantidad_elementos(mesa->boletas)){
		votante_t* votante = cola_ver_primero(mesa->votantes_en_espera);
		size_t id_partido = (size_t) atoi(parametros->param1);
		char partido_votado[300]; // Suficiente para guardar el entero
		sprintf (partido_votado, "%i", (int) id_partido); // Asi, apilamos cadenas en vez de enteros.
		char* operacion = pila_ver_tope(votante->operaciones);
		if (!operacion) return 10;
		partido_t* partido = vector_obtener(mesa->boletas, id_partido - 1);
		if (strcmp(operacion, "presidente") == 0) {
			pila_apilar(votante->operaciones, partido_votado);
			operacion = "gobernador";
			pila_apilar(votante->operaciones, operacion);
			partido->votos_presi++;
			printf("OK\n");
			votar_candidato(mesa, 2);
			return -1; // Devuelvo -1 para que no me imprima OK
		}
		else if (strcmp(operacion, "gobernador") == 0){
			pila_apilar(votante->operaciones, partido_votado);
			operacion = "intendente";
			pila_apilar(votante->operaciones, operacion);
			partido->votos_gober++;
			printf("OK\n");
			votar_candidato(mesa, 3);
			return -1; // Devuelvo -1 para que no me imprima OK
		}
		else if (strcmp(operacion, "intendente") == 0){
			pila_apilar(votante->operaciones, partido_votado);
			partido->votos_inten++;
			votante->ya_voto = true;
			printf("OK\n");
			return -1;
		}
	}
	return 10;
}


// Verifica que la mesa pueda cerrarse (si todos los votantes
// en espera ya votaron), e imprime el resultado de las elecciones.
char cerrar(mesa_t* mesa, parametros_t* parametros) {
	if (!mesa->abierta) return 3;
	if (parametros->param2 != NULL || parametros->param1 != NULL) return 10; // 'cerrar' no lleva parametros.
	if (!cola_esta_vacia(mesa->votantes_en_espera)) return 11;
	imprimir_resultado(mesa);
	mesa->abierta = false;
	return -1; // Devuelvo -1 para que no me imprima OK
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
					else if (strcmp(parametros->comando, "cerrar") == 0) resultado = cerrar(mesa, parametros);
						else {
							parametros->comando = NULL;
							printf("Comando erróneo. Ingrese nuevamente.\n");
						}
		if (parametros->comando) {
			if (resultado == 0) printf("OK\n");
			else if (resultado != -1) printf("ERROR%d\n", resultado);
			if ((strcmp(parametros->comando, "cerrar") == 0) && (resultado == 0)) fin = true;
		}
		free(linea);
		free(parametros);
	// Repite el ciclo hasta que el comando que se haya podido cerrar la mesa correctamente
	} while (!fin);
	mesa_destruir(mesa, NULL);
	return 0;
}

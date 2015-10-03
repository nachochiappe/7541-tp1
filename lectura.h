#ifndef LECTURA_H
#define LECTURA_H

/*
 * Lee una linea de la entrada estandar, hasta encontrar un caracter de fin
 * de linea o el fin de la entrada estandar.
 * Devuelve un char* con el contenido leido en memoria dinamica. Debe liberarse
 * con free() cuando no se necesite mas.
 */
char* leer_linea() ;

#endif // LECTURA_H

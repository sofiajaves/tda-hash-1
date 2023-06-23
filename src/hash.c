#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

#define FACTOR_CARGA_MAXIMO 0.7
#define CAPACIDAD_MINIMA 3

typedef struct nodo {
	char *clave;
	void *elemento;
	struct nodo *siguiente;
} nodo_t;

struct hash {
	nodo_t **vector;
	size_t capacidad;
	size_t cantidad;
};

hash_t *hash_crear(size_t capacidad)
{
	hash_t *hash = calloc(1, sizeof(hash_t));
	if (!hash)
		return NULL;

	if (capacidad < CAPACIDAD_MINIMA)
		hash->capacidad = CAPACIDAD_MINIMA;
	hash->capacidad = capacidad;
	hash->vector = calloc(1, sizeof(nodo_t *) * capacidad);
	if (!hash->vector) {
		free(hash);
		return NULL;
	}
	return hash;
}

unsigned long funcion_hash(const char *str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = hash + (unsigned long)c;
	return hash;
}

/*nodo_t *nodo_crear(const char *clave, void *elemento)
{
	nodo_t *nodo = malloc(sizeof(nodo_t));
	if (!nodo)
		return NULL;
	char *copia = malloc(strlen(clave) + 1);
	if (!copia)
		return NULL;

	nodo->clave = strcpy(copia, clave);
	nodo->elemento = elemento;
	nodo->siguiente = NULL;
	return nodo;
}*/

/*hash_t *rehash(hash_t *hash)
{
	size_t nueva_capacidad = hash->capacidad * 2;
	hash_t *nuevo_hash = hash_crear(nueva_capacidad);
	if (!nuevo_hash)
		return NULL;

	for (int posicion = 0; posicion < hash->capacidad; posicion++) {
		nodo_t *actual = hash->vector[posicion];
		while (actual != NULL) {
			hash_insertar(nuevo_hash, actual->clave,
				      actual->elemento, NULL);
			actual = actual->siguiente;
		}
	}
	hash_t aux = *hash;
	*hash = *nuevo_hash;
	*nuevo_hash = aux;
	hash_destruir(nuevo_hash);
	return hash;
}*/

char *strdup(const char *clave)
{
	char *duplicado = malloc((strlen(clave) + 1) * sizeof(char));
	if (!duplicado)
		return NULL;

	strcpy(duplicado, clave);

	return duplicado;
}

/* 
 *	Se devuelve un struct de par clave/valor con su respectiva reserva de memoria y campos actualizados
 *	Devuelve NULL en caso de error
 */
nodo_t *crear_par(const char *clave, void *elemento)
{
	if (!clave)
		return NULL;

	nodo_t *nuevo_par = malloc(sizeof(nodo_t));
	if (!nuevo_par)
		return NULL;

	nuevo_par->clave = strdup(clave);
	if (!nuevo_par->clave)
		return NULL;
	nuevo_par->elemento = elemento;
	nuevo_par->siguiente = NULL;

	return nuevo_par;
}

hash_t *rehash(hash_t *hash)
{
	size_t nueva_capacidad = hash->capacidad * 2;
	nodo_t **nuevo_vector = calloc(nueva_capacidad, sizeof(nodo_t *));
	if (!nuevo_vector)
		return NULL;

	for (int posicion = 0; posicion < hash->capacidad; posicion++) {
		nodo_t *actual = hash->vector[posicion];
		while (actual != NULL) {
			nodo_t *siguiente = actual->siguiente;

			size_t nueva_posicion =
				funcion_hash(actual->clave) % nueva_capacidad;

			actual->siguiente = nuevo_vector[nueva_posicion];
			nuevo_vector[nueva_posicion] = actual;

			actual = siguiente;
		}
	}
	free(hash->vector);
	hash->vector = nuevo_vector;
	hash->capacidad = nueva_capacidad;
	return hash;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;

	unsigned long posicion = funcion_hash(clave) % hash->capacidad;

	float factor_carga = (float)(hash->cantidad) / (float)(hash->capacidad);
	if (factor_carga > FACTOR_CARGA_MAXIMO)
		hash = rehash(hash);

	if (!hash)
		return NULL;

	if (hash->vector[posicion] == NULL) {
		nodo_t *nodo = crear_par(clave, elemento);
		if (!nodo)
			return NULL;

		hash->vector[posicion] = nodo;
		hash->cantidad++;
		if (anterior != NULL)
			*anterior = NULL;

		return hash;
	}

	nodo_t *actual = hash->vector[posicion];
	while (actual != NULL) {
		if (strcmp(actual->clave, clave) == 0) {
			if (anterior != NULL)
				*anterior = actual->elemento;
			actual->elemento = elemento;
			return hash;
		}
		actual = actual->siguiente;
	}

	nodo_t *nodo = crear_par(clave, elemento);
	if (!nodo)
		return NULL;

	nodo->siguiente = hash->vector[posicion];
	hash->vector[posicion] = nodo;
	hash->cantidad++;
	if (anterior != NULL)
		*anterior = NULL;
	return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = funcion_hash(clave) % hash->capacidad;

	nodo_t *actual = hash->vector[posicion];
	nodo_t *anterior = NULL;
	while (actual && (strcmp(actual->clave, clave) != 0)) {
		anterior = actual;
		actual = actual->siguiente;
	}

	if (actual != NULL) {
		void *elemento = actual->elemento;
		if (anterior == NULL) {
			hash->vector[posicion] = actual->siguiente;
		} else {
			anterior->siguiente = actual->siguiente;
		}
		free(actual->clave);
		free(actual);
		hash->cantidad--;
		return elemento;
	}
	return NULL;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = funcion_hash(clave) % hash->capacidad;
	if (!hash->vector[posicion])
		return NULL;
	nodo_t *actual = hash->vector[posicion];
	while (actual) {
		if (strcmp(actual->clave, clave) == 0)
			return actual->elemento;
		actual = actual->siguiente;
	}
	return NULL;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return false;

	if (!hash_obtener(hash, clave))
		return false;
	return true;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;

	return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	for (int posicion = 0; posicion < hash->capacidad; posicion++) {
		nodo_t *actual = hash->vector[posicion];
		while (actual) {
			nodo_t *nodo_a_liberar = actual;
			actual = actual->siguiente;
			if (destructor)
				destructor(nodo_a_liberar->elemento);

			free(nodo_a_liberar->clave);
			free(nodo_a_liberar);
		}
	}
	free(hash->vector);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *elemento,
				     void *aux),
			   void *aux)
{
	if (!hash || !f)
		return 0;

	size_t cantidad_recorrido = 0;
	for (int posicion = 0; posicion < hash->capacidad; posicion++) {
		nodo_t *actual = hash->vector[posicion];
		while (actual) {
			cantidad_recorrido++;
			if (!f(actual->clave, actual->elemento, aux))
				return cantidad_recorrido;
			actual = actual->siguiente;
		}
	}
	return cantidad_recorrido;
}

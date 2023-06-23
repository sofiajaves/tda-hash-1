#include "src/hash.h"
#include "pa2m.h"

void probar_creacion()
{
	pa2m_nuevo_grupo("PRUEBAS DE CREACION");

	hash_t *hash = hash_crear(1);
	pa2m_afirmar(hash != NULL,
		     "Se puede crear un hash solicitando capacidad menor a 3");
	hash_destruir(hash);

	hash = hash_crear(8);
	pa2m_afirmar(hash != NULL,
		     "se puede crear un hash solicitando capacidad mayor a 3");

	pa2m_afirmar(hash_cantidad(hash) == 0,
		     "un hash recien creado tiene 0 elementos");

	hash_destruir(hash);
}

void probar_insertar()
{
	pa2m_nuevo_grupo("PRUEBAS DE INSERTAR");
	hash_t *hash = hash_crear(1);
	pa2m_afirmar(hash_insertar(hash, "1", "UNO", NULL) != NULL,
		     "Se inserta un elemento ");
	pa2m_afirmar(hash_insertar(hash, "2", "DOS", NULL) != NULL,
		     "Se inserta un elemento");
	pa2m_afirmar(hash_insertar(hash, "3", "TRES", NULL) != NULL,
		     "Se inserta un elemento");
	pa2m_afirmar(hash_cantidad(hash) == 3, "El hash contiene 3 elementos ");

	pa2m_afirmar(hash_insertar(hash, "4", "CUATRO", NULL) != NULL,
		     "Se inserta un elemento ");
	pa2m_afirmar(hash_cantidad(hash) == 4, "El hash contiene 4 elementos ");

	pa2m_afirmar(hash_insertar(hash, "4", "CUATRO1", NULL) != NULL,
		     "Se inserta un elemento ");
	pa2m_afirmar(hash_cantidad(hash) == 4, "El hash contiene 4 elementos ");

	pa2m_afirmar(hash_contiene(hash, "4") == true,
		     "El hash contiene el elemento con clave 4");

	hash_destruir(hash);
}

int main()
{
	probar_creacion();
	probar_insertar();

	return pa2m_mostrar_reporte();
}

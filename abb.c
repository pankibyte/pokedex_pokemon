#include "abb.h"
#include <stdio.h>
#include <string.h>
#define ERROR -1
#define EXITO 0
#define IGUALES 0
#define IZQ_MAYOR 1
#define DER_MAYOR -1
#define SIN_HIJOS 0
#define UN_HIJO 1
#define DOS_HIJOS 2
#define DERECHA 1
#define IZQUIERDA -1
#define TAM_MAX_ARRAY 1000
/*
 * Crea el arbol y reserva la memoria necesaria de la estructura.
 * Comparador se utiliza para comparar dos elementos.
 * Destructor es invocado sobre cada elemento que sale del arbol,
 * puede ser NULL indicando que no se debe utilizar un destructor.
 *
 * Devuelve un puntero al arbol creado o NULL en caso de error.
 */
abb_t* arbol_crear(abb_comparador comparador, abb_liberar_elemento destructor){
	abb_t* arbol = malloc(sizeof(abb_t));
	if(!arbol) return NULL;
	arbol->nodo_raiz = NULL;
	arbol->comparador = comparador;
	arbol->destructor = destructor;
	return arbol;
}
nodo_abb_t* crear_nodo_arbol(void* elemento){
	nodo_abb_t* nodo_arbol = malloc(sizeof(nodo_abb_t));
	if(!nodo_arbol) return NULL;
	nodo_arbol->elemento = elemento;
	nodo_arbol->derecha = NULL;
	nodo_arbol->izquierda = NULL;
	return nodo_arbol;
}


int insertar_nodo_en_arbol(abb_t* arbol, nodo_abb_t* padre, nodo_abb_t* nodo_hoja){
	int pos = arbol->comparador(padre->elemento, nodo_hoja->elemento);
 	
 	if(pos == DER_MAYOR || pos == IGUALES){
 		if(!padre->derecha){
 			padre->derecha = nodo_hoja;
 		}
 		else
 			return insertar_nodo_en_arbol(arbol, padre->derecha, nodo_hoja);
 	}
 	if(pos == IZQ_MAYOR){
 		if(!padre->izquierda){
 			padre->izquierda = nodo_hoja;
 		}
 		else
 			return insertar_nodo_en_arbol(arbol, padre->izquierda, nodo_hoja);
 	}
 	return EXITO;
}
/*
 * Inserta un elemento en el arbol.
 * Devuelve 0 si pudo insertar o -1 si no pudo.
 * El arbol admite elementos con valores repetidos.
 */
 int arbol_insertar(abb_t* arbol, void* elemento){
 	if(!arbol) return ERROR;
 	nodo_abb_t* nodo_hoja = crear_nodo_arbol(elemento);
 	if(!nodo_hoja) return ERROR;
 
 	if(!arbol->nodo_raiz){
 		arbol->nodo_raiz = nodo_hoja;
 		return EXITO;
 	}
 	nodo_abb_t* padre = arbol->nodo_raiz;
 	insertar_nodo_en_arbol(arbol, padre, nodo_hoja);
 	return EXITO;
 }

 /*Devuelve el nodo maximo de la rama del subarbol izquierdo*/
nodo_abb_t* encontrar_el_maximo_left_subtree(nodo_abb_t* nodo_subarbol){
	if(!nodo_subarbol->derecha) return nodo_subarbol;
	return encontrar_el_maximo_left_subtree(nodo_subarbol->derecha);
}

int cantidad_hijos(nodo_abb_t* nodo_a_borrar){
	if(!nodo_a_borrar) return 0;
	
	if(!nodo_a_borrar->izquierda && !nodo_a_borrar->derecha)	return 0;
	
	if(nodo_a_borrar->derecha!=NULL && !nodo_a_borrar->izquierda)	return 1;
	
	if(!nodo_a_borrar->derecha && nodo_a_borrar->izquierda!=NULL)	return 1;
	
	return 2;
}


nodo_abb_t* delete(abb_t* arbol, nodo_abb_t* nodo, void* elemento, abb_liberar_elemento liberar){
	if(!nodo) return NULL;

	if(arbol->comparador(nodo->elemento, elemento) == IZQ_MAYOR){
		nodo->izquierda = delete(arbol, nodo->izquierda, elemento, liberar);
	}
	if(arbol->comparador(nodo->elemento, elemento) == DER_MAYOR){
		nodo->derecha = delete(arbol, nodo->derecha, elemento, liberar);
	}

	if(arbol->comparador(nodo->elemento, elemento) == IGUALES){

		if(liberar!=NULL){
			liberar(nodo->elemento);
		}
		if(cantidad_hijos(nodo)==0){
			free(nodo);
			nodo = NULL;
			return nodo;
		}
		if(cantidad_hijos(nodo)==1){
			if(!nodo->izquierda){					
				nodo->elemento = nodo->derecha->elemento;	
				nodo->derecha = delete(arbol, nodo->derecha, nodo->elemento, NULL);				
				return nodo;	
			}
			if(!nodo->derecha){	
				nodo->elemento = nodo->izquierda->elemento;
				nodo->izquierda = delete(arbol, nodo->izquierda, nodo->elemento, NULL);
				return nodo;				
			}			
		}
		if(cantidad_hijos(nodo)== 2){
			nodo_abb_t* maximo_subarbol = encontrar_el_maximo_left_subtree(nodo->izquierda);
			nodo->elemento = maximo_subarbol->elemento;
			nodo->izquierda = delete(arbol, nodo->izquierda, nodo->elemento, NULL);
		}
	}
	return nodo;
}



/*Devuelve el elemento buscado en el arbol, si no, devuelve NULL*/
void* elemento_esta_en_arbol(abb_t* arbol, nodo_abb_t* nodo_actual, void* elemento_buscado){
	if(!arbol || !nodo_actual) return NULL;
	int pos = arbol->comparador(nodo_actual->elemento, elemento_buscado);

	if(pos == 0) return nodo_actual->elemento;
	if(pos == DER_MAYOR){	
		return elemento_esta_en_arbol(arbol, nodo_actual->derecha , elemento_buscado);
	}
	if(pos == IZQ_MAYOR)
	    return elemento_esta_en_arbol(arbol, nodo_actual->izquierda , elemento_buscado);
	return NULL;
}


/*
 * Busca en el arbol un elemento igual al provisto (utilizando la
 * funcion de comparación) y si lo encuentra lo quita del arbol.
 * Adicionalmente, si encuentra el elemento, invoca el destructor con
 * dicho elemento.
 * Devuelve 0 si pudo eliminar el elemento o -1 en caso contrario.
 */
int arbol_borrar(abb_t* arbol, void* elemento){
	if(!arbol || !arbol->nodo_raiz) return -1;
	if(!elemento_esta_en_arbol(arbol, arbol->nodo_raiz, elemento)) return -1;
	if(arbol->comparador(arbol->nodo_raiz->elemento, elemento)==0 && cantidad_hijos(arbol->nodo_raiz)==0){
		arbol->destructor(arbol->nodo_raiz->elemento);
		arbol->nodo_raiz->elemento = NULL;
		free(arbol->nodo_raiz);
		arbol->nodo_raiz = NULL;
		return EXITO;
	}
	delete(arbol, arbol->nodo_raiz, elemento, arbol->destructor);
	return EXITO;
}

/*
 * Busca en el arbol un elemento igual al provisto (utilizando la
 * funcion de comparación).
 *
 * Devuelve el elemento encontrado o NULL si no lo encuentra.
 */
void* arbol_buscar(abb_t* arbol, void* elemento){
	if(!arbol || !arbol->nodo_raiz) return NULL;
	nodo_abb_t* nodo_actual = arbol->nodo_raiz;
    void* encontro = NULL;
    encontro = elemento_esta_en_arbol(arbol, nodo_actual, elemento);
    return encontro;
}


/*
 * Devuelve el elemento almacenado como raiz o NULL si el árbol está
 * vacío o no existe.
 */
void* arbol_raiz(abb_t* arbol){
	if(!arbol || !arbol->nodo_raiz) return NULL;
	return arbol->nodo_raiz->elemento;
}

/*
 * Determina si el árbol está vacío.
 * Devuelve true si está vacío o el arbol es NULL, false si el árbol tiene elementos.
 */
bool arbol_vacio(abb_t* arbol){
	if(!arbol || !arbol->nodo_raiz) return true;
	return false;
}


void visitar_inorden(nodo_abb_t* nodo, void** array, int tam_max, int* iterador){
	if(!nodo) return;
	visitar_inorden(nodo->izquierda, array, tam_max, iterador);
	if(*iterador < tam_max){
		array[*iterador] = nodo->elemento;
		(*iterador)++;
	} 
	visitar_inorden(nodo->derecha, array, tam_max, iterador);
}
/*
 * Llena el array del tamaño dado con los elementos de arbol
 * en secuencia inorden.
 * Devuelve la cantidad de elementos del array que pudo llenar (si el
 * espacio en el array no alcanza para almacenar todos los elementos,
 * llena hasta donde puede y devuelve la cantidad de elementos que
 * pudo poner).
 */
int arbol_recorrido_inorden(abb_t* arbol, void** array, int tamanio_array){
	if(!arbol || !array || tamanio_array<=0) return 0;
	int iter=0;
	visitar_inorden(arbol->nodo_raiz, array, tamanio_array, &iter);
	return iter;	
} 

/*Realiza la visita en PREORDEN*/
void visitar_preorder(nodo_abb_t* nodo, void** array, int tam_max, int* iterador){
	if(!nodo) return;
	if(*iterador<tam_max){
		array[*iterador] = nodo->elemento;
		(*iterador)++;
	}
	visitar_preorder(nodo->izquierda,array, tam_max, iterador);
	visitar_preorder(nodo->derecha,array, tam_max, iterador);
}
/*
 * Llena el array del tamaño dado con los elementos de arbol
 * en secuencia preorden.
 * Devuelve la cantidad de elementos del array que pudo llenar (si el
 * espacio en el array no alcanza para almacenar todos los elementos,
 * llena hasta donde puede y devuelve la cantidad de elementos que
 * pudo poner).
 */

int arbol_recorrido_preorden(abb_t* arbol, void** array, int tamanio_array){
	if(!arbol || !array || tamanio_array<=0) return 0;
	int iter=0;
	visitar_preorder(arbol->nodo_raiz, array, tamanio_array, &iter);
	return iter;
}



								
void visitar_postorden(nodo_abb_t* nodo, void** array, int tam_max, int* iterador){
	if(!nodo) return;
	visitar_postorden(nodo->izquierda, array, tam_max, iterador);
	visitar_postorden(nodo->derecha, array, tam_max, iterador);
	if(*iterador<tam_max){
		array[*iterador] = nodo->elemento;
		(*iterador)++;
	}
}
/*
 * Llena el array del tamaño dado con los elementos de arbol
 * en secuencia postorden.
 * Devuelve la cantidad de elementos del array que pudo llenar (si el
 * espacio en el array no alcanza para almacenar todos los elementos,
 * llena hasta donde puede y devuelve la cantidad de elementos que
 * pudo poner).
 */
int arbol_recorrido_postorden(abb_t* arbol, void** array, int tamanio_array){
	if(!arbol || !array || tamanio_array<=0) return 0;
	int iter=0;
	visitar_postorden(arbol->nodo_raiz, array, tamanio_array, &iter);
	return iter;
}

void destruir_nodos_arbol(abb_t* arbol, nodo_abb_t* nodo_actual){
	if(!nodo_actual) return;
	nodo_abb_t* nodo_borrar = nodo_actual;
	destruir_nodos_arbol(arbol, nodo_actual->izquierda);
	destruir_nodos_arbol(arbol, nodo_actual->derecha);
	arbol->destructor(nodo_borrar->elemento);
	free(nodo_borrar);
}

/*
 * Destruye el arbol liberando la memoria reservada por el mismo.
 * Adicionalmente invoca el destructor con cada elemento presente en
 * el arbol.
 */
void arbol_destruir(abb_t* arbol){
	if(!arbol) return;
	if(arbol->nodo_raiz!=NULL) 	destruir_nodos_arbol(arbol, arbol->nodo_raiz);
	free(arbol);
}

/*
 * Iterador interno. Recorre el arbol e invoca la funcion con cada
 * elemento del mismo. El puntero 'extra' se pasa como segundo
 * parámetro a la función. Si la función devuelve true, se finaliza el
 * recorrido aun si quedan elementos por recorrer. Si devuelve false
 * se sigue recorriendo mientras queden elementos.
 * El recorrido se realiza de acuerdo al recorrido solicitado.  Los
 * recorridos válidos son: ABB_RECORRER_INORDEN, ABB_RECORRER_PREORDEN
 * y ABB_RECORRER_POSTORDEN.
*/
void abb_con_cada_elemento(abb_t* arbol, int recorrido, bool (*funcion)(void*, void*), void* extra){
	if(!arbol) return;
	int tam_max = TAM_MAX_ARRAY;
	void* array[TAM_MAX_ARRAY];
	int total_elementos;
	if(recorrido == ABB_RECORRER_INORDEN){
		total_elementos = arbol_recorrido_inorden(arbol, array, tam_max);
	}
	if(recorrido == ABB_RECORRER_PREORDEN){
		total_elementos = arbol_recorrido_preorden(arbol, array, tam_max);
	}
	if(recorrido == ABB_RECORRER_POSTORDEN){
		total_elementos = arbol_recorrido_postorden(arbol, array, tam_max);
	}
	for(int i = 0; i < total_elementos; i++){
			if(funcion(array[i],extra)== true) return;
	}
}

//compilar: gcc *.c -o abb -g -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O0
//valgrind: valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./abb
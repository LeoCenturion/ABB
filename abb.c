#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "abb.h"
#include "pila.h"

struct _nodo
{
	char *clave;
	void *dato;
};

typedef struct _nodo nodo_t;

struct abb
{
	nodo_t* raiz;
	struct abb *rama_izq, *rama_der;
	abb_comparar_clave_t f_comparacion;
	abb_destruir_dato_t f_destrucion;
	size_t cant_elementos;
};

struct abb_iter
{
	pila_t* pila_inorder;
};

/* establecemos que f_comparacion devuelve 1 si es mayor
 * y -1 si el dato es menor */

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato)
{
	abb_t* arbol = malloc( sizeof(abb_t) );
	if( !arbol )
		return NULL;
	nodo_t* raiz = malloc(sizeof(nodo_t));
	if(!raiz){
		free(arbol);
		return NULL;
	}
	arbol->raiz=raiz;
	arbol->raiz->clave = NULL;
	arbol->raiz->dato = NULL;
	arbol->f_comparacion = cmp;
	arbol->f_destrucion = destruir_dato;
	arbol->rama_izq = NULL;
	 arbol->rama_der = NULL;
	arbol->cant_elementos = 0;

	return arbol;
}

bool _abb_guardar(abb_t *arbol, const char *clave, void *dato)
{
	char* copia_clave;
	if( !arbol->cant_elementos ) /* primer elemento que agregamos?*/
	{
		copia_clave = strdup(clave);
		arbol->raiz->clave = copia_clave;
		arbol->raiz->dato = dato;
		arbol->cant_elementos=1;
		return true;
	}
	bool nodo_creado=true;
	int comparacion=arbol->f_comparacion(clave, arbol->raiz->clave);
	if(comparacion==0){
		if(arbol->f_destrucion){
			arbol->f_destrucion(arbol->raiz->dato);
		}
		arbol->raiz->dato=dato;
		nodo_creado=false;
	}
	else if(comparacion > 0){
		if(arbol->rama_izq==NULL){
			abb_t* nuevo_hijo=abb_crear(arbol->f_comparacion,arbol->f_destrucion);
			if(!nuevo_hijo){
				return false;
			}
			nuevo_hijo->cant_elementos=1;
			copia_clave = strdup(clave);
			nuevo_hijo->raiz->clave=copia_clave;
			nuevo_hijo->raiz->dato=dato;
			arbol->rama_izq = nuevo_hijo;
		}
		else{
			nodo_creado=_abb_guardar(arbol->rama_izq,clave, dato);
		}

	}
	else if( comparacion < 0 )
	{
		if(arbol->rama_der==NULL){
			abb_t* nuevo_hijo= abb_crear(arbol->f_comparacion,arbol->f_destrucion);
			if(!nuevo_hijo){
				return false;
			}
			nuevo_hijo->cant_elementos=1;
			copia_clave = strdup(clave);
			nuevo_hijo->raiz->clave=copia_clave;
			nuevo_hijo->raiz->dato=dato;
			arbol->rama_der = nuevo_hijo;
		}
		else{
			nodo_creado=_abb_guardar(arbol->rama_der, clave, dato);
		}
	}
	if(nodo_creado){
		arbol->cant_elementos++;
	}
	return nodo_creado;
}

bool abb_guardar(abb_t* arbol,const char* clave, void* dato){
	_abb_guardar(arbol,clave,dato);
	return true;
}
void* abb_borrar_recursivo(abb_t** arbol_p, const char* clave,pila_t* camino_recorrido){
	abb_t* arbol = *arbol_p;
	void *dato;
	if(!arbol){
		return NULL;
	}
	int comparacion=arbol->f_comparacion(clave, arbol->raiz->clave);
	if( comparacion >  0 ){
		pila_apilar(camino_recorrido,arbol);
		dato=abb_borrar_recursivo(&arbol->rama_izq, clave,camino_recorrido);
		pila_desapilar(camino_recorrido);
		if(dato){
			arbol->cant_elementos--;
		}
	}
	else if( comparacion < 0 ){
		pila_apilar(camino_recorrido,arbol);
		dato=abb_borrar_recursivo(&arbol->rama_der, clave,camino_recorrido);
		pila_desapilar(camino_recorrido);
		if(dato){
			arbol->cant_elementos--;
		}
	}
	else{
	 //caso en que este es el nodo que corresponde borrar

		if( !arbol->rama_izq && !arbol->rama_der ){
			// Arbol no tiene ningun hijo izquierdo ni derecho
			dato = arbol->raiz->dato;
			if(!pila_esta_vacia(camino_recorrido)){
				abb_t* padre= pila_desapilar(camino_recorrido);
				if(padre->rama_izq==arbol){
					padre->rama_izq=NULL;
				}
				else if(padre->rama_der==arbol){
					padre->rama_der=NULL;
				}
				free(arbol->raiz->clave);
				free(arbol->raiz);
				free(arbol);
			}
			else{
				//caso en que el nodo es la raiz
				free(arbol->raiz->clave);
				arbol->raiz->dato=NULL;
				arbol->cant_elementos=0;

			}
		}
		else if( (!arbol->rama_izq && arbol->rama_der) || (arbol->rama_izq && !arbol->rama_der) )
		{

			// tiene un hijo, necesitamos reemplazar
			// si falta la parte derecha, reemplazamos por el izquierdo
			// Si falta la parte izquierda, reemplazamos por la derecha
			dato = arbol->raiz->dato;
			abb_t* siguiente;
			if(arbol->rama_der){
				siguiente=arbol->rama_der;
			}
			else{
				siguiente=arbol->rama_izq;
			}
			free(arbol->raiz->clave);
			free(arbol->raiz);
			if(!pila_esta_vacia(camino_recorrido)){
				abb_t* padre=pila_desapilar(camino_recorrido);
				if(padre->rama_izq==arbol){
					padre->rama_izq=siguiente;
				}
				else if(padre->rama_der==arbol){
					padre->rama_der=siguiente;
				}
				free(arbol);
			}
			else{
				arbol->rama_izq=siguiente->rama_izq;
				arbol->rama_der=siguiente->rama_der;
				arbol->raiz=siguiente->raiz;
				arbol->cant_elementos--;
			}

		}
		else
		{
			abb_t* mayor_izq_padre=arbol;
			abb_t* mayor_izq = arbol->rama_izq;
			while(mayor_izq && mayor_izq->rama_der != NULL ){
				mayor_izq_padre=mayor_izq;
				mayor_izq = mayor_izq->rama_der;
			}
			dato=arbol->raiz->dato;
			free(arbol->raiz->clave);
			free(arbol->raiz);
			arbol->raiz=mayor_izq->raiz;
			arbol->cant_elementos--;
			if(mayor_izq_padre==arbol){
				arbol->rama_izq=mayor_izq->rama_izq;
			}
			else{
				mayor_izq_padre->rama_der = mayor_izq->rama_izq;
			}
			free(mayor_izq);

		}

	}
	return dato;

}

void *abb_borrar(abb_t *arbol, const char *clave)
{
	if ( !arbol || arbol->cant_elementos==0 ){
		return NULL;
	}
	pila_t* camino_recorrido=pila_crear();
	void* dato=abb_borrar_recursivo(&arbol,clave,camino_recorrido);
	pila_destruir(camino_recorrido);
	return dato;
}

void *abb_obtener(const abb_t *arbol, const char *clave)
{
	if ( !arbol || arbol->cant_elementos==0 ){
		return NULL;
	}
	if( arbol->f_comparacion(clave, arbol->raiz->clave) > 0 )
		return abb_obtener(arbol->rama_izq, clave);
	else if( arbol->f_comparacion(clave, arbol->raiz->clave) < 0 )
		return abb_obtener(arbol->rama_der, clave);
	else
	{
		void *dato = arbol->raiz->dato;
		return dato;
	}
}

bool abb_pertenece(const abb_t *arbol, const char *clave)
{
	if ( !arbol || arbol->cant_elementos==0 ){
		return false;
	}
	int comparacion=arbol->f_comparacion(clave, arbol->raiz->clave);
	if( comparacion > 0 )
		return abb_obtener(arbol->rama_izq, clave);
	else if( comparacion< 0 )
		return abb_obtener(arbol->rama_der, clave);
	else
	{
		return true;
	}
}

size_t abb_cantidad(abb_t *arbol)
{
	return arbol->cant_elementos;
}

void abb_destruir(abb_t *arbol)
{
	if( !arbol ){
		return;
	}

	abb_destruir(arbol->rama_izq);
	abb_destruir(arbol->rama_der);

	if(arbol->cant_elementos){
		free(arbol->raiz->clave);
		if(arbol->f_destrucion ){
			arbol->f_destrucion(arbol->raiz->dato);
		}
	}
	free(arbol->raiz);
	free(arbol);
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra)
{
	if( arbol )
	{
		abb_in_order(arbol->rama_izq, visitar, extra);
		visitar(arbol->raiz->clave, arbol->raiz->dato, extra);
		abb_in_order(arbol->rama_der, visitar, extra);
	}
}
void abb_pre_order(abb_t *arbol, bool visitar(const char *, void *, void *),void* extra){
	if( arbol )
	{
		visitar(arbol->raiz->clave, arbol->raiz->dato, extra);
		abb_in_order(arbol->rama_izq, visitar, extra);
		abb_in_order(arbol->rama_der, visitar, extra);
	}
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol)
{
	abb_iter_t* iterador = malloc( sizeof(abb_iter_t) );

	if( !iterador )
		return NULL;

	iterador->pila_inorder = pila_crear();

	if( !iterador->pila_inorder )
	{
		free(iterador);
		return NULL;
	}
	if(arbol->cant_elementos){
		pila_apilar(iterador->pila_inorder,(void*)arbol);

		abb_t* tmp = arbol->rama_der;

		/*apilamos todos los izquierdos*/
		while( tmp )
		{
			pila_apilar(iterador->pila_inorder, tmp);
			tmp = tmp->rama_der;
		}
	}
	return iterador;
}
bool abb_iter_in_al_final(const abb_iter_t* iter){
	return pila_esta_vacia(iter->pila_inorder);
}
bool abb_iter_in_avanzar(abb_iter_t *iter)
{
	if(abb_iter_in_al_final(iter)){
		return false;
	}
	abb_t* desapilado = pila_desapilar(iter->pila_inorder);
	abb_t* tmp = desapilado->rama_izq;

	while( tmp )
	{
		pila_apilar(iter->pila_inorder, tmp);
		tmp = tmp->rama_der;
	}
	return true;
}
const char *abb_iter_in_ver_actual(const abb_iter_t *iter)
{
	if( abb_iter_in_al_final(iter) )
		return NULL;
	abb_t* abb_actual=pila_ver_tope(iter->pila_inorder);
	return abb_actual->raiz->clave;
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila_inorder);
	free(iter);
}

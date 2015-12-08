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
	struct _nodo *rama_izq, *rama_der;

};

	typedef struct _nodo nodo_t;

struct abb
{
	nodo_t* raiz;
	abb_comparar_clave_t f_comparacion;
	abb_destruir_dato_t f_destrucion;
	size_t cant_elementos;
};

struct abb_iter
{
	pila_t* pila_inorder;
};

//La clave ya debe ser un duplicado de la original
nodo_t* nodo_abb_crear(char* clave, void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if(!nodo){
		return NULL;
	}
	nodo->clave=clave;
	nodo->dato=dato;
	nodo->rama_izq=NULL;
	nodo->rama_der=NULL;
	return nodo;
}
/* establecemos que f_comparacion devuelve 1 si es mayor
 * y -1 si el dato es menor */

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato)
{
	abb_t* arbol = malloc( sizeof(abb_t) );
	if( !arbol )
		return NULL;
	nodo_t* raiz = nodo_abb_crear(NULL,NULL);
	if(!raiz){
		free(arbol);
		return NULL;
	}
	arbol->raiz=raiz;
	arbol->f_comparacion = cmp;
	arbol->f_destrucion = destruir_dato;
	arbol->cant_elementos = 0;

	return arbol;
}

bool _abb_guardar(abb_t *arbol,nodo_t* nodo, const char *clave, void *dato)
{
	char* copia_clave;
	if( !arbol->cant_elementos ) /* primer elemento que agregamos?*/
	{
		copia_clave=(clave==NULL)?NULL:strdup(clave); // Leo: se contempla el caso
		arbol->raiz->clave = copia_clave;			//en que la clave es NULL
		arbol->raiz->dato = dato;
		arbol->cant_elementos=1;
		return true;
	}
	int comparacion=arbol->f_comparacion(clave,nodo->clave);
	if(comparacion==0){
		if(arbol->f_destrucion){
			arbol->f_destrucion(nodo->dato);
		}
		nodo->dato=dato;
	}
	else if(comparacion < 0){
		if(nodo->rama_izq==NULL){
			copia_clave=(clave==NULL)?NULL:strdup(clave);
			nodo_t* nuevo_hijo=nodo_abb_crear(copia_clave,dato);
			if(!nuevo_hijo){
				return false;
			}
			nodo->rama_izq = nuevo_hijo;
			arbol->cant_elementos++;

		}
		else{
			_abb_guardar(arbol,nodo->rama_izq,clave, dato);
		}

	}
	else if( comparacion > 0 ){
		if(nodo->rama_der==NULL){
			copia_clave=(clave==NULL)?NULL:strdup(clave);
			nodo_t* nuevo_hijo=nodo_abb_crear(copia_clave,dato);
			if(!nuevo_hijo){
				return false;
			}
			nodo->rama_der = nuevo_hijo;
			arbol->cant_elementos++;
		}
		else{
			_abb_guardar(arbol,nodo->rama_der, clave, dato);
		}
	}
	return true;

}

bool abb_guardar(abb_t* arbol,const char* clave, void* dato){
	_abb_guardar(arbol,arbol->raiz,clave,dato);
	return true;
}
void* abb_borrar_recursivo(abb_t* arbol,nodo_t* nodo, const char* clave,pila_t* camino_recorrido){
	void *dato;
	if(!nodo){
		return NULL;
	}
	int comparacion=arbol->f_comparacion(clave, nodo->clave);
	if( comparacion <  0 ){
		pila_apilar(camino_recorrido,nodo);
		dato=abb_borrar_recursivo(arbol,nodo->rama_izq,clave,camino_recorrido);
		pila_desapilar(camino_recorrido);
	}
	else if( comparacion > 0 ){
		pila_apilar(camino_recorrido,nodo);
		dato=abb_borrar_recursivo(arbol,nodo->rama_der, clave,camino_recorrido);
		pila_desapilar(camino_recorrido);

	}
	else{
	 //caso en que este es el nodo que corresponde borrar
		if( !nodo->rama_izq && !nodo->rama_der ){
			// Arbol no tiene ningun hijo izquierdo ni derecho
			dato = nodo->dato;
			if(!pila_esta_vacia(camino_recorrido)){
				nodo_t* padre= pila_desapilar(camino_recorrido);
				if(padre->rama_izq==nodo){
					padre->rama_izq=NULL;
				}
				else if(padre->rama_der==nodo){
					padre->rama_der=	NULL;
				}
				arbol->cant_elementos--;
				free(nodo->clave);
				free(nodo);
			}
			else{
				//caso en que el nodo es la raiz
				free(nodo->clave);
				arbol->raiz->clave=NULL;
				arbol->raiz->dato=NULL;
				arbol->cant_elementos=0;

			}
		}
		else if( (!nodo->rama_izq && nodo->rama_der) || (nodo->rama_izq && !nodo->rama_der) )
		{

			// tiene un hijo, necesitamos reemplazar
			// si falta la parte derecha, reemplazamos por el izquierdo
			// Si falta la parte izquierda, reemplazamos por la derecha
			dato = nodo->dato;
			nodo_t* siguiente;
			if(nodo->rama_der){
				siguiente=nodo->rama_der;
			}
			else{
				siguiente=nodo->rama_izq;
			}
			free(nodo->clave);
			if(!pila_esta_vacia(camino_recorrido)){
				nodo_t* padre=pila_desapilar(camino_recorrido);
				if(padre->rama_izq==nodo){
					padre->rama_izq=siguiente;
				}
				else if(padre->rama_der==nodo){
					padre->rama_der=siguiente;
				}
				free(nodo);
			}
			else{ //es la raiz
				free(nodo);
				arbol->raiz=siguiente;
			}
			arbol->cant_elementos--;

		}
		else
		{
			nodo_t* mayor_izq_padre=nodo;
			nodo_t* mayor_izq = nodo->rama_izq;
			while(mayor_izq && mayor_izq->rama_der != NULL ){
				mayor_izq_padre=mayor_izq;
				mayor_izq = mayor_izq->rama_der;
			}
			dato=nodo->dato;
			free(nodo->clave);
			nodo->dato=mayor_izq->dato;
			nodo->clave=mayor_izq->clave;
			arbol->cant_elementos--;
			if(mayor_izq_padre==nodo){
				nodo->rama_izq=mayor_izq->rama_izq;
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
	void* dato=abb_borrar_recursivo(arbol,arbol->raiz,clave,camino_recorrido);
	pila_destruir(camino_recorrido);
	return dato;
}

void* _abb_obtener(const abb_t* arbol, nodo_t* nodo,const char* clave){
	if ( !nodo){
		return NULL;
	}
	if( arbol->f_comparacion(clave, nodo->clave) < 0 )
		return _abb_obtener(arbol,nodo->rama_izq, clave);
	else if( arbol->f_comparacion(clave, nodo->clave) > 0 )
		return _abb_obtener(arbol,nodo->rama_der, clave);
	else
	{
		void *dato = nodo->dato;
		return dato;
	}
}

void *abb_obtener(const abb_t *arbol, const char *clave)
{

	if ( !arbol || arbol->cant_elementos==0 ){
		return NULL;
	}
	if( arbol->f_comparacion(clave, arbol->raiz->clave) < 0 )
		return _abb_obtener(arbol,arbol->raiz->rama_izq, clave);
	else if( arbol->f_comparacion(clave, arbol->raiz->clave) > 0 )
		return _abb_obtener(arbol,arbol->raiz->rama_der, clave);
	else
	{
		void *dato = arbol->raiz->dato;
		return dato;
	}
}
bool _abb_pertenece(const abb_t* arbol, nodo_t* nodo, const char* clave){
	if ( !nodo ){
		return false;
	}
	int comparacion=arbol->f_comparacion(clave, nodo->clave);
	if( comparacion < 0 )
		return _abb_pertenece(arbol,nodo->rama_izq, clave);
	else if( comparacion> 0 )
		return _abb_pertenece(arbol,nodo->rama_der, clave);
	else
	{
		return true;
	}
}

bool abb_pertenece(const abb_t *arbol, const char *clave)
{
	if ( !arbol || arbol->cant_elementos==0 ){
		return false;
	}
	int comparacion=arbol->f_comparacion(clave, arbol->raiz->clave);
	if( comparacion < 0 )
		return _abb_pertenece(arbol,arbol->raiz->rama_izq, clave);
	else if( comparacion> 0 )
		return _abb_pertenece(arbol,arbol->raiz->rama_der, clave);
	else
	{
		return true;
	}
}

size_t abb_cantidad(abb_t *arbol)
{
	return arbol->cant_elementos;
}
void _abb_destruir(abb_t* arbol, nodo_t* nodo){
	if( !nodo ){
		return;
	}

	_abb_destruir(arbol,nodo->rama_izq);
	_abb_destruir(arbol,nodo->rama_der);


		if(arbol->f_destrucion ){
			arbol->f_destrucion(nodo->dato);
		}
	free(nodo->clave);
	free(nodo);
}
void abb_destruir(abb_t *arbol)
{
	if( !arbol ){
		return;
	}

	_abb_destruir(arbol,arbol->raiz->rama_izq);
	_abb_destruir(arbol,arbol->raiz->rama_der);

	if(arbol->cant_elementos){
		free(arbol->raiz->clave);
		if(arbol->f_destrucion ){
			arbol->f_destrucion(arbol->raiz->dato);
		}
	}
	free(arbol->raiz);
	free(arbol);
}
void _abb_in_order(nodo_t* nodo, bool visitar(const char *, void *, void *), void *extra ){
	if( nodo )
	{
		_abb_in_order(nodo->rama_izq, visitar, extra);
		if(visitar(nodo->clave, nodo->dato, extra)){
			_abb_in_order(nodo->rama_der, visitar, extra);
		}
	}
}
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra)
{
	if( arbol )
	{
		_abb_in_order(arbol->raiz->rama_izq, visitar, extra);
		if(visitar(arbol->raiz->clave, arbol->raiz->dato, extra)){
			_abb_in_order(arbol->raiz->rama_der, visitar, extra);
		}
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
		pila_apilar(iterador->pila_inorder,(void*)arbol->raiz);

		nodo_t* tmp = arbol->raiz->rama_izq;

		/*apilamos todos los izquierdos*/
		while( tmp )
		{
			pila_apilar(iterador->pila_inorder, tmp);
			tmp = tmp->rama_izq;
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
	nodo_t* desapilado = pila_desapilar(iter->pila_inorder);
	nodo_t* tmp = desapilado->rama_der;

	while( tmp )
	{
		pila_apilar(iter->pila_inorder, tmp);
		tmp = tmp->rama_izq;
	}
	return true;
}
const char *abb_iter_in_ver_actual(const abb_iter_t *iter)
{
	if( abb_iter_in_al_final(iter) )
		return NULL;
	nodo_t* nodo_actual=pila_ver_tope(iter->pila_inorder);
	return nodo_actual->clave;
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila_inorder);
	free(iter);
}

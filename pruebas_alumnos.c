#define _XOPEN_SOURCE 500
#include "testing.h"
#include <stdlib.h>
#include "abb.h"
#include <stdio.h>
#include <string.h>
#include "cola.h"
void imprimir_cadena(const char cadena[]){
    int actual=0;
    while(cadena[actual]!='\0'){
        printf("%c",cadena[actual]);
        actual++;
    }
    printf("\n");
}
bool imprimir_clave(const char* cadena, void* dato , void* extra){
    imprimir_cadena(cadena);
    return true;
}
bool guardar_datos_inorder(const char* cadena, void* dato, void* array){
    int* dato_entero = dato;
    int* arreglo_inorder=array;
    int i=0;
    while(arreglo_inorder[i]!=-1){
        i++;
    }
    arreglo_inorder[i]=*dato_entero;
    return true;
}
/*bool guardar_datos_inorder(const char* cadena, void* dato, void* extra){
    int* dato_p = dato;
    int dato_i = *dato_p;
    cola_t* cola=extra;
    cola_encolar(cola,&dato_i);
    return true;
}*/
void prueba_abb_vacio(void){
    printf("ABB VACIO:\n");
    abb_t* arbol = abb_crear(strcmp,NULL);
    print_test("Arbol vacío...",abb_cantidad(arbol)==0);
    print_test("Borrar...",abb_borrar(arbol,"cadena")==NULL);
    print_test("Obtener...",abb_obtener(arbol,"cadena")==NULL);
    print_test("Pertenece...",abb_pertenece(arbol,"cadena")==false);

    printf("ITERADOR:\n");
    abb_iter_t* iter=abb_iter_in_crear(arbol);
    print_test("Iterador actual es NULL...",abb_iter_in_ver_actual(iter)==NULL);
    print_test("Iterador al final...",abb_iter_in_al_final(iter)==true);
    print_test("No se puede avanzar...",abb_iter_in_avanzar(iter)==false);
    abb_iter_in_destruir(iter);
    abb_destruir(arbol);

}
void prueba_abb_clave_vacia(void){
    printf("CLAVE VACIA:\n");
    abb_t* arbol = abb_crear(strcmp,NULL);
    char* clave ="";
    int valor = 5;
    print_test("Guardar clave vacia...",abb_guardar(arbol,clave,&valor));
    print_test("Tamanio correcto...",abb_cantidad(arbol)==1);
    print_test("Clave pertenece...",abb_pertenece(arbol,clave));
    print_test("Obtener valor...",abb_obtener(arbol,clave)==&valor);
    print_test("Borrar clave...",abb_borrar(arbol,clave)==&valor);
    char* clave_2="no_vacia";
    int valor_2=3;
    abb_guardar(arbol,clave,&valor);
    print_test("guardar clave no vacia...",abb_guardar(arbol,clave_2,&valor_2));
    print_test("Tamanio correcto...",abb_cantidad(arbol)==2);
    print_test("Clave vacia pertenece...",abb_pertenece(arbol,clave));
    print_test("Obtener valor clave vacia...",abb_obtener(arbol,clave)==&valor);
    print_test("Clave no vacia pertenece...",abb_pertenece(arbol,clave_2));
    print_test("Obtener valor clave no vacia...",abb_obtener(arbol,clave_2)==&valor_2);
    print_test("Borrar clave no vacia...",abb_borrar(arbol,clave_2)==&valor_2);
    print_test("Borrar clave  vacia...",abb_borrar(arbol,clave)==&valor);

    abb_destruir(arbol);

}
void prueba_abb_guardar_borrar(void){
    abb_t* arbol = abb_crear(strcmp,NULL);

    printf("GUARDAR Y BORRAR:\n");
    printf("GUARDAR:\n");
    int elementos[] = {0,1,2,3,4,5,6,7,8,9};
    char* claves[] = {"cero","uno","dos","tres","cuatro","cinco","seis",
                    "siete","ocho","nueve"};
    for(int i=0;i<10;i++){
        print_test("Guardar entero...",abb_guardar(arbol,claves[i],&elementos[i]));
    }
    print_test("Longitud correcta...",abb_cantidad(arbol)==10);

    for(int i=0;i<10;i++){
        print_test("Elemento guardado pertenece...",abb_pertenece(arbol,claves[i]));
        print_test("Obtener elemento...",abb_obtener(arbol,claves[i])==&elementos[i]);
    }
    printf("BORRAR HOJAS:\n");
    for(int i=9;i>=0;i--){
        print_test("Borrar...",abb_borrar(arbol,claves[i])==&elementos[i]);
        print_test("Longitud correcta...",abb_cantidad(arbol)==i);

    }
    printf("GUARDAR:\n");
    for(int i=0;i<10;i++){
        print_test("Guardar entero...",abb_guardar(arbol,claves[i],&elementos[i]));
    }
    print_test("Longitud correcta...",abb_cantidad(arbol)==10);

    for(int i=0;i<10;i++){
        print_test("Elemento guardado pertenece...",abb_pertenece(arbol,claves[i]));
        print_test("Obtener elemento...",abb_obtener(arbol,claves[i])==&elementos[i]);
    }
    printf("BORRAR NODOS INTERNOS:\n");
    for(int i=0;i<10;i++){
        print_test("Borrar...",abb_borrar(arbol,claves[i])==&elementos[i]);
        print_test("Longitud correcta...",abb_cantidad(arbol)==(9-i));
    }
    abb_destruir(arbol);
}
void prueba_abb_reemplazar(void){
    abb_t* arbol=abb_crear(strcmp,NULL);
    int elementos[] = {0,1,2,3,4,5,6,7,8,9};
    char* claves[] = {"cero","uno","dos","tres","cuatro","cinco","seis",
                    "siete","ocho","nueve"};
    for(int i=0;i<10;i++){
        print_test("Guardar entero...",abb_guardar(arbol,claves[i],&elementos[i]));
    }
    print_test("Longitud correcta...",abb_cantidad(arbol)==10);
    for(int i=0;i<10;i++){
        int* elemento=abb_obtener(arbol,claves[i]);
        (*elemento)++;
        print_test("Modificar entero...",abb_guardar(arbol,claves[i],elemento));
    }
    bool ok=true;
    for(int i=0;i<10;i++){
        int* elemento=abb_obtener(arbol,claves[i]);
        if(!(ok=(abb_pertenece(arbol,claves[i])&& i+1==*elemento))){
            break;
        }
    }
    print_test("Se reemplazaron todos los elementos...",ok);
    abb_destruir(arbol);
}
void probar_abb_borrar(char *orden_insercion, char *orden_borrado) {
  abb_t *abb = abb_crear(strcmp, NULL);
  void *dato = abb;  // O cualquier otro valor.

  // Insertar cada letra como un nodo, en orden.
  for (char *c = orden_insercion; *c; c++) {
    char k[] = { *c, '\0' };
    printf("Insertar \"%s\" (%s)\n", k,
       abb_guardar(abb, k, dato) ? "ok" : "ERROR");
  }

  // Eliminar cada nodo en el orden indicado. Si no se especificó, usar el orden
  // de inserción.
  if (!orden_borrado)
    orden_borrado = orden_insercion;

  for (char *c = orden_borrado; *c; c++) {
    char k[] = { *c, '\0' };
    printf("Borrar \"%s\" (%s)\n", k,
       abb_borrar(abb, k) == dato ? "ok" : "ERROR");
  }

  abb_destruir(abb);
  puts("FIN\n");
}
bool sumar(const char* clave, void* dato, void* extra){
    int* dato_entero= dato;
    dato_entero++;
    return true;
}

void prueba_iterador(void){
    printf("PRUEBAS ITERADORES\n");
    abb_t* arbol = abb_crear(strcmp,NULL);
    int elementos[] = {0,1,2,3,4,5,6,7,8,9};
    char* claves[] = {"cero","uno","dos","tres","cuatro","cinco","seis",
                    "siete","ocho","nueve"};
    for(int i=0;i<10;i++){
        abb_guardar(arbol,claves[i],&elementos[i]);
    }
    int elementos_inorder[10];
    for(int i=0;i<10;i++){
        elementos_inorder[i]=-1;
    }
    abb_in_order(arbol,guardar_datos_inorder,(void*)elementos_inorder);
    char ok = true;
    abb_iter_t* iter = abb_iter_in_crear(arbol);
    for(int i=0; i<10;i++){
        const char* clave_actual=abb_iter_in_ver_actual(iter);
        if(!abb_pertenece(arbol,clave_actual)){
            ok=false;
        }
        abb_iter_in_avanzar(iter);
    }
    print_test("Iterador al final...",abb_iter_in_al_final(iter));
    print_test("Todos los elementos pertenecen...",ok);
    abb_destruir(arbol);
    abb_iter_in_destruir(iter);
}
/*int intcmp(void* a, void* b){
    char* a_c=a;
    char* b_c=b;
    return *b_c-*a_c;
}*/


/*void armar_arbol(abb_t* arbol, size_t i, size_t f,int* elementos){
    if(i>f) return;
    size_t medio = (i+f)/2;
    armar_arbol(arbol,i,medio-1,elementos);
    armar_arbol(arbol,medio+1,f,elementos);
    abb_guardar(arbol,(char*)&elementos[medio],&elementos[medio]);

}*/

void prueba_volumen(void){
    abb_t* arbol = abb_crear(strcmp,NULL);
    char claves[1000][10];
    for(int i=0;i<1000;i++){
        sprintf(claves[i],"%4i",i);
    }
    for(int i=0;i<1000;i++){
        abb_guardar(arbol,claves[i],claves[i]);
    }
    print_test("Longitud correcta...",abb_cantidad(arbol)==1000);
    bool ok=true;
    for(int i=0; i<1000;i++){
        if(!abb_pertenece(arbol,claves[i])){
            ok=false;
        }
    }
    print_test("Todos los elementos pertenecen...",ok);
    for(int i=0;i<1000;i++){
        if(!abb_borrar(arbol,claves[i])){
            ok=false;
        }
    }
    print_test("Se borraron todos los elem...",ok);
    print_test("Longitud correcta...",abb_cantidad(arbol)==0);
    abb_destruir(arbol);
}
void pruebas_abb_alumno(void){
    prueba_abb_vacio();
    prueba_abb_guardar_borrar();
    prueba_abb_reemplazar();
    prueba_abb_clave_vacia();
    prueba_iterador();
    prueba_volumen();
    //probar_abb_borrar("JAHBGICFDE", NULL);  // Da OK
    //probar_abb_borrar("JCIHBDFEAG", NULL);  // Falla

}

#include "pokedex.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define EXITO 0
#define ERROR -1
#define POKEMON_ESPECIE 'E'


/*Comparador para el arbol de pokemons*/
/*tendría que poner la estructura del pokemon? para poder comprar en campo primer_pokemon*/
int comparar_especie(void* primer_pokemon, void* segundo_pokemon){
	if(!primer_pokemon || !segundo_pokemon){
		return 0;
	}
	if(((especie_pokemon_t*)primer_pokemon)->numero > (((especie_pokemon_t*)segundo_pokemon)->numero)){	
		return 1;
	}	

	if(((especie_pokemon_t*)primer_pokemon)->numero < ((especie_pokemon_t*)segundo_pokemon)->numero){		
		return -1;
	}
	return 0;
}

void destruir_pokemones(void* especie_pokemon){
	especie_pokemon_t* especie = (especie_pokemon_t*)especie_pokemon; 
	while(!lista_vacia(especie->pokemones)){
		particular_pokemon_t* pokemon = (particular_pokemon_t*)lista_tope(especie->pokemones);
		free(pokemon);
		lista_desapilar(especie->pokemones);
	}
	free(especie->pokemones);
	free(especie);
}
/*
 * Crea la Pokedex, reservando la memoria necesaria para la misma.
 * Devolverá una referencia a la pokedex creada o NULL si no se pudo crear.
 */
pokedex_t* pokedex_crear(char entrenador[MAX_NOMBRE]){
	pokedex_t* pokedex = malloc(sizeof(pokedex_t));
	if(!pokedex) return NULL;
	strcpy(pokedex->nombre_entrenador, entrenador);

	abb_t* pokemones_arbol = arbol_crear(comparar_especie, destruir_pokemones);
	if(!pokemones_arbol) {
		free(pokedex);
		return NULL;
	}
	pokedex->pokemones = pokemones_arbol;

	lista_t* lista_capturados = lista_crear();
	if(!lista_capturados) {
		free(pokedex->pokemones);
		free(pokedex);	
		return NULL;	
	}
	pokedex->ultimos_capturados = lista_capturados;

	lista_t* lista_vistos = lista_crear();
	if(!lista_vistos){
		free(pokedex->ultimos_capturados);
		free(pokedex->pokemones);
		free(pokedex);
		return NULL;
	}
	pokedex->ultimos_vistos = lista_vistos;

	return pokedex;
}


/*Devuelve un puntero a una nueva especie pokemon*/
especie_pokemon_t* nueva_especie(int numero, char nombre[MAX_NOMBRE], char descripcion[MAX_DESCRIPCION]){
	especie_pokemon_t* especie = malloc(sizeof(especie_pokemon_t));
	if(!especie) return NULL;
	especie->numero = numero;
	strcpy(especie->nombre, nombre);
	strcpy(especie->descripcion, descripcion);
	lista_t* lista_de_pokemones = lista_crear();
	if(!lista_de_pokemones) {
		free(especie);
		return NULL;
	}
	especie->pokemones = lista_de_pokemones;
	return especie;
}

/*Creo un pokemon asignando sus campos*/
particular_pokemon_t* crear_pokemon(char nombre[MAX_NOMBRE], int nivel, char capturado){
	particular_pokemon_t* nuevo_pokemon = malloc(sizeof(particular_pokemon_t));
	if(!nuevo_pokemon) return NULL;
	strcpy(nuevo_pokemon->nombre , nombre);
	nuevo_pokemon->nivel = nivel;
	if(capturado == 'N'){
		nuevo_pokemon->capturado = false;
	}
	else{
		nuevo_pokemon->capturado = true;
	}
	return nuevo_pokemon;
}

particular_pokemon_t* crear_pokemon_pokedex(char nombre[MAX_NOMBRE], int nivel, char* capturado){
	particular_pokemon_t* nuevo_pokemon = malloc(sizeof(particular_pokemon_t));
	if(!nuevo_pokemon) return NULL;
	strcpy(nuevo_pokemon->nombre , nombre);
	nuevo_pokemon->nivel = nivel;
	if(capturado[0] == 'N'){
		nuevo_pokemon->capturado = false;
	}
	else{
		nuevo_pokemon->capturado = true;
	}
	return nuevo_pokemon;
}

/*
 * Función que dado un archivo, deberá cargar en la Pokedex a los
 * Pokémon que fueron tanto capturados como vistos. No verifica si
 * dos Pokémon de la misma especie tienen nombre repetido.
 *
 * Agrega a la pila correspondiente los Pokémon capturados.
 * Agrega a la cola correspondiente los Pokémon avistados.
 *
 * Devuelve 0 si tuvo éxito o -1 si se encuentra algún error durante el proceso.
 */
int pokedex_avistar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
	if(!pokedex || !ruta_archivo) return -1;

	FILE* archivo = fopen(ruta_archivo, "r");
	if(!archivo){
		return ERROR;
	}
	int numero_de_especie, nivel_del_pokemon;
	char especie_pokemon[MAX_NOMBRE], descripcion_especie[MAX_DESCRIPCION], nombre_del_pokemon[MAX_NOMBRE], capturado;
	
	int leidos = fscanf(archivo, "%i;%[^;];%[^;];%[^;];%i;%c\n", &numero_de_especie, especie_pokemon, descripcion_especie,
		nombre_del_pokemon, &nivel_del_pokemon, &capturado);

	if(leidos!=6){ 
		fclose(archivo);
		return ERROR;
	}
	especie_pokemon_t* especie_buscada;
	particular_pokemon_t* nuevo_pokemon;
	especie_pokemon_t especie;
	
	while(leidos == 6){
		especie.numero = numero_de_especie;
		especie_buscada = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, &especie);	
		if(!especie_buscada){
			especie_buscada = nueva_especie(numero_de_especie, especie_pokemon, descripcion_especie);
			arbol_insertar(pokedex->pokemones , especie_buscada);
			printf("\n\nSe agrego una nueva especie\n\n");
		}
		
		nuevo_pokemon = crear_pokemon(nombre_del_pokemon, nivel_del_pokemon, capturado);
		if(!nuevo_pokemon) return ERROR;
		lista_insertar(especie_buscada->pokemones ,nuevo_pokemon);

	
		if(capturado=='S'){
			particular_pokemon_t* nuevo_pokemon_capturado = crear_pokemon(nombre_del_pokemon, nivel_del_pokemon, capturado);
			if(!nuevo_pokemon_capturado) return ERROR;
			lista_apilar(pokedex->ultimos_capturados, nuevo_pokemon_capturado);
		}

		particular_pokemon_t* nuevo_pokemon_visto = crear_pokemon(nombre_del_pokemon, nivel_del_pokemon, capturado);
		if(!nuevo_pokemon_visto) return ERROR;
		lista_encolar(pokedex->ultimos_vistos, nuevo_pokemon_visto);
		
		leidos = fscanf(archivo, "%i;%[^;];%[^;];%[^;];%i;%c\n", &numero_de_especie, especie_pokemon, descripcion_especie,nombre_del_pokemon, &nivel_del_pokemon, &capturado);	
	}
	fclose(archivo);
	return EXITO;
}

/*
 * Función que dado un archivo, deberá cargar en la Pokedex a los
 * Pokémon que evolucionaron.
 *
 * La evolución de Pokémon no afecta a la pila de capturados ni a la
 * cola de vistos. Un Pokémon que no está marcado como capturado no
 * puede evolucionar.
 *
 * Devuelve 0 si tuvo éxito o -1 si se encuentra algún error durante
 * el proceso (por ejemplo si no existe la especie, el Pokémon
 * específico o si el Pokémon no fue capturado).
 */
int pokedex_evolucionar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
	if(!pokedex) return ERROR;
	FILE* archivo = fopen(ruta_archivo, "r");
	if(!archivo) return ERROR;

	int numero_de_especie, numero_especie_evolucionada;
	char nombre_pokemon[MAX_NOMBRE], descripcion_especie[MAX_DESCRIPCION], nombre_nueva_especie[MAX_NOMBRE];


	int leidos = fscanf(archivo, "%i;%[^;];%i;%[^;];%[^\n]\n", &numero_de_especie, nombre_pokemon, &numero_especie_evolucionada,
		nombre_nueva_especie, descripcion_especie);
	if(leidos!=5){ 
		fclose(archivo);
		return ERROR;
	}

	especie_pokemon_t* nueva_especie_evolucionada;	
	bool encontro = false;
	especie_pokemon_t especie;
	while(leidos == 5){
		especie.numero = numero_de_especie;
		especie_pokemon_t* especie_buscada = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, &especie);
		if(!especie_buscada){
			printf("No se encontro la especie\n");
			fclose(archivo);
			return ERROR;
		}
		printf("La especie es: %s\n", especie_buscada->nombre);
		
		lista_t* lista_pokemones = especie_buscada->pokemones;

		for(size_t i=0; i< lista_elementos(lista_pokemones) && !encontro; i++){
			particular_pokemon_t* pokemon_buscado = (particular_pokemon_t*)lista_elemento_en_posicion(lista_pokemones, i);
			if(strcmp(pokemon_buscado->nombre, nombre_pokemon)==0 && pokemon_buscado->capturado){
				
				especie_pokemon_t especie_evolucionada;
				especie_evolucionada.numero = numero_especie_evolucionada;
				nueva_especie_evolucionada=(especie_pokemon_t*)arbol_buscar(pokedex->pokemones, &especie_evolucionada);
				if(!nueva_especie_evolucionada){
					nueva_especie_evolucionada=nueva_especie(numero_especie_evolucionada,nombre_nueva_especie,descripcion_especie);	
					arbol_insertar(pokedex->pokemones, nueva_especie_evolucionada);
					printf("\nSe agrego una nueve especie\n");
				}
				particular_pokemon_t* pokemon_evolucionado = crear_pokemon(pokemon_buscado->nombre, pokemon_buscado->nivel, 'S');
				lista_insertar(nueva_especie_evolucionada->pokemones, pokemon_evolucionado);
				free(pokemon_buscado);
				lista_borrar_de_posicion(lista_pokemones, i);
				encontro = true;
				printf("Se evoluciono el pokemon\n");
			}

		}
		if(!encontro){
			printf("No se encontro el pokemon buscado dentro de la especie\n");
			fclose(archivo);
 			return ERROR;
		}
		encontro = false;
 		leidos = fscanf(archivo, "%i;%[^;];%i;%[^;];%[^\n]\n", &numero_de_especie, nombre_pokemon, &numero_especie_evolucionada,
			nombre_nueva_especie, descripcion_especie);	
	}
 	fclose(archivo);
	return 0;
}

/*
 * Procedimiento que muestra los últimos Pokémon que fueron
 * capturados. Dichos Pokémon deberán ser listados del mas reciente
 * al menos reciente.
 *
 * Luego de invocar esta función, la pila de ultimos capturados queda
 * vacía.
 */
void pokedex_ultimos_capturados(pokedex_t* pokedex){
	if(!pokedex || lista_vacia(pokedex->ultimos_capturados)){
		printf("\nNo hay pokedex o pokemones capturados\n");
		return;
	} 
	printf("\nA continuacion, los pokemones capturados\n");
	while(!lista_vacia(pokedex->ultimos_capturados)){
		particular_pokemon_t* pokemon_desapilado = (particular_pokemon_t*)lista_tope(pokedex->ultimos_capturados);
		printf("Pokemon nombre: %s || Su nivel: %i\n", pokemon_desapilado->nombre,pokemon_desapilado->nivel);
		free(pokemon_desapilado);
		lista_desapilar(pokedex->ultimos_capturados);
	}
}

/*
 * Procedimiento que muestra los últimos Pokémon que fueron
 * vistos. Dichos Pokémon deberán ser listados del mas antiguo al
 * mas reciente.
 *
 * Luego de invocar esta función, la cola de últimos vistos queda
 * vacía.
 */
void pokedex_ultimos_vistos(pokedex_t* pokedex){
	if(!pokedex || lista_vacia(pokedex->ultimos_vistos)){
		printf("\nNo hay pokedex o pokemones vistos\n");
		return;
	}
	printf("\nA continuacion: Los pokemones vistos, se mostraran del primero al ultimo visto.\n"); 
	while(!lista_vacia(pokedex->ultimos_vistos)){
		particular_pokemon_t* pokemon_antiguo = (particular_pokemon_t*)lista_primero(pokedex->ultimos_vistos);
		printf("Pokemon nombre: %s || Su nivel: %i || ",pokemon_antiguo->nombre, pokemon_antiguo->nivel);
		printf("Fue capturado: %s\n", pokemon_antiguo->capturado==true?"SI":"NO");
		free(pokemon_antiguo);
		lista_desencolar(pokedex->ultimos_vistos);
	}
	return;
}


/*
 * Muestra la información del Pokémon recibido por parametros (especie
 * y nombre).  En caso de no existir el Pokémon o la especie, se
 * deberá mostrar un mensaje informando que es una especie o Pokémon
 * desconocidos..
 *
 * En caso de recibir un nombre vacío, se deben listar a todos los
 * Pokémon de esa especie.
 *
 */
void pokedex_informacion(pokedex_t* pokedex, int numero_pokemon, char nombre_pokemon[MAX_NOMBRE]){
	if(!pokedex){
		printf("\nLa pokedex no esta creada o no existe\n");
		return;
	}
	especie_pokemon_t* especie_buscada = (especie_pokemon_t*)arbol_buscar(pokedex->pokemones, &numero_pokemon);
	if(!especie_buscada){
		printf("\nNo se encuentra la especie\n");
		return;
	}
	printf("numero de especie buscada: %i\n", especie_buscada->numero);
	especie_pokemon_t* pokemones[100];
	int cantidad = arbol_recorrido_inorden(pokedex->pokemones, (void**)pokemones, 100);
	bool encontro=false;
	for(int i=0; i<cantidad && !encontro; i++){
		if(pokemones[i]->numero == numero_pokemon){
			printf("\nSe encontró la especie\n");	
			lista_t* lista_pokemones = pokemones[i]->pokemones; 	
			for(size_t j=0;j < lista_elementos(lista_pokemones); j++){
				particular_pokemon_t* pokemon =(particular_pokemon_t*)lista_elemento_en_posicion(lista_pokemones,j);
				if(!nombre_pokemon){
					printf("\nEl nombre del pokemon: %s\n", pokemon->nombre);
					printf("Su nivel es: %i\n",pokemon->nivel);
					printf("Está capturado: %s\n", pokemon->capturado==true?"SI":"NO");	
				}
				else if(strcmp(nombre_pokemon, pokemon->nombre)==0){
					printf("\nEl nombre del pokemon: %s\n", pokemon->nombre);
					printf("Su nivel es: %i\n",pokemon->nivel);
					printf("Está capturado: %s\n", pokemon->capturado==true?"SI":"NO");
					encontro=true;
				}
				
			}
		}			
	}
	return;
}

/*
 * Destruye la estructura de la Pokedex, liberando la memoria que fue
 * reservada para la misma.
 */
void pokedex_destruir(pokedex_t* pokedex){
	if(!pokedex){
		printf("\nNo existia la pokedex\n");
		return;
	} 
	arbol_destruir(pokedex->pokemones);
	printf("Eliminando de las listas capturados y vistos:\n");
	pokedex_ultimos_capturados(pokedex);
	pokedex_ultimos_vistos(pokedex);
	free(pokedex->ultimos_vistos);
	free(pokedex->ultimos_capturados);
	free(pokedex);
	printf("\nSe eliminó el pokedex\n");	
	return;
}

/*
 * Guarda la información de la pokedex a archivo pokedex.txt. La
 * información debe ser guardada en el formato especificado en el
 * enunciado del TP y adicionalmente deben guardarse de tal forma que
 * al prender la pokedex, la forma de árbol de especies tenga la misma
 * forma del árbol original.
 *
 * Devuelve 0 en caso de éxito o -1 si hubo algún error.
 */
int pokedex_apagar(pokedex_t* pokedex){

	FILE* archivo_pokedex;
	archivo_pokedex = fopen("guardar_pokedex.txt", "w");
	if(!archivo_pokedex) return ERROR;
	fprintf(archivo_pokedex, "%s\n", pokedex->nombre_entrenador);

	char nombre[MAX_NOMBRE], descripcion[MAX_DESCRIPCION];
	int numero;
	char nombre_pokemon[MAX_NOMBRE];
	int nivel_pokemon;
	char pokemon_capturado; 

	especie_pokemon_t* especie_pokemones[500]; 

	int cantidad_especies = arbol_recorrido_inorden(pokedex->pokemones, (void**)especie_pokemones, 500);

	for(int i=0 ; i < cantidad_especies; i++){
		numero = especie_pokemones[i]->numero;
		strcpy(nombre,especie_pokemones[i]->nombre);
		strcpy(descripcion,especie_pokemones[i]->descripcion);
		fprintf(archivo_pokedex, "%c;%s;%i;%s\n", 'E', nombre, numero, descripcion);

		lista_iterador_t* lista_iterador_pokemones = lista_iterador_crear(especie_pokemones[i]->pokemones);
		while(lista_iterador_tiene_siguiente(lista_iterador_pokemones)){
			particular_pokemon_t* pokemon = (particular_pokemon_t*)lista_iterador_siguiente(lista_iterador_pokemones);
			if(pokemon->capturado){
				pokemon_capturado = 'S';
			}
			else{
				pokemon_capturado = 'N';
			}
			strcpy(nombre_pokemon, pokemon->nombre);
			nivel_pokemon = pokemon->nivel;
			fprintf(archivo_pokedex, "%c;%s;%i;%c\n", 'P', nombre_pokemon, nivel_pokemon, pokemon_capturado);
		}
		lista_iterador_destruir(lista_iterador_pokemones);
	}
	fclose(archivo_pokedex);
	return 0;	
}

/*
 * Carga la información de la pokedex del archivo pokedex.txt.
 *
 * No se piden validaciones sobre el formato del archivo ya que
 * suponemos que fueron guardados correctamente mediante la función
 * pokedex_apagar.
 *
 * Devuelve la pokedex creada desde el archivo o NULL en caso de error.
 */
pokedex_t* pokedex_prender(){

	FILE* archivo = fopen("pokedex.txt", "r");
	if(!archivo){
		return NULL;
	}

	char nombre_del_entrenador[MAX_NOMBRE];
	int cant_nombre = fscanf(archivo , "%s\n", nombre_del_entrenador);
	if(cant_nombre!=1){
		fclose(archivo);
		return NULL;
	}
	printf("Nombre del entrenador: %s\n", nombre_del_entrenador);

	pokedex_t* pokedex = pokedex_crear(nombre_del_entrenador);

	char primera_letra;
	int numero_de_pokemon;
	char especie_pokemon[100];
	char descripcion_especie[500];
	
	int leidos = fscanf(archivo, "%c;%[^;];%i;%[^\n]\n", &primera_letra, especie_pokemon, &numero_de_pokemon, descripcion_especie);
	if(leidos!=4){ 
		fclose(archivo);
		return NULL;
	}

	especie_pokemon_t* especie_actual;
	particular_pokemon_t* nuevo_pokemon;
	while(leidos == 4){
		printf("despues de volver a leer, primera letra: %c\n", primera_letra);
		printf("LA ESPECIE: %s\n", especie_pokemon );
		printf("el numero: %i\n", numero_de_pokemon);
		printf("la descripcion: %s\n",descripcion_especie );

		if(primera_letra == 'E'){
			// Inserto un especie pokemon;
			especie_pokemon_t* especie = nueva_especie(numero_de_pokemon, especie_pokemon, descripcion_especie);
			if(!especie){
				return NULL;
			}
			arbol_insertar(pokedex->pokemones, especie);
			especie_actual = especie;
		}
		
		if(primera_letra == 'P'){
				nuevo_pokemon = crear_pokemon_pokedex(especie_pokemon, numero_de_pokemon, descripcion_especie);
				printf("EL POKEMON AGREGADO PODER %i\n", numero_de_pokemon);
				if(!nuevo_pokemon) return NULL;
				lista_insertar(especie_actual->pokemones, nuevo_pokemon);
				if(nuevo_pokemon->capturado){
					particular_pokemon_t* nuevo_pokemon_capturado = crear_pokemon_pokedex(especie_pokemon, numero_de_pokemon, descripcion_especie);
					if(!nuevo_pokemon_capturado) return NULL;
					lista_apilar(pokedex->ultimos_capturados, nuevo_pokemon_capturado);
				}
				particular_pokemon_t* nuevo_pokemon_visto = crear_pokemon_pokedex(especie_pokemon, numero_de_pokemon, descripcion_especie);
				if(!nuevo_pokemon_visto) return NULL;
				lista_encolar(pokedex->ultimos_vistos, nuevo_pokemon_visto);
		}

		leidos = fscanf(archivo, "%c;%[^;];%i;%[^\n]\n", &primera_letra, especie_pokemon, 
			&numero_de_pokemon, descripcion_especie);
	}
	fclose(archivo);
	return pokedex;
}



//compilar: gcc *.c -Wall -Werror -Wconversion -std=c99 -o pokedex
//valgrind: valgrind --leak-check=full --track-origins=yes --show-reachable=yes ./pokedex
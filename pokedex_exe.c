#include "pokedex.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

void imprimir_comandos(){
	printf("\nBienvendo al juego de la pokedex.\nPor favor ingrese una opcion:\n");
	printf("Iniciar pokedex (tecla I)\n");
	printf("Guardar pokedex (tecla G)\n");
	printf("Salir del programa (tecla S)\n");
	printf("Ayuda (tecla H)\n");
	printf("Avistar Pokemon (tecla A)\n");
	printf("Evolucionar Pokemon (tecla E)\n");
	printf("Capturas recientes (tecla C)\n");
	printf("Vistas recientes (tecla V)\n");
	printf("Informacion especie (tecla M)\n");
	printf("Informacion pokemon (tecla P)\n\n");
}

void comandos_si_no_hay_pokedex(){
	printf("\nIniciar pokedex (tecla I)\n");
	printf("Salir del programa (tecla S)\n");
	printf("Ayuda (tecla H)\n");
}

bool validar_comando(char comando[20]){
	if(comando[0] == 'I' || comando[0] =='G' || comando[0] == 'S' || comando[0] == 'H' || comando[0] == 'A' || comando[0] =='E'
		|| comando[0] == 'C' || comando[0] == 'V' || comando[0] == 'M' || comando[0] == 'P'){
		return true;
	}
	return false;
}

char pedir_comando(){
	printf("\nPor favor ingrese un comando...\n");
	char comando[20];
	fgets(comando, 19, stdin);

	//comando = toupper(comando);

	bool comando_es_valido=false;
	comando_es_valido = validar_comando(comando);
	while(!comando_es_valido){
		printf("\nPor favor ingrese un comando valido \n");
		fgets(comando, 19, stdin);
		comando_es_valido = validar_comando(comando);
	}
	return *comando;
}

pokedex_t* iniciar_pokedex(){
	pokedex_t* pokedex = pokedex_prender();
	return pokedex;
}

void mostrar_comandos_al_haber_pokedex(){
	printf("\nIngrese un comando para seguir:\n");
	printf("Guardar Pokedex (tecla G)\n");
	printf("Salir del programa (Tecla S)\n");
	printf("Ayuda (tecla H)\n");
	printf("Avistar pokemon (tecla A)\n");
	printf("Evolucionar (tecla E)\n");
	printf("Capturas recientes (tecla C)\n");
	printf("Pokemones vistos recientemente (tecla V)\n");
	printf("Informacion de especie (tecla M)\n");
	printf("Informacion pokemon (tecla P)\n");
}


int main(){

	imprimir_comandos();
	pokedex_t* pokedex = NULL;
	
	//pedir_comando();

	while(!pokedex){
		comandos_si_no_hay_pokedex();
		char comando = pedir_comando();
		if(comando =='I') {
			printf("Ha iniciado una pokedex\n");
			pokedex = iniciar_pokedex();
		}
		if(comando == 'S') {
			pokedex_destruir(pokedex);
			return 0;
		}	
		if(comando == 'H') comandos_si_no_hay_pokedex();
	}
	mostrar_comandos_al_haber_pokedex();
	char comando = pedir_comando();
	
	while(pokedex){
		if(comando == 'A'){
			pokedex_avistar(pokedex, "avistamientos.txt");
		}
		if(comando == 'S'){
			pokedex_destruir(pokedex);
			return 0;
		}
		if(comando == 'P'){
			printf("Por favor ingrese la especie buscada:\n");
			int numero_especie;
			scanf("%d", &numero_especie);

			char nombre_del_pokemon[100];
			printf("Por favor ingrese el nombre del pokemon\n");				
			scanf("%s", nombre_del_pokemon);
			pokedex_informacion(pokedex, numero_especie, nombre_del_pokemon);

		}
		if(comando =='M'){
			printf("Por favor ingrese la especie buscada:\n");
			int numero_especie;
			scanf("%d", &numero_especie);
			pokedex_informacion(pokedex, numero_especie, NULL);
		}
		if(comando == 'V'){
			pokedex_ultimos_vistos(pokedex);
		}
		if(comando == 'C'){
			pokedex_ultimos_capturados(pokedex);
		}
		if(comando == 'G'){
			pokedex_apagar(pokedex);
		}
		if(comando == 'E'){
			pokedex_evolucionar(pokedex, "evoluciones.txt");
		}
		mostrar_comandos_al_haber_pokedex();
		comando = pedir_comando();
	}

		
	
	pokedex_destruir(pokedex);
	return 0;	

}


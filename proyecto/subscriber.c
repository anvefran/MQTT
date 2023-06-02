#include <pthread.h>
#include <stdio.h>
#include <limits.h>
#include "common.h"

void print_topicos(void);
void print_topicos(void){
        printf("Escoja la metrica que desea recibir, cada subnivel incluirlo con un /\n");
	printf("Metricas:\n");
	printf("(Host#)\t\tRecibir toda la informacion del sistema\n");
        printf("(CPU)\t\tInformacion de la CPU\n");
        printf("(RAM)\t\tInformacion de la memoria RAM\n");
	printf("(SWA)\t\tInformacion de la memoria SWAP\n");
	printf("(/+/)\t\tWildcard de un nivel\n");
	printf("(/#)\t\tWildcard de varios niveles\n");
	printf("\n");
}

int main(int argc, char **argv)
{
	//Socket
	int clientfd;
	//Direcciones y puertos
	char *hostname, *port;

	//Lectura desde consola
	char *linea_consola;
	char read_buffer[MAXLINE + 1] = {0};
	size_t max = MAXLINE;
	ssize_t n, l = 0;

	if(argc != 3){
		fprintf(stderr, "uso: %s <hostname> <puerto>\n", argv[0]);
		return -1;
	}else{
		hostname = argv[1];
		port = argv[2];
	}

	//Valida el puerto
	int port_n = atoi(port);
	if(port_n <= 0 || port_n > USHRT_MAX){
		fprintf(stderr, "Puerto: %s invalido. Ingrese un número entre 1 y %d.\n", port, USHRT_MAX);
		return -1;
	}

	//Se conecta al servidor retornando un socket conectado
	clientfd = open_clientfd(hostname, port);

	if(clientfd < 0)
		connection_error(clientfd);

	printf("Conectado exitosamente a %s en el puerto %s.\n", hostname, port);
	print_topicos();

	linea_consola = (char *) calloc(1, MAXLINE);
	printf("ingrese el host y la metrica divididos por / >>");
	l = getline(&linea_consola, &max, stdin); //lee desde consola, linea_consola es el host y la metrica
	n = write(clientfd, linea_consola, l); //Envia al servidor

	while(1){
		n=read(clientfd, read_buffer,MAXLINE);
		if (n>0){
			printf("%s\n", read_buffer);
		}
		memset(read_buffer,0,MAXLINE + 1); //Encerar el buffer
	}

	printf("Desconectando...\n");
	free(linea_consola);
	close(clientfd);
}

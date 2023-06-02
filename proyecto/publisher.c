#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "common.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio_ext.h>
#include <limits.h>

#define STDIN 0
#define STDOUT 1


int main(int argc, char **argv){
	char *topico;
	ssize_t n, l, size = 0;
	size_t max = 1024;//MAXLINE;
	int status;
	pid_t pid;

	//Socket
        int clientfd;
        //Direcciones y puertos
        char *hostname, *port;
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


	topico = (char *) calloc(1,1024);
	printf("Ingrese el Nombre de host: ");
	size = getline(&topico, &max, stdin);
	while (1){
		char *valorfinal;
		valorfinal = (char *) calloc(1,1024);
		int fd[2];
		pipe(fd);

		if ((pid = fork()) == 0){
			close(STDOUT);
			dup(fd[1]); //redirige stdout a la pipe
			close(fd[0]); //hijo no lee del pipe
			close(fd[1]);
			char *comando[] = {"top","-bn2"};

			if (execvp("top",comando) == -1){
				printf("%s: Command not found. \n", comando[0]);
				exit(0);
			}
			exit(0);

		} else {
			while ((pid = waitpid(-1, &status,0))>0){
                               //
                	}
			close(STDIN);
			dup(fd[0]);
			close(fd[0]);
			close(fd[1]);

			char *linea;
			size_t maximo = 1024;
			linea = (char *) calloc(1,1024);
			char *segmento;
			int cont = 0;

			//SACA INFO GENERAL DE CPU
			while((getline(&linea, &maximo, stdin))!=-1){
				segmento = strtok(linea, " ");
				if (strcmp(&segmento[0],"%Cpu(s):")==0){
					cont++;
					if (cont==2){
						while (segmento!=NULL || strcmp(segmento, "(null)")!=0){
							if (segmento!=NULL && strcmp(segmento, "st\n")!=0){
								strcat(valorfinal, segmento);
							}
							if (strcmp(segmento, "st\n")==0){
								strcat(valorfinal, "st");
								break;
							}
							segmento = strtok(NULL, " ");
						}
					}
				}
				if(cont==2){
					break;
				}
			}
			
			strcat(valorfinal, ";");
			//printf("Probando valor final %s\n", valorfinal);
			//SACA INFO DE RAM
			while((getline(&linea, &maximo, stdin))!=-1){
                                segmento = strtok(linea, " ");
                                if (strcmp(&segmento[0],"MiB")==0){
                                        cont++;
                                        if (cont==3){
                                                while (segmento!=NULL || strcmp(segmento, "(null)")!=0){
                                                        if (segmento!=NULL && strcmp(segmento, "buff/cache\n")!=0){
                                                                strcat(valorfinal, segmento);
                                                        }
                                                        if (strcmp(segmento, "buff/cache\n")==0){
                                                                strcat(valorfinal, "buff/cache");
                                                                break;
                                                        }
                                                        segmento = strtok(NULL, " ");
                                                }
                                        }
                                }
                                if(cont==3){
                                        break;
                                }
                        }
			//printf("Probando valor final2 %s\n", valorfinal);
			strcat(valorfinal, ";");
			
			//SACA INDO DE SWAP
			while((getline(&linea, &maximo, stdin))!=-1){
                                segmento = strtok(linea, " ");
                                if (strcmp(&segmento[0],"MiB")==0){
                                        cont++;
                                        if (cont==4){
                                                while (segmento!=NULL || strcmp(segmento, "(null)")!=0){
							if (segmento!=NULL){
                                                                strcat(valorfinal, segmento);
                                                        }
                                                        if (strcmp(segmento, "Mem")==0){
                                                                //strcat(valorfinal, "Mem");
                                                                break;
                                                        }
                                                        segmento = strtok(NULL, " ");
                                                }
                                        }
                                }
                                if(cont==4){
                                        break;
                                }
                        }
			//printf("prueba final3 %s\n", valorfinal);
			free(linea);

		}
		printf("%s\n", valorfinal);
		n = write(clientfd, valorfinal, 1024); //Envia al servidor
		free(valorfinal);
		//while(1){
                //sleep(2);
                //}

	}
	free(topico);
	close(clientfd);
	return 0;
}



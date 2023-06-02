#include <getopt.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>
#include <syslog.h>
#include <fcntl.h>
#include <semaphore.h>
#include "common.h"

int bufferHost1[10]= {0};
int bufferCPU[10] = {0}; //se guardan los descriptores de los clientes
int bufferRAM[10] = {0};
int bufferSWA[10] = {0};
int bufferCPUid[10] = {0};
int bufferRAMused[10] = {0};
int bufferSWAused[10] = {0};

sem_t sem;
sem_t sem2;
sem_t sem3;
sem_t sem4;
sem_t sem5;
sem_t sem6;


void atender_cliente(int connfd,char buf[]);

/**
 * Recibe SIGINT, termina ejecución.
 */
void salir(int signal){
	exit(0);
}


void *thread(void* vargp);


int main(int argc, char **argv)
{

	pthread_t tid;
	sem_init(&sem,0,1);
	sem_init(&sem2,0,1);
	sem_init(&sem3,0,1);
	sem_init(&sem4,0,1);
        sem_init(&sem5,0,1);
        sem_init(&sem6,0,1);
	//Sockets
	int listenfd, *connfd;
	unsigned int clientlen;
	//Direcciones y puertos
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	char *haddrp, *port;

	if(argv == NULL || argc!=2){
		fprintf(stderr, "uso: %s <puerto>\n", argv[0]);
		return 1;
	}
	port =argv[1];

	//Valida el puerto
	int port_n = atoi(port);
	if(port_n <= 0 || port_n > USHRT_MAX){
		fprintf(stderr, "Puerto: %s invalido. Ingrese un número entre 1 y %d.\n", port, USHRT_MAX);
		return 1;
	}
	printf("Waiting for connections...\n");

	//Registra funcion para señal SIGINT (Ctrl-C)
	signal(SIGINT, salir);

	//Abre un socket de escucha en port
	listenfd = open_listenfd(port);

	if(listenfd < 0)
		connection_error(listenfd);

	printf("server escuchando en puerto %s...\n", port);

	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = malloc(sizeof(int));
		*connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
		/* Determine the domain name and IP address of the client */
		hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
					sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		if (hp)
			printf("\n>> Server connected to %s (%s)\n\n", hp->h_name, haddrp);
		else
			printf("server connected to %s\n", haddrp);
		//n = read(connfd, buf, MAXLINE);
        	FILE* f = fopen("registro.log", "a");
        	if (!f) {
                 perror("fopen");
                 exit(EXIT_FAILURE);
        	}
        	char *str1=(char *) calloc(1,1024);
        	sprintf(str1, "%s Se ha conectado al broker\n",haddrp);
        	fwrite(str1, 1,strlen(str1), f);
        	fclose(f);
		pthread_create(&tid, NULL, thread, (void*)connfd);
	}

}


void *thread(void* vargp){
	int n;
        int connfd = *((int *)vargp);
	char buf[MAXLINE] = {0};
	n = read(connfd, buf, MAXLINE);

	if (strcmp(buf,"host1\n")==0 ||strcmp(buf,"host1/#\n")==0){
		int i;
		sem_wait(&sem);
                for(i=0;i<MAXLINE;i++){
			if (!(bufferHost1[i]>0)){
                                bufferHost1[i] = connfd;
                                break;
                        }
                }
		sem_post(&sem);
	}else if(strcmp(buf,"host1/CPU\n")==0 || strcmp(buf,"host1/CPU/#\n")==0){
		int i;
		for(i=0;i<MAXLINE;i++){
                        if (!(bufferCPU[i]>0)){
                                bufferCPU[i] = connfd;
                                break;
                        }
                }
	} else if (strcmp(buf,"host1/RAM\n")==0 ||strcmp(buf,"host1/RAM/#\n")==0){
                int i;
		sem_wait(&sem2);
                for(i=0;i<MAXLINE;i++){
                        if (!(bufferRAM[i]>0)){
				bufferRAM[i] = connfd;
                                break;
                        }
                }
		sem_post(&sem2);
        }else if (strcmp(buf,"host1/SWA\n")==0||strcmp(buf,"host1/SWA/#\n")==0){
                int i;
		sem_wait(&sem3);
                for(i=0;i<MAXLINE;i++){
			if (!(bufferSWA[i]>0)){
                                bufferSWA[i] = connfd;
                                break;
                        }
                }
		sem_post(&sem3);
        }else if (strcmp(buf,"host1/CPU/idle\n")==0||strcmp(buf,"host1/+/idle\n")==0){
                int i;
                sem_wait(&sem4);
                for(i=0;i<MAXLINE;i++){
                        if (!(bufferCPUid[i]>0)){
                                bufferCPUid[i] = connfd;
                                break;
                        }
                }
                sem_post(&sem4);
        }
	if (strcmp(buf,"host1/RAM/free\n")==0||strcmp(buf,"host1/+/free\n")==0){
                int i;
		sem_wait(&sem5);
                for(i=0;i<MAXLINE;i++){
                        if (!(bufferRAMused[i]>0)){
                                bufferRAMused[i] = connfd;
                                break;
                        }
                }
		sem_post(&sem5);
        }
	if (strcmp(buf,"host1/SWA/free\n")==0||strcmp(buf,"host1/+/free\n")==0){
                int i;
		sem_wait(&sem6);
                for(i=0;i<MAXLINE;i++){
                        if (!(bufferSWAused[i]>0)){
                                bufferSWAused[i] = connfd;
                                break;
                        }
                }
		sem_post(&sem6);
        }
        pthread_detach(pthread_self());
        free(vargp);
        atender_cliente(connfd,buf);
	close(connfd);
        return NULL;
}


void atender_cliente(int connfd, char buf[])
{
	int n;

	if(strcmp(buf,"host1/CPU\n")==0 ||strcmp(buf,"host1/RAM\n")==0 || strcmp(buf,"host1/SWA\n")==0||strcmp(buf,"host1/SWA/free\n")==0||strcmp(buf,"host1/+/free\n")==0 ||strcmp(buf,"host1/RAM/free\n")==0||strcmp(buf,"host1/CPU/idle\n")==0||strcmp(buf,"host1/+/idle\n")==0||strcmp(buf,"host1/SWA/#\n")==0||strcmp(buf,"host1/RAM/#\n")==0 ||strcmp(buf,"host1\n")==0 ||strcmp(buf,"host1/#\n")==0){
		//n = read(connfd, buf, MAXLINE);
        	FILE* f = fopen("registro.log", "a");
        	if (!f) {
                 perror("fopen");
                 exit(EXIT_FAILURE);
        	}
        	char *str1=(char *) calloc(1,1024);
        	sprintf(str1, "El usuario con connfd %d ha solicitado: %s\n",connfd,buf);
        	fwrite(str1, 1,strlen(str1), f);
        	fclose(f);
		while(1){
			sleep(1);
		}

	}else{
		while(1){
			char *nombre= (char *) calloc(1,100);
			nombre= buf;
			n = read(connfd, buf, MAXLINE);
			if (n>0){
				FILE* f = fopen("registro.log", "a");
                        	if (!f) {
                                	perror("fopen");
                                	exit(EXIT_FAILURE);
                        	}
                        	char *str1=(char *) calloc(1,1024);
                        	sprintf(str1, "%s ha enviado %s\n",nombre,buf);
                        	fwrite(str1, 1,strlen(str1), f);
                        	fclose(f);
                                int i;
                                for (i=0;i<10;i++){
					if(bufferHost1[i]>0){
						//printf("%s\n",buf);
						n = write(bufferHost1[i],buf, strlen(buf) + 1);
					}
                                        if(bufferCPU[i]>0){
						char *token= strtok(buf,";");
                                                n = write(bufferCPU[i],token, strlen(token) + 1);
                                        } 
					if(bufferRAM[i]>0){
                                                char *token= strtok(buf,";");
						token= strtok(NULL,";");
                                                 //printf("%s\n", token);
						n = write(bufferRAM[i],token, strlen(token) + 1);
                                        } 
					if(bufferSWA[i]>0){
                                                char *token= strtok(buf,";");
                                                token= strtok(NULL,";");
						token=strtok(NULL,";");
                                                n = write(bufferSWA[i],token, strlen(token) + 1);
                                        } 
					char stringfree[1024] = "";
					strcpy(stringfree,buf);
					if(bufferRAMused[i]>0){
						if(i==0){
                                                char *ram= strtok(buf,";");
                                                //printf("ram1 %s\n",ram);
						ram= strtok(NULL,";");
						//printf("ram2 %s\n",ram);
						ram = strtok(ram, ",");
						ram=strtok(NULL,",");
						//ram=strtok(NULL,",");
						//printf("ram %s\n",ram);
						char *string=(char *) calloc(1,1024);
                                		sprintf(string, "RAM free: %s\n",ram);
						//printf("string %s\n",string);
						int i;
                                                for(i=0;i<4;i++){
                                                if(bufferRAMused[i]>0){
						n = write(bufferRAMused[i],string, strlen(string) + 1);
                                       		}
						}
						}
					} 
					if(bufferSWAused[i]>0){
						if(i==0){
						if(strcmp(stringfree, "")!=0){
						strcpy(buf,stringfree);
                                                }
						char *token= strtok(buf,";");
                                                token= strtok(NULL,";");
                                                token=strtok(NULL,";");
						token= strtok(token,",");
                                                token = strtok(NULL, ",");
                                                //token=strtok(NULL,",");
                                                //swa=strtok(NULL,",");
						char *string=(char *) calloc(1,1024);
                                                sprintf(string, "SWAP free: %s\n",token);
						int i;
						for(i=0;i<4;i++){
						if(bufferSWAused[i]>0){
                                                n = write(bufferSWAused[i],string, strlen(string) + 1);
						}
						}
						}
                                        } 
					if(bufferCPUid[i]>0){
                                                if(i==0){
						char *cp= strtok(buf,";");
						//printf("probando buf %s\n en iter %d", buf, i);
     
						cp= strtok(cp, ",");
						//printf("probando buf %s\n", buf);
                                                cp=strtok(NULL,",");
                                                cp=strtok(NULL,",");
						cp=strtok(NULL,",");
						int i;
						for(i=0;i<4;i++){
						if (bufferCPUid[i]>0){
						n = write(bufferCPUid[i],cp, strlen(cp) + 1);
						}
						}
						}
                                                //n = write(bufferCPUid[i],cp, strlen(cp) + 1);
                                        } 
                                }
                        }
                        memset(buf, 0, MAXLINE); //Encera el buffer
                }
	}

}

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include<stdbool.h>  
#include <pthread.h>

int main(int argc, char *argv[]) {
	
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	if((sock_listen=socket(AF_INET, SOCK_STREAM, 0))<0)
		printf("Error creando el socket\n");
	
	memset(&serv_adr,0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(9070);
	if(bind(sock_listen, (struct sockaddr *)&serv_adr, sizeof(serv_adr))<0)
		printf("Error al bind \n");
	
	if(listen(sock_listen, 3)<0)
		printf("Error en el listen \n");
	
	int contador;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	int terminar = 0;
	while(terminar == 0) //Bucle de atencion al cliente
	{
		printf("Escuchando \n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf("He recibido conexion \n");
		
		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibido \n");
		
		peticion[ret]='\0';
		
		printf("Peticion %s \n", peticion);
		
		char *p = strtok(peticion, "/");
		int codigo = atoi(p);
		char nombre[20];
		//Escribimos la peticion en la consola
		if (codigo != 0) {
			printf("La peticion es: %s\n", peticion);
			p = strtok(NULL, "/");
			strcpy(nombre, p);
			printf("Codigo: %d, Nombre: %s\n", codigo, nombre);
		}
		
		if (codigo == 0)
			terminar = 1;
		else if (codigo == 1) //piden la longitud del nombre
			sprintf(respuesta, "%d", strlen(nombre));
		else if (codigo == 2)
		{
			// quieren saber si el nombre es bonito
			if ((nombre[0] == 'M') || (nombre[0] == 'S'))
				strcpy(respuesta, "SI");
			else
				strcpy(respuesta, "NO");
		}
		else if (codigo == 3)//decir si es alto 
		{
			p = strtok(NULL, "/");
			float altura = atof(p);
			if (altura > 1.70)
				sprintf(respuesta, "%s: Eres alto", nombre);
			else
				sprintf(respuesta, "%s: No eres alto", nombre);
		}
		else if (codigo==4)//palindromo
		{
			bool palindromo=true;
			int longitud_nombre = strlen(nombre);
			for(int k=0;k<longitud_nombre;k++)
			{
				if(nombre[k]!=nombre[longitud_nombre-k-1])
				{
					palindromo=false;
				}
			}
			if(palindromo)
				sprintf(respuesta,"Tu nombre, %s, si es un palindromo",nombre);
			else
				sprintf(respuesta,"Tu nombre, %s, no es un palindromo",nombre);					
		}
		else if (codigo==5)//nombre en maysuculas
		{
			for (int j = 0; nombre[j] != '\0'; ++j){
				nombre[j] = toupper(nombre[j]);
			}
			sprintf(respuesta, "%s", nombre);
		}
		else if (codigo == 6)
			sprintf(respuesta, "%d", contador);
		
		if (codigo != 0) {
			// Enviamos la respuesta
			write(sock_conn, respuesta, strlen(respuesta));
		}
		if ((codigo == 1) || (codigo == 2) || (codigo == 3) || (codigo == 4) || (codigo == 5))
		{
			pthread_mutex_lock(&mutex); //No me interrumpas ahora
			contador = contador + 1;
			pthread_mutex_unlock(&mutex); //ya puedes interrumpirme
		}
		
	}
	// Se acabo el servicio para este cliente
	close(sock_conn);
}



	
	

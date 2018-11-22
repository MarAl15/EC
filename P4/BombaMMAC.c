#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <string.h>	// para strncmp()/strlen()
#include <sys/time.h>	// para gettimeofday(), struct timeval
#include <sys/cdefs.h> 
#include <errno.h>
#include <unistd.h>

void oneko(){
	int fd[2];
	pid_t pid;
	char num1[50], num2[50];
	char buffer[100];
	int tamanio, estado;

	// Llamadas al sistema para crear dos pipes, cauces sin nombre
	// para comunicar cada esclavo con su maestro.
	pipe(fd);

	if((pid = fork())<0){
		perror("\nError en fork");
		exit(-1);
	}else if(pid == 0){
		// Cerrar el descriptor de lectura de cauce, fd[0], en el proceso hijo.
		close(fd[0]);

		// Duplicar el descriptor de escritura en cauce, fd[1], en el descriptor
		// correspondiente a la salida estándar (stdout), cerrado
		// previamente en la misma operación.
		dup2(fd[1],STDOUT_FILENO);
		
		if(execlp("oneko", "oneko", NULL)<0); //Sólo se ejecuta si se tiene instalado 
		/*{
			perror("\nError en el execl1");
			exit(-1);
		}*/
	}else{
		// Cerrar el descriptor de escritura en cauce, fd[1], situado en el
		// proceso padre.
		close(fd[1]);

		dup2(fd[0],STDIN_FILENO);
		
		if(execlp("sort", "sort", NULL)<0){
			perror("\nError en el execl1");
			exit(-1);
		}
	}
}

void trensl(){
	printf("\n          (@@@@)");
	printf("\n       (     )");
	printf("\n ");
	printf("\n     (@@@)");
	printf("\n    ++       +------  ___________________ ___________________ ");
	printf("\n    ||       |+-+ |   | ___ ___ ___ ___ | | ___ ___ ___ ___ | ");
	printf("\n  /----------|| | |   | |_| |_| |_| |_| | | |_| |_| |_| |_| | ");
	printf("\n  + ========  +-+ |   |_________________| |_________________| ");
	printf("\n__|--O========O~ -+   |_________________| |_________________| ");
	printf("\n///  _/       _/        (o)         (o)     (o)         (o) \n");
}

void trensinvagon(){
	printf("\n          (@@@@)                      @");
	printf("\n       (     )               @");
	printf("\n                         @    ");
	printf("\n     (@@@)                  æ     w");
	printf("\n    ++       +------     @  @j  @  (o)       ___________________ ");
	printf("\n    ||       |+-+ |      // _ @ /!  @   @    | ___ ___ ___ ___ | ");
	printf("\n  /----------|| | |    d(o) @ l @  _@        | |_| |_| |_| |_| | ");
	printf("\n  + ========  +-+ | @  x //  @  @@ @  o  @   |_________________| ");
	printf("\n__|--O========O~ -+    @   _@  _   @  c   @  |_________________| ");
	printf("\n///  _/       _/     @  @   /@@  9  @          (o)         (o) \n");
}

void boom(){
	printf("****************************************************\n");
	printf("**** BOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOM!!! ****\n");
	printf("**** Estalló la bomba en uno de los vagones :´( ****\n");
	printf("****************************************************\n");
	trensinvagon();
	exit(-1);
}

void defused(){
	printf("·························································\n");
	printf("·················· Bomba desactivada ····················\n");
	printf("··· Salvastes a los pasajeros y al gato inflitrado ;) ···\n");
	printf("·························································\n");
	
	oneko();
	
	exit(0);
}

int main(){
#define SIZE 15
	char pass[SIZE];
	int  pasv;
#define TLIM 60
	
	struct timeval tv1,tv2;	// gettimeofday() secs-usecs

	gettimeofday(&tv1,NULL);
	
	trensl();
	
	printf("\nA Poirot le han llegado rumores de que en el Orient Express se va a producir un asesinato.");
	printf(" El criminal ha puesto una bomba. Poirot debe descifrarla y descubrir al culpable, y para ello necesita tu ayuda ;)\n\n ");
	

	printf("Introduce la contraseña: ");
	fgets(pass,SIZE,stdin);
	
	char *password;
	
	if(pass[0] >= 'D' || pass[0] <= 'N')
		password = "DivieRtete;)\n";
	else
		password = "Desactivala:O\n";
	
	pass[0] -= 10;
	pass[2] += 8;
	pass[3] -= 1;
	pass[4] += 4;
	
	if (strncmp(pass,password,strlen(password)))
	    boom();

	gettimeofday(&tv2,NULL);
	if (tv2.tv_sec - tv1.tv_sec > TLIM)
	    boom();

	int passcode = 253;
	
	printf("Introduce el código: ");
	scanf("%i",&pasv);
	
	pasv /= 2;
	
	if (pasv!=passcode) boom();

	gettimeofday(&tv1,NULL);
	if (tv1.tv_sec - tv2.tv_sec > TLIM)
	    boom();

	defused();
}

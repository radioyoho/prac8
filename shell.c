#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "vdisk.h"

#define MAXLEN 80
#define BUFFERSIZE 512

void locateend(char *cmd);
int executecmd(char *cmd);
int diru(char *arg1);
int isinvd(char *arg);
int copyuu(char *arg1,char *arg2);
int copyuv(char *arg1,char *arg2);
int copyvv(char *arg1,char *arg2);
int copyvu(char *arg1,char *arg2);
int dirv(char *arg1);
int diru(char *arg1);
int catv(char *arg1);
int catu(char *arg1);
int removev(char * arg1);
int removeu(char * arg1);

int main()
{
	update();
	char linea[MAXLEN];
	int result=1;
	while(result)
	{
		printf("vshell > ");
		fflush(stdout);
		read(0,linea,80);
		locateend(linea);
		result=executecmd(linea);
	} 
}

void locateend(char *linea)
{
	// Localiza el fin de la cadena para poner el fin
	int i=0;
	while(i<MAXLEN && linea[i]!='\n')
		i++;
	linea[i]='\0';
}

int executecmd(char *linea)
{
	char *cmd;
	char *arg1;
	char *arg2;
	char *search=" ";

	// Separa el comando y los dos posibles argumentos
	cmd=strtok(linea," ");
	arg1=strtok(NULL," ");
	arg2=strtok(NULL," ");

	// comando "exit"
	if(strcmp(cmd,"exit")==0)
		return(0);
	
	// comando dir
	if(strcmp(cmd,"dir")==0)
	{
		if(arg1 == NULL)
			dirv(arg1);
		else
			diru(arg1);
		
		return(1);
	}
	
	
	// comando "copy"
	if(strcmp(cmd,"copy")==0)
	{
		if(arg1==NULL || arg2==NULL)
		{
			fprintf(stderr,"Error en los argumentos\n");
			return(1);
		}
		
		if(!isinvd(arg1) && !isinvd(arg2))
			copyuu(arg1,arg2);
		
		else if(!isinvd(arg1) && isinvd(arg2))
			copyuv(&arg1[2],arg2);
		
		else if(isinvd(arg1) && isinvd(arg2))
			copyvv(arg1,arg2);
		
		else if(isinvd(arg1) && !isinvd(arg2))
			copyvu(arg1,&arg2[2]);
				
		return(1);
	}
	
	
	// comando "type"
	if(strcmp(cmd,"type")==0)
	{
		if(isinvd(arg1))
			catv(arg1);
		else
			catu(&arg1[2]);
		return(1);
	}
	
	//comando remove
	if(strcmp(cmd,"remove")==0){
		if(isinvd(arg1))
			removev(arg1);
		else
			removeu(&arg1[2]);
		
		return(1);
	}
}

//Desactiva todas las funciones-->

//Regresa verdadero si el nombre del archivo no comienza con // y por lo 
//   tanto es un archivo que está en el disco virtual

int isinvd(char *arg)
{
	if(strncmp(arg,"//",2)==0)
		return(0);
	else
		return(1);
}

//Muestra el directorio en el sistema de archivosd de UNIX 

int diru(char *arg1)
{
	DIR *dd;	
	struct dirent *entry;
	int dirtype = 4;
	
	if(strcmp(arg1, "//") == 0){
		arg1 = ".";
		printf("Directorio actual en sistema de archivos UNIX: \n\n");
		dirtype = 0;
	}
	
	else if(strcmp(arg1, "///") == 0){
		arg1 = "/";
		printf("Directorio raiz en sistema de archivos UNIX: \n\n");
		dirtype = 1;
	}
	
	else if(strncmp(arg1, "///", 3) == 0 && arg1[3] != '\0'){
		arg1 = &arg1[2];
		printf("Directorio %s en sistema de archivos UNIX: \n\n", arg1);
		dirtype = 2;
	}

	dd=opendir(arg1);
	if(dd==NULL)
	{
		fprintf(stderr,"Error al abrir directorio\n");
		return(-1);
	}
	
	switch(dirtype){
		case 0:
			while((entry=readdir(dd))!=NULL){
				printf("%s\t",entry->d_name);
			}
			//implementar: si esta vacio que diga que esta vacio si se me antoja
			break;
		case 1:
			while((entry=readdir(dd))!=NULL){
				//if(!isinvd(entry->d_name))
				printf("%s\t",entry->d_name);
			}
			break;
		case 2:
			while((entry=readdir(dd))!=NULL){
				//if(!isinvd(entry->d_name))
				printf("%s\t",entry->d_name);
			}
			break;
		case 4:
			printf("No esta detectando\n");
			break;
	}
	
	printf("\n\n");

	closedir(dd);	
}

int dirv(char *arg1)
{
	DIR *dd;	
	struct dirent *entry;
	int dirtype = 4;
	char nombre[18];
	
	if(arg1==NULL)
		arg1 = ".";
		
	printf("Directorio raiz de sistema de archivos VD: \n\n");

	dd=opendir(arg1);
		
	if(dd==NULL)
	{
		fprintf(stderr,"Error al abrir directorio\n");
		return(-1);
	}
	
	//[OPCIONAL] Leer cuantos nodos i hay de tabla de particiones
	for(int i = 0; i < 24; i++){
		printf("%s\t",inodename(i));
	}

	printf("\n\n");

	closedir(dd);	
}

// Copia un archivo del sistema de archivos de UNIX a un archivo destino
//   en el mismo sistema de archivos de UNIX

int copyuu(char *arg1,char *arg2)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=open(arg1,0);
	dfile=creat(arg2,0640);
	do {
		ncars=read(sfile,buffer,BUFFERSIZE);
		write(dfile,buffer,ncars);
	} while(ncars==BUFFERSIZE);
	close(sfile);
	close(dfile);
	return(1);	
}

// Copia un archivo del sistema de archivos de UNIX a un archivo destino
//   en el el disco virtual

int copyuv(char *arg1,char *arg2)
{
	int sfile,dfile, error;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=open(arg1,0);
	dfile=vdcreat(arg2,0640);
	
	printf("<Debug> --- Archivo abierto en posicion %s.\n", dfile);
	
	ncars=read(sfile,buffer,BUFFERSIZE);
	printf("<Debug> --- Reading %d characters of file.\n", ncars);
	error = vdwrite(dfile,buffer,ncars);
	if(error == -1){
		printf("<FATAL ERROR> --- Error al escribir en el archivo%s.\n", dfile);	
	}
	
	close(sfile);
	vdclose(dfile);
	return(1);	
}

// Copia un archivo del disco virtual a un archivo destino
//   en el mismo disco virtual 

int copyvv(char *arg1,char *arg2)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=vdopen(arg1,0);
	dfile=vdcreat(arg2,0640);
	
	ncars=vdread(sfile,buffer,BUFFERSIZE);
	printf("<Debug> --- Reading %d characters of file.\n", ncars);
	vdwrite(dfile,buffer,ncars);

	vdclose(sfile);
	vdclose(dfile);
	return(1);		
}


// Copia un archivo del disco virtual a un archivo destino
//   en el sistema de archivos de UNIX 

int copyvu(char *arg1,char *arg2)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=vdopen(arg1,0);
	dfile=creat(arg2,0640);
	do {
		ncars=vdread(sfile,buffer,BUFFERSIZE);
		write(dfile,buffer,ncars);
	} while(ncars==BUFFERSIZE);
	vdclose(sfile);
	close(dfile);
	return(1);	
}


// Despliega un archivo del disco virtual a pantalla

int catv(char *arg1)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=vdopen(arg1,0);
	do {
		ncars=vdread(sfile,buffer,BUFFERSIZE);
		write(1,buffer,ncars);  // Escribe en el archivo de salida estandard
	} while(ncars==BUFFERSIZE);
	vdclose(sfile);
	return(1);		
}


// Despliega un archivo del sistema de archivos de UNIX a pantalla

int catu(char *arg1)
{
	int sfile,dfile;
	char buffer[BUFFERSIZE];
	int ncars;
	
	sfile=open(arg1,0);
	do {
		ncars=read(sfile,buffer,BUFFERSIZE);
		write(1,buffer,ncars);  // Escribe en el archivo de salida estandard
	} while(ncars==BUFFERSIZE);
	close(sfile);
	return(1);		
}

int removeu(char * arg1){
	if (remove(arg1) == 0)
      printf("Deleted successfully\n");
   else
      printf("Unable to delete the file\n");
 
   return 0;
}

int removev(char * arg1){
	printf("<DEBUG> --- Removing file...\n");
	vdunlink(arg1);
}




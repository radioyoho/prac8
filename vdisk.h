#pragma pack(1)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define HEADS 8
#define SECTORS 27
#define CYLINDERS 200 

#define SEC_X_PISTA 27

//modificar si se hacen puntos extra
#define SFIP 2
#define SIP 0
#define CIP 0

struct DATE {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
};

struct OPENFILES {
	int inuse;		// 0 cerrado, 1 abierto
	unsigned short inode;
	int currpos;
	int currbloqueenmemoria;
	char buffer[1024];
	unsigned short buffindirect[512]; //		
}openfiles[24] = {0};

struct PARTITION {
	char drive_status;	
	char chs_begin[3];
	char partition_type;
	char chs_end[3];
	int lba;
	int secs_partition;
};

struct MBR {
	char bootstrap_code[446];
	struct PARTITION partition[4];
	short boot_signature;
};

struct INODE {
	char name[18];
	unsigned int datetimecreat;	// 32 bits
	unsigned int datetimemodif;	// 32 bits
	unsigned int datetimelaacc; // 32 bits
	unsigned short uid;		// 16 bits
	unsigned short gid;		// 16 bits
	unsigned short perms;	// 16 bits
	unsigned int size;			// 32 bits
	unsigned short blocks[10];	// 10 x 16 bits = 20 bytes
	unsigned short indirect;	// 16 bits
	unsigned short indirect2;	// 16 bits
} inode[24] = {0}, emptyinode = {0};

struct SECBOOTPART {
	char jump[4];
	char nombre_particion[8];
	// Tabla de parámetros del bios
	// Están los datos sobre el formato de la partición
	unsigned short sec_inicpart;		// 1 sector
	unsigned char sec_res;		// 1 sector reservado para el sector de boot de la partición
	unsigned char sec_mapa_bits_area_nodos_i;// 1 sector
	unsigned char sec_mapa_bits_bloques;	// 6 sectores
	unsigned short sec_tabla_nodos_i;	// 3 sectores
	unsigned int sec_log_particion;		// 43199 sectores
	unsigned char sec_x_bloque;			// 2 sectores por bloque
	unsigned char heads;				// 8 superficies
	unsigned char cyls;				// 200 cilindros
	unsigned char secfis;				// 27 sectores por track
	char restante[484];	// Código de arranque
}sbp;

int sl_mb_nodosi;
int sl_mb_datos;
int sl_nodosi;
int sl_datos;
int TAMBLOQUE;
char inodesmap[3] = {0};
char blocksmap[3072] = {0};
int openfiles_inicializada = 0;

void update();
int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadseclog(int drive, int seclog, char * buffer);
int vdwriteseclog(int drive, int seclog, char * buffer);
int vdcreat(char *filename,unsigned short perms);
int vdopen(char *filename,unsigned short mode);
int vdclose(int fd);
int vdunlink(char *filename);
int vdseek(int fd, int offset, int whence);
int vdwrite(int fd, char *buffer, int bytes);
int vdread(int fd, char *buffer, int bytes);
unsigned short *currpostoptr(int fd);
unsigned short *postoptr(int fd,int pos);

int searchinode(char *filename);
int nextfreeinode();
int removeinode(int numinode);
int setninode(int num, char *filename,unsigned short atribs, int uid, int gid);
int isinodefree(int inode);
int assigninode(int inode);
int unassigninode(int inode);
	
int isblockfree(int block);
int nextfreeblock();
int assignblock(int block);
int unassignblock(int block);
int writeblock(int block,char *buffer);
int readblock(int block,char *buffer);
int check_inodes_map(int num);
	
unsigned int datetoint(struct DATE date);
int inttodate(struct DATE *date,unsigned int val);
unsigned int currdatetimetoint();

char * inodename(int numnodo);

char * inodename(int numnodo){
	return(inode[numnodo].name);
}

int check_inodes_map(int num){
	if((inodesmap[num/8] & 1<<num%8)>>num%8)
		return(1);
	return(0);
}
/*
int vdreadseclog(int drive, int seclog, char * buffer){
	int ncyl,nhead,nsec;
	ncyl=((seclog + sbp.sec_inicpart)/(SEC_X_PISTA*HEADS)) + CIP;
	nhead=((seclog + sbp.sec_inicpart) + SIP)/SEC_X_PISTA%HEADS;
	nsec=(seclog + sbp.sec_inicpart)%SEC_X_PISTA + 1;
	//printf("%d, %d, %d\n", ncyl, nhead, nsec);
	vdreadsector(drive,nhead,ncyl,nsec,1,buffer);
	//agregar validacion si se me antoja
}

int vdwriteseclog(int drive, int seclog, char * buffer){
	int ncyl,nhead,nsec;
	ncyl=((seclog + sbp.sec_inicpart)/(SEC_X_PISTA*HEADS)) + CIP;
	nhead=((seclog + sbp.sec_inicpart) + SIP)/SEC_X_PISTA%HEADS;
	nsec=(seclog + SFIP - 1)%SEC_X_PISTA + 1;
	vdwritesector(drive,nhead,ncyl,nsec,1,buffer);
	//agregar validacion si se me antoja
}
*/

int vdreadseclog(int drive, int seclog, char * buffer){
	int ncyl,nhead,nsec;
	ncyl=((seclog + SFIP - 1)/(SEC_X_PISTA*HEADS)) + CIP;
	nhead=((seclog + SFIP - 1) + SIP)/SEC_X_PISTA%HEADS;
	nsec=(seclog + SFIP - 1)%SEC_X_PISTA + 1;
	//printf("%d, %d, %d\n", ncyl, nhead, nsec);
	vdreadsector(drive,nhead,ncyl,nsec,1,buffer);
	//agregar validacion si se me antoja
}

int vdwriteseclog(int drive, int seclog, char * buffer){
	int ncyl,nhead,nsec;
	ncyl=((seclog + SFIP - 1)/(SEC_X_PISTA*HEADS)) + CIP;
	nhead=((seclog + SFIP - 1) + SIP)/SEC_X_PISTA%HEADS;
	nsec=(seclog + SFIP - 1)%SEC_X_PISTA + 1;
	vdwritesector(drive,nhead,ncyl,nsec,1,buffer);
	//agregar validacion si se me antoja
}


void update(){
	int i, result;
	i = vdreadseclog(0,0,&sbp);
	sl_mb_nodosi = sbp.sec_inicpart;
	sl_mb_datos = sl_mb_nodosi + sbp.sec_mapa_bits_area_nodos_i;
	sl_nodosi = sl_mb_datos + sbp.sec_mapa_bits_bloques;
	sl_datos = sl_nodosi + sbp.sec_tabla_nodos_i;
	TAMBLOQUE = sbp.sec_x_bloque * 512;
	
	//printf("<Debug> --- SLs: %d, %d, %d, %d, %d\n", sl_mb_nodosi, sl_mb_datos, sl_nodosi, sl_datos, TAMBLOQUE);
	
	//leer mapa de bits de nodos i
	result=vdreadseclog(0,sl_mb_nodosi,inodesmap);
	
	//leer nodos i
	for(i=0;i<sbp.sec_tabla_nodos_i;i++)
		result=vdreadseclog(0, sl_nodosi+i,&inode[i*4]);
	
	//printf("<Debug> --- sec_mapa_bits: %d\n", sbp.sec_mapa_bits_bloques);
	
	
	//leer mapa de bits de bloques de datos
	for(i=0;i<sbp.sec_mapa_bits_bloques;i++)
		result=vdreadseclog(0, sl_mb_datos+i,&blocksmap+i*512);
		//printf("spam");
		//result=vdreadsector(0, 0, 0, 4+i, 1,blocksmap+i*512);
	
	//printf("<Debug> --- Primer byte de mapa de bits en update es: %d\n", blocksmap[0]);
}

//###################################################################################
//------------------------------SISTEMA DE ARCHIVOS----------------------------------
//###################################################################################

// Esta función se va a usar para crear un archivo en el 
// sistema de archivos
int vdcreat(char *filename,unsigned short perms) //[YA QUEDO]
{
	int numinode;
	int i;
	//update();
	//printf("<Debug> --- Primer byte de mapa de bits en vdcreat es: %d\n", blocksmap[0]);
	// Ver si ya existe el archivo
	numinode=searchinode(filename);
	if(numinode==-1) // Si el archivo no existe
	{
		printf("<Debug> --- El archivo no existe: buscando inodo en blanco en el mapa debits...\n");
		// Buscar un inodo en blanco en el mapa de bits (nodos i)
		numinode=nextfreeinode(); // Recorrer la tabla 
									// de nodos i buscando
									// un inodo que esté
									// libre
		if(numinode==-1) // La tabla de nodos-i está llena
		{
			printf("<Debug> --- La tabla de nodos i esta llena.\n");
			return(-1); // No hay espacio para más archivos
		}
		printf("<Debug> --- Asignado a nodo i num: %d\n", numinode);
	} else{	// Si el archivo ya existe elimina el inodo
		removeinode(numinode);
		printf("<Debug> --- El archivo ya existe, nodo borrado.\n");
	}
	// Escribir el archivo en el inodo encontrado
	// En un inodo de la tabla, escribe los datos del archivo
	setninode(numinode,filename,perms,getuid(),getgid());
		
	assigninode(numinode);

	// Poner el archivo en la tabla de archivos abiertos
	// Establecer el archivo como abierto
	if(!openfiles_inicializada)
	{
		// La primera vez que abrimos un archivo, necesitamos
		// inicializar la tabla de archivos abiertos
		for(i=0;i<24;i++)
		{
			openfiles[i].inuse=0;
			openfiles[i].currbloqueenmemoria=-1;
		}
		openfiles_inicializada=1;
		printf("<Debug> --- Estructura openfiles inicializada, prueba: la primera posicion tiene valor de %d.\n", openfiles[0].currbloqueenmemoria);
	}

	// Buscar si hay lugar en la tabla de archivos abiertos
	// Si no hay lugar, regresa -1
	i=0;
	while(openfiles[i].inuse && i<24)
		i++;
	
	printf("<Debug> --- Elegida posicion de openfiles num: %d\n", i);
	
	if(i>=24)		// Llegamos al final y no hay lugar
		return(-1);

	openfiles[i].inuse=1;	// Poner el archivo en uso
	openfiles[i].inode=numinode;  // Indicar que inodo es el
							// del archivo abierto
	openfiles[i].currpos=0; // Y la posición inicial
	// del archivo es 0
	return(i);
}

int vdopen(char *filename,unsigned short mode) //[YA QUEDO]
{
	int numinode;
	int i;
	
	// Poner el archivo en la tabla de archivos abiertos
	// Establecer el archivo como abierto
	if(!openfiles_inicializada)
	{
		// La primera vez que abrimos un archivo, necesitamos
		// inicializar la tabla de archivos abiertos
		for(i=0;i<24;i++)
		{
			openfiles[i].inuse=0;
			openfiles[i].currbloqueenmemoria=-1;
		}
		openfiles_inicializada=1;
	}
	
	numinode=searchinode(filename);
	
	i=0;
	while(openfiles[i].inuse && i<24)
		i++;

	if(i>=24)		// Llegamos al final y no hay lugar
		return(-1);

	openfiles[i].inuse=1;	// Poner el archivo en uso
	openfiles[i].inode=numinode;  // Indicar que inodo es el
							// del archivo abierto
	openfiles[i].currpos=0; // Y la posición inicial
	// del archivo es 0
	return(i);
	
}

int vdclose(int fd) //[YA QUEDO]
{
	if(fd < 0 || fd > 23)
		return(-1);
	
	openfiles[fd].inuse=-1;	// Poner el archivo en uso
	openfiles[fd].inode=NULL;  // Indicar que inodo es el
	
	return(1);
}

// Borrar un archivo del sistema de archivos
int vdunlink(char *filename) //[YA QUEDO]
{
	int numinode;
	int i;

	// Busca el inodo del archivo
	numinode=searchinode(filename);
	
	printf("<DEBUG> --- Removing inode number: %d\n", numinode);
	
	if(numinode==-1)
		return(-1); // No existe

	removeinode(numinode);
	printf("<DEBUG> --- Copiando estructura vacia a estructura de nodoi num: %d\n", numinode);
	inode[numinode] = emptyinode;
	
	for(i=0;i<sbp.sec_tabla_nodos_i;i++){
		vdwriteseclog(0, sl_nodosi+i,&inode[i*8]);
		//printf("spam\n");
	}
}

// Mover el puntero del archivo a la posición indicada
// a partir de: el inicio si whence es 0, de la posición 
// actual si whence es 1, o a partir del final si whence es 2
int vdseek(int fd, int offset, int whence) //[YA QUEDO]
{
	unsigned short oldblock,newblock;
	//update();
	
	// Si no está abierto regresa error
	if(openfiles[fd].inuse==0)
		return(-1);  // Regresar ERROR

	// La función currpostoptr es una función que a partir de 
	// la posición actual del apuntador del archivo, 
	// me regresa la dirección de memoria en el nodo i o en 
	// en el bloque de apuntadores que contiene el bloque
	// donde está el puntero del archivo
	oldblock=*currpostoptr(fd);
		
	if(whence==0) // A partir del inicio
	{
		// Si el offset está antes del inicio, o después 
	// del final, regresar un error
		if(offset<0||offset>inode[openfiles[fd].inode].size)
			return(-1);
		openfiles[fd].currpos=offset;

	} else if(whence==1) // A partir de la posición actual
	{

		// Validar si no estás quieriendo mover antes del
		// inicio del archivo o después del final		
		if(openfiles[fd].currpos+offset>inode[openfiles[fd].inode].size ||
		openfiles[fd].currpos+offset<0)
			return(-1);
		openfiles[fd].currpos+=offset;

	} else if(whence==2) // A partir del final
	{
		if(offset>inode[openfiles[fd].inode].size ||
		   openfiles[fd].currpos-offset<0)
			return(-1);
		openfiles[fd].currpos=inode[openfiles[fd].inode].size-offset;
	} else
		return(-1);

	// Verificamos si la nueva posición del puntero es un
	// bloque diferente al que estábamos,si es así hay que
	// cargar ese bloque a memoria
	newblock=*currpostoptr(fd);
	
	// Después de mover el puntero, ahora me cambié a otro
	// bloque?
	if(newblock!=oldblock)
	{
		// Escribir el bloque viejo
		writeblock(oldblock,openfiles[fd].buffer);
		// Leer el bloque nuevo
		readblock(newblock,openfiles[fd].buffer);
		// Indicar el nuevo bloque como bloque actual
		// en la tabla de archivos abiertos
		openfiles[fd].currbloqueenmemoria=newblock;
	}

	// Regresamos la posición actual del archivo
	return(openfiles[fd].currpos);
}

// Esta es la función más difícil, 
int vdwrite(int fd, char *buffer, int bytes) //[YA QUEDO]
{
	int currblock;
	int currinode;
	int cont=0;
	int sector;
	int i;
	int result;
	unsigned short *currptr;

	//update();
	
	// Si no está abierto, regresa error
	if(openfiles[fd].inuse==0)
		return(-1);

	currinode=openfiles[fd].inode;
	printf("<Debug> --- Escribiendo en nodo i: %d...\n", currinode);
	
	// Copiar byte por byte del buffer que recibo como 
	// argumento al buffer del archivo
	while(cont<bytes)
	{
		// Obtener la dirección de donde está el bloque que corresponde
		// a la posición actual
		currptr=currpostoptr(fd);
		if(currptr==NULL)
			return(-1);
	
		// Cuál es el bloque en el que escibiríamos
		currblock=*currptr;

		// Si el bloque está en blanco, dale uno
		//printf("<Debug> --- Primer byte de mapa de bits es: %d\n", blocksmap[0]);
		if(currblock==0)
		{
			printf("<Debug> --- Nodo i no tenia bloques asignados. Asignandole uno...\n");
			currblock=nextfreeblock();
			printf("<Debug> --- Bloque asignado num: %d\n", currblock);
			// El bloque encontrado ponerlo en donde
			// apunta el apuntador al bloque actual
			*currptr=currblock;
			assignblock(currblock);	// Asignarlo en el mapa de bits 
			printf("<Debug> --- Bit asignado en mapa de bits de bloques (Sectores logicos 2-7)\n");
			
			// Escribir el sector de la tabla de nodos i
			// En el disco
			sector=(currinode/8);
			result=vdwriteseclog(0, sl_nodosi+sector,&inode[sector*8]);
		}

		// Si el bloque de la posición actual no está en memoria
		// Lee el bloque al buffer del archivo
		if(openfiles[fd].currbloqueenmemoria!=currblock)
		{
			// Leer el bloque actual hacia el buffer que
			// está en la tabla de archivos abiertos
			readblock(currblock,openfiles[fd].buffer);			
			// Actualizar en la tabla de archivps abiertos
			// el bloque actual
			
			openfiles[fd].currbloqueenmemoria=currblock;
		}

		// Copia el caracter al buffer
		openfiles[fd].buffer[openfiles[fd].currpos%TAMBLOQUE]=buffer[cont];

		// Incrementa posición actual del actual
		openfiles[fd].currpos++;

		// Si la posición es mayor que el tamaño, modifica el tamaño
		if(openfiles[fd].currpos>inode[currinode].size)
			inode[openfiles[fd].inode].size=openfiles[fd].currpos;

		// Incrementa el contador
		cont++;

		// Si se llena el buffer, escríbelo
		if(openfiles[fd].currpos%TAMBLOQUE==0){
			writeblock(currblock,openfiles[fd].buffer);
			printf("<Debug> --- Se escribio el buffer.\n");
		}
	}
	if(openfiles[fd].currpos%TAMBLOQUE!=0){
		writeblock(currblock, openfiles[fd].buffer);
		printf("<Debug> --- Se escribio el buffer.\n");
	}
	return(cont);
}

int vdread(int fd, char *buffer, int bytes)	//return: -1 -> error; 1 -> todo bien; 2 -> ya no hay bloques
{
	int currblock;
	int currinode;
	int cont=0;
	int sector;
	int i;
	int result;
	unsigned short *currptr;
	
	//update();
	
	// Si no está abierto, regresa error
	if(openfiles[fd].inuse==0)
		return(-1);

	currinode=openfiles[fd].inode;
	
	// Copiar byte por byte del buffer que recibo como 
	// argumento al buffer del archivo
	while(cont<bytes)
	{
		// Obtener la dirección de donde está el bloque que corresponde
		// a la posición actual
		currptr=currpostoptr(fd);
		if(currptr==NULL)
			return(-1);
	
		// Cuál es el bloque en el que escibiríamos
		currblock=*currptr;

		// Si el bloque está en blanco, dale uno
		if(currblock==0)
		{
			return(2);
		}

		// Si el bloque de la posición actual no está en memoria
		// Lee el bloque al buffer del archivo
		if(openfiles[fd].currbloqueenmemoria!=currblock)
		{
			// Leer el bloque actual hacia el buffer que
			// está en la tabla de archivos abiertos
			readblock(currblock,openfiles[fd].buffer);			
			// Actualizar en la tabla de archivps abiertos
			// el bloque actual
			openfiles[fd].currbloqueenmemoria=currblock;
		}

		// Copia el caracter al buffer
		buffer[cont]=openfiles[fd].buffer[openfiles[fd].currpos%TAMBLOQUE];

		// Incrementa posición actual del actual
		openfiles[fd].currpos++;

		// Incrementa el contador
		cont++;
	}
	return(cont);

	
}

unsigned short *currpostoptr(int fd) //[YA QUEDO]
{
	unsigned short *currptr;

	currptr=postoptr(fd,openfiles[fd].currpos);

	return(currptr);
}

unsigned short *postoptr(int fd,int pos)
{
	int currinode;
	unsigned short *currptr;
	unsigned short indirect1;

	currinode=openfiles[fd].inode;

	// Está en los primeros 10 K
	if((pos/1024)<10)
		// Está entre los 10 apuntadores directos
		currptr=&inode[currinode].blocks[pos/1024];
	else if((pos/1024)<522)
	{
		// Si el indirecto está vacío, asígnale un bloque
		if(inode[currinode].indirect==0)
		{
			// El primer bloque disponible
			indirect1=nextfreeblock();
			assignblock(indirect1); // Asígnalo
			inode[currinode].indirect=indirect1;
		} 
		currptr=&openfiles[fd].buffindirect[pos/1024-10];
	}
	else
		return(NULL);

	return(currptr);
}

//###################################################################################
//---------------------------------------NODOS I-------------------------------------
//###################################################################################


// Buscar en la tabla de nodos I, el nodo I que contiene el 
// nombre del archivo indicado en el apuntador a la cadena
// Regresa el número de nodo i encontrado
// Si no lo encuentra, regresa -1
int searchinode(char *filename)  //[[YA QUEDO]]
{
	int i;
	int free;
	int result;
	//update();
	// El nombre del archivo no debe medir más de 18 bytes
	if(strlen(filename)>17)
	  	filename[17]='\0';

	// Recorrer la tabla de nodos I que ya tengo en memoria
	// desde el principio hasta el final buscando el archivo.
	i=0;
	while(strcmp(inode[i].name,filename) && i<sbp.sec_tabla_nodos_i)
		i++;

	if(i>= sbp.sec_tabla_nodos_i)
		return(-1);		// No se encuentra el archivo
	else
		return(i);		// La posición donde fue encontrado 
}

// Buscar en el mapa de bits, el primer nodo i que esté libre, es decir, que su bit está en 0, me regresa ese dato
int nextfreeinode()				//[[YA QUEDO]]
{
	int i,j;
	int result;
	// Recorrer byte por byte mientras sea 0xFF sigo recorriendo
	i=0;
	printf("<Debug> --- El byte en inodesmap es %X\n", inodesmap[i]);
	
	while(inodesmap[i]==0xFFFFFFFF && i<3)
		i++;

	if(i<3)
	{
		// Recorrer los bits del byte, para encontrar el bit
		// que está en cero
		j=0;
		while(inodesmap[i] & (1<<j) && j<8)
			j++;
		
		printf("<Debug> --- Nodo i encontrado esta en byte %d y en bit %d.\n", i, j);
		
		return(i*8+j); // Regresar el bit del mapa encontrado en cero
	}
	else // Todos los bits del mapa de nodos i están en 1
		return(-1); // -1 significa que no hay nodos i disponibles		
}

int removeinode(int numinode) //[YA QUEDO]
{
	int i;
	unsigned short temp[512]; // 1024 bytes
	// Desasignar los bloques directos en el mapa de bits que 
	// corresponden al archivo
	//update();
	// Recorrer los apuntadores directos del nodo i
	for(i=0;i<10;i++)
		if(inode[numinode].blocks[i]!=0) // Si es dif de cero
										// Si está asignado
		{
			unassignblock(inode[numinode].blocks[i]);
			inode[numinode].blocks[i]=0;
		}

	// Si el bloque indirecto, ya está asignado
	if(inode[numinode].indirect!=0)
	{
	// Leer el bloque que contiene los apuntadores
	// a memoria
		readblock(inode[numinode].indirect,(char *) temp);
		// Recorrer todos los apuntadores del bloque para
		// desasignarlos 
		for(i=0;i<512;i++)
			if(temp[i]!=0)
				unassignblock(temp[i]);
		// Desasignar el bloque que contiene los apuntadores
		unassignblock(inode[numinode].indirect);
		inode[numinode].indirect=0;
	}

	// Poner en cero el bit que corresponde al inodo en el mapa
	// de bits de nodos-i
	unassigninode(numinode);
	return(1);
}

// Escribir en la tabla de nodos-I del directorio raíz, los datos de un archivo
int setninode(int num, char *filename,unsigned short atribs, int uid, int gid) //[YA QUEDO]
{
	int i;
	
	int result;
	//update();
	//Antes de continuar debe cargarse en memoria el sector de boot de la partición.
	//Con los datos que están ahí, calcular:
	//El sector lógico donde empieza la tabla de nodos-i del directorio raiz.
	//También vamos a usar el número de sectores que tiene la tabla de nodos-i

	printf("<Debug> --- Num de inodo a escribir en la tabla de inodos: %d\n", num);
	// Si la tabla de nodos-i no está en memoria, 
	// hay que cargarla a memoria

	// Copiar el nombre del archivo en el nodo i
	strncpy(inode[num].name,filename,18);
	
	printf("<Debug> --- Copied name '%s' to inode table: '%s'.\n", filename,inode[num].name);
	
	// Asegurando que el último caracter es el terminador (cero)
	if(strlen(inode[num].name)>17)
	 	inode[num].name[17]='\0';

	// Poner en el nodo I las fechas y horas de creación
	// con las fecha y hora actual
	inode[num].datetimecreat=currdatetimetoint(); 
	inode[num].datetimemodif=currdatetimetoint();
	// Información sobre el dueño, grupo dueño y atributos
	// Para propósitos de la práctica, los datos que se 
	// refieren a dueño, grupo dueño y atributos (permisos)
	// no son relevantes.
	inode[num].uid=uid;
	inode[num].gid=gid;
	inode[num].perms=atribs;

	// Un archivo nuevo, su tamaño inicial es 0
	inode[num].size=0;	// Tamaño del archivo en 0
	
	// Establecer los apuntadores a bloques directos en 0
	for(i=0;i<10;i++)
		inode[num].blocks[i]=0;

	// Establecer los apuntadores indirectos en 0
	inode[num].indirect=0;
	inode[num].indirect2=0;
	
	printf("<Debug> --- Nombre de nodo i a escribir en disco: %s.\n", inode[num].name);

	// Optimizar la escritura escribiendo solo el sector lógico que
	// corresponde al inodo que estamos asignando.
	// i=num/8;
	// result=vdwriteseclog(inicio_nodos_i+i,&inode[i*8]);
	
	printf("<Debug> --- Sectores logicos en tabla de nodos i: %d.\n", sbp.sec_tabla_nodos_i);
	printf("<Debug> --- Sector logico de inicio de nodos i: %d.\n", sl_nodosi);
	
	for(i=0;i<sbp.sec_tabla_nodos_i;i++){
		result=vdwriteseclog(0, sl_nodosi+i,&inode[i*8]);
		//printf("spam\n");
	}
	
	printf("<Debug> --- Escrito nodo i en sector logico de nodos i (Sectores logicos 8 - 10).\n");
	return(num);
}

// Usando el mapa de bits, determinar si un nodo i, está libre u ocupado.
int isinodefree(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;
	
	//update();
	
	if(inodesmap[offset] & (1<<shift))
		return(0); // El nodo i ya está ocupado
	else
		return(1); // El nodo i está libre
}

// Poner en 1, el bit que corresponde al número de inodo indicado
int assigninode(int inode) //[YA QUEDO]
{
	int offset=inode/8;
	int shift=inode%8;
	int result;
	
	//update();
	
	inodesmap[offset]|=(1<<shift); // Poner en 1 el bit indicado
	result = vdwriteseclog(0, sl_mb_nodosi,inodesmap);
	
	printf("<Debug> --- Escrito bit en mapa de bits de nodos i (Sector logico 1)\n");
	return(1);
}

// Poner en 0, el bit que corresponde al número de inodo indicado
int unassigninode(int inode)
{
	int offset=inode/8;
	int shift=inode%8;
	int result;
	 
	//update();
	
	inodesmap[offset]&=(char) ~(1<<shift); // Poner en cero el bit que corresponde al inodo indicado
	vdwriteseclog(0, sl_mb_nodosi,inodesmap);
	return(1);
}

//###################################################################################
//---------------------------------BLOQUES DE DATOS----------------------------------
//###################################################################################

// Usando la información del mapa de bits del area de datos, saber si un bloque está libre o no
int isblockfree(int block)
{
	int offset=block/8; // Número de byte en el mapa
	int shift=block%8; // Número de bit en el byte
	int result;
	int i;
	
	//update();

	if(blocksmap[offset] & (1<<shift))
		return(0);	// Si el bit está en 1, regresar 0 (no está libre)
	else
		return(1);	// Si el bit está en 0, regresar 1 (si está libre)
}

// Usando el mapa de bits, buscar el siguiente bloque libre
int nextfreeblock() //[YA QUEDO]
{
	int i,j;
	int result;

	// Empezar desde el primer byte del mapa de bloques.
	i=0;
	// Si el byte tiene todos los bits en 1, y mientras no
	// lleguemos al final del mapa de bits
	while(blocksmap[i]==0xFFFFFFFF && i<sbp.sec_mapa_bits_bloques*512)
		i++;

	// Si no llegué al final del mapa de bits, quiere decir
	// que aún hay bloques libres
	if(i<sbp.sec_mapa_bits_bloques*512)
	{
		j=0;
		while(blocksmap[i] & (1<<j) && j<8)
			j++;

		return(i*8+j);	// Retorno el número de bloque
						// que se encontró disponible
	}
	else
		// Si ya no hublo bloques libres, regresar -1 = Error
		return(-1);
}

// Asignar un bloque en 1 en el mapa de bits, lo cual significa que estaría ya ocupado.
int assignblock(int block) //[YA QUEDO]
{
	int offset=block/8;
	int shift=block%8;
	int result;
	int i;
	int sector;
	
	blocksmap[offset]|=(1<<shift);

	// Determinar en que número de sector está el bit que 
	// modificamos 	
	sector=(offset/512);
	// Escribir el sector del mapa de bits donde está el bit
	// que modificamos
	vdwriteseclog(0, sl_mb_datos+sector,blocksmap+sector*512);
	//for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
	//	vdwriteseclog(mapa_bits_bloques+i,blocksmap+i*512);
	return(1);
}

// Poner en 0 el bit que corresponde a un bloque en el mapa de bits, esto equivale a decir que el bloque está libre
int unassignblock(int block) //[YA QUEDO]
{
	int offset=block/8;
	int shift=block%8;
	int result;
	int sector;

	// Determinar si tenemos el sector de boot de la partición en memoria

	// Calcular el sector lógico donde está el mapa de bits de los bloques
	//mapa_bits_bloques= secboot.sec_inicpart+secboot.sec_res+secboot.sec_mapa_bits_nodos_i;

	blocksmap[offset]&=(char) ~(1<<shift);

	// Calcular en que sector está el bit modificado
	// Escribir el sector en disco
	sector=(offset/512);
	result = vdwriteseclog(0, sl_mb_datos+sector,blocksmap+sector*512);
	// for(i=0;i<secboot.sec_mapa_bits_bloques;i++)
	//	vdwriteseclog(mapa_bits_bloques+i,blocksmap+i*512);
	return(1);
}

int writeblock(int block,char *buffer) //[YA QUEDO]
{
	int result;
	int i;

	// Escribir todos los sectores que corresponden al 
	// bloque
	for(i=0;i<sbp.sec_x_bloque;i++)
		vdwriteseclog(0, sl_datos+(block-1)*sbp.sec_x_bloque+i,buffer+512*i);
	return(1);	
}

int readblock(int block,char *buffer) //[YA QUEDO]
{
	int result;
	int i;

	// Determinar si el sector de boot de la partición está en memoria, si no está en memoria, cargarlo

	for(i=0;i<sbp.sec_x_bloque;i++)
		result = vdreadseclog(0, sl_datos+(block-1)*sbp.sec_x_bloque+i,buffer+512*i);
	return(1);	
}

//###################################################################################
//---------------------------------FUNCIONES DE DATE---------------------------------
//###################################################################################

unsigned int datetoint(struct DATE date) //[YA QUEDO]
{
	unsigned int val=0;

	val=date.year-1970;
	val<<=4;
	val|=date.month;
	val<<=5;
	val|=date.day;
	val<<=5;
	val|=date.hour;
	val<<=6;
	val|=date.min;
	val<<=6;
	val|=date.sec;
	
	return(val);
}

int inttodate(struct DATE *date,unsigned int val)
{
	date->sec=val&0x3F;
	val>>=6;
	date->min=val&0x3F;
	val>>=6;
	date->hour=val&0x1F;
	val>>=5;
	date->day=val&0x1F;
	val>>=5;
	date->month=val&0x0F;
	val>>=4;
	date->year=(val&0x3F) + 1970;
	return(1);
}

// Obtiene la fecha y hora actual del sistema y la 
// empaqueta en un entero de 32 bits
unsigned int currdatetimetoint() //[YA QUEDO]
{
	struct tm *tm_ptr;
	time_t the_time;
	
	struct DATE now;

	// Llamada al sistema para obtener la fecha/hora actual
	// y guardar el resultado en the_time
	(void) time(&the_time);
	tm_ptr=gmtime(&the_time);

	// Poner la fecha y hora obtenida en la estructura TIME
	now.year=tm_ptr->tm_year-70;
	now.month=tm_ptr->tm_mon+1;
	now.day=tm_ptr->tm_mday;
	now.hour=tm_ptr->tm_hour;
	now.min=tm_ptr->tm_min;
	now.sec=tm_ptr->tm_sec;
	// Convertirlo a un entero de 32 bits y regresar el 
// resultado
	return(datetoint(now)); 
}

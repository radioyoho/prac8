#define HEADS 8
#define SECTORS 27
#define CYLINDERS 200 

#define SEC_X_PISTA 27

//modificar si se hacen puntos extra
#define SFIP 2
#define SIP 0
#define CIP 0

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
};

int vdwritesector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadsector(int drive, int head, int cylinder, int sector, int nsecs, char *buffer);
int vdreadseclog(int drive, int seclog, char * buffer);
int vdwriteseclog(int drive, int seclog, char * buffer);
void read_parameters();

int vdreadseclog(int drive, int seclog, char * buffer){
	int ncyl,nhead,nsec;
	ncyl=((seclog + SFIP - 1)/(SEC_X_PISTA*HEADS)) + CIP;
	nhead=((seclog + SFIP - 1) + SIP)/SEC_X_PISTA%HEADS;
	nsec=(seclog + SFIP - 1)%SEC_X_PISTA + 1;
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

void read_parameters(){
	struct SECBOOTPART sbp = {0};
	int i;
	i = vdwritesector(0,0,0,2,1,(void *)&sbp);
}




#pragma pack(2)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vdisk.h"

// Debe medir 512 bytes

void main(){
	struct SECBOOTPART secboot = {0};
	struct SECBOOTPART newsec = {0};
	int i;
	printf("%d\n",sizeof(struct SECBOOTPART));	
	char buffer[512] = {0};
	//tabla de datos
	secboot.sec_inicpart = 1;
	secboot.sec_res = 1;
	secboot.sec_mapa_bits_area_nodos_i = 1;
	secboot.sec_mapa_bits_bloques = 6;
	secboot.sec_tabla_nodos_i = 3;
	secboot.sec_log_particion = 43199;
	secboot.sec_x_bloque = 2;
	secboot.heads = 8;
	secboot.cyls = 200;
	secboot.secfis = 27;
	
	//escribir tabla en sec boot part
	i = vdwritesector(0,0,0,2,1,(void *)&secboot);
	
	//limpiar mb nodos i sec fis 3
	i = vdwritesector(0,0,0,3,1,(void *)buffer);
	
	//limpiar mb area de datos
	buffer[0] = 0x01;
	i = vdwritesector(0,0,0,4,1,(void *)buffer);
	
	buffer[0] = 0;
		
	for(i = 5; i < 43201; i++){
		vdwritesector(0,0,0,i,1,(void *)buffer);	
	}
	
	
}
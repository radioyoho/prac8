#pragma pack(2)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vdisk.h"

//guardar tabla de alguna forma


void main(){
	struct MBR mbr = {0};
	char buffer[512];
	int i;
	//buffer = "Hello world!";
	//mbr.bootstrap_code = NULL;
	
	//-------------for debugging---------------
	for (i = 0;i<512;i++)
		buffer[i] = 0;
	//-----------------------------------------
	
	mbr.partition[0].drive_status = 0x80;
	mbr.partition[0].partition_type = 0x01;
	mbr.partition[0].lba = 11; 				//checar si el comienzo de sectores lógicos está bien
	mbr.partition[0].secs_partition = 43200;
	
	mbr.partition[0].chs_begin[0] = 0;
	mbr.partition[0].chs_begin[1] = 2 & 0x1f;
	mbr.partition[0].chs_begin[2] = 0;
	
	mbr.partition[0].chs_end[0] = 7;
	mbr.partition[0].chs_end[1] = 26 & 0x1f;
	mbr.partition[0].chs_end[2] = 199 & 0xff;
	
	mbr.partition[0].chs_end[1] = mbr.partition[0].chs_end[1] | (199 & 0x300)>>2; //check 
	
	printf("mbr weighs: %d\n", sizeof(mbr));
	
	i = vdwritesector(0,0,0,1,1,(void *)&mbr);
	printf("vdwrite returns: %d\n", i);
	
}
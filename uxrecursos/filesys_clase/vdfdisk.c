/* Este programa escribe el MBR que contiene la tabla de particiones
   en el disco */

#include <stdio.h>
#include <string.h>
#include "vdisk.h"

#pragma pack(2)

struct PARTITION 
{
	unsigned char status;
	unsigned char CHS_begin[3];
	unsigned char partition_type;
	unsigned char CHS_end[3];
	unsigned int LBA;
	unsigned int secs_per_partition;
};

struct MBR 
{
	unsigned char bootcode[446];	// 446 +
	struct PARTITION partition[4];	//  64 +
	unsigned short bootsignature;	//   2 = 512
};

int main()
{
	struct MBR mbr;
	
	int cilinic=0,supinic=0,sfinic=2;
	int cilfin=599,supfin=5,sffin=17;
	
	memset(&mbr,0,512);
	
	mbr.partition[0].CHS_begin[0]=(unsigned char) supinic;
	mbr.partition[0].CHS_begin[1]=(unsigned char) sfinic & 0x3F;
	mbr.partition[0].CHS_begin[2]=(unsigned char) (cilinic & 0xFF);
	mbr.partition[0].CHS_begin[1]=mbr.partition[0].CHS_begin[1] |
									(unsigned char) ((cilinic & 0x300)>>2);
	
	mbr.partition[0].CHS_end[0]=(unsigned char) supfin;
	mbr.partition[0].CHS_end[1]=(unsigned char) sffin & 0x3F;
	mbr.partition[0].CHS_end[2]=(unsigned char) (cilfin & 0xFF);
	mbr.partition[0].CHS_end[1]=mbr.partition[0].CHS_end[1] |
									(unsigned char) ((cilfin & 0x300)>>2);
	
	printf("CHS Inicial %X %X %X ",mbr.partition[0].CHS_begin[2]
		   							,mbr.partition[0].CHS_begin[1]
		   							,mbr.partition[0].CHS_begin[0]);
	printf("CHS Final %X %X %X\n",mbr.partition[0].CHS_end[2]
		   							,mbr.partition[0].CHS_end[1]
		   							,mbr.partition[0].CHS_end[0]);
	
	vdwritesector(0,0,0,1,1,(char *) &mbr);

}
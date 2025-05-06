#include "ftl.h"
#include <stdlib.h>
#include <stdio.h>

FTL ftl;
byte8 totalReq = 0, writeReq = 0;

int main(){
    FILE *TraceFile, *ConfigFile, *ResultFile;
    char *config_file = "./files/config.txt";
    char *trace_file  = "./files/trace.txt";
    
    // BYTE lineBuffer[MAX_BUFFER_SIZE];
    // char operation[100], buffer[100];
    // BYTE ResultFileName[100], InitFileName[100], ConfigFileName[100];
    

    // DWORD sector_nr, len, tag, offset_sector, len_sector, writeReq;

    // writeReq = 0;					
	// MLCflash.REGION_NUMBER = 1;	
	// MLCflash.DEMOTE_THRESHOLD = 0xffffffff;
	// MLCflash.PROMOTE_THRESHOLD = 0;	

    printf("Initializing %s ...\n", config_file);
    ConfigFile = fopen(config_file, "r");
    FTLinit(&ftl, ConfigFile);


    // printf("Logical Addree Size: %lld, Phy Addr size: %lld\n", ftl.config.LSize, ftl.config.PSize);

    char operation[100], buffer[100];
    byte8 tag, sectorNum, len;
    TraceFile = fopen(trace_file, "r");
    // int cnt = 0;
    while(!feof(TraceFile)){
        // sector_nr = 0;
        // len = 0;

        fgets(buffer, 100, TraceFile);

        sscanf(buffer, "%lld %s %lld %lld", &tag, &operation, &sectorNum, &len);

        // If over the bound, in this simulation, I will ignore that operation.
        if(sectorNum + len >= ftl.config.LSize / ftl.config.sectorSize || len == 0) continue;

        // In this simulation, We just deal with the write request.
        switch(operation[0]) {
        case 'R':
            totalReq++;
            break;
        case 'W':
            writeReq++;
            totalReq++;
            while(1){
                writeSector(&ftl, sectorNum);
                sectorNum += 1;
                len -= 1;

                if(len <= 0)break;
            }
            break;
        }
    } 
    // printf("%lld %lld\n", totalReq, writeReq);
    printf("%d \n", ftl.mapTable[2].pageNo);


    // fclose(traceFile);
	// printf("simulation done\n");

	// printf("printing ResultFile\n");
	// sprintf(ResultFileName,"D:\\SimResultTmp\\(IO)NoSep+Greedy_%s",ConfigFileName);

	// ResultFile = fopen(ResultFileName, "w+t");
	// printf("user page write = %d\n",MLCflash.stat.userPageWrite);
	// fprintf(ResultFile,"user page write = %d\n",MLCflash.stat.userPageWrite);

	// //=======page write================================================================================================================
	// printf("total page write = %d\n",MLCflash.stat.pageWrite);
	// fprintf(ResultFile,"total page write = %d\n",MLCflash.stat.pageWrite);
	// //=================================================================================================================================

	// //=======Erase Count===============================================================================================================
	// printf("totalEC = %lu\n",MLCflash.stat.blockErase);
	// fprintf(ResultFile,"totalEC = %lu\n",MLCflash.stat.blockErase);
	// //=================================================================================================================================

	// printf("Max page copy = %d\n",MLCflash.stat.maxCopy);
	// fprintf(ResultFile,"Max page copy = %d\n",MLCflash.stat.maxCopy);

	// //=======Avg Copy==================================================================================================================
	// printf("AVG page copy = %f\n",(double)(MLCflash.stat.pageWrite-MLCflash.stat.userPageWrite)/MLCflash.stat.blockErase );
	// fprintf(ResultFile,"AVG page copy = %f\n",(double)(MLCflash.stat.pageWrite-MLCflash.stat.userPageWrite)/MLCflash.stat.blockErase );
	// //=================================================================================================================================

    FTLfree(&ftl);
}
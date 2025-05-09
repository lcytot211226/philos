#include "ftl.h"
#include <stdlib.h>
#include <stdio.h>

void print(FTL *FTLptr) {
    for(int i=0;i<10;i++) {
        if(FTLptr->mapTable[i].used) {
            printf("%d: %d block %d page\n",i , FTLptr->mapTable[i].blockNo, FTLptr->mapTable[i].pageNo);
        }
    }
    
}

int main() {
    FTL ftl;
    byte8 totalReq = 0, writeReq = 0;

    FILE *TraceFile, *ConfigFile, *ResultFile;
    char *config_file = "./files/config.txt";
    char *trace_file  = "./files/trace.txt";

    printf("Initializing %s ...\n", config_file);
    ConfigFile = fopen(config_file, "r");
    FTLinit(&ftl, ConfigFile);


    // printf("Logical Addree Size: %lld, Phy Addr size: %lld\n", ftl.config.LSize, ftl.config.PSize);

    char operation[100], buffer[100];
    byte8 tag, sectorNum, len;
    TraceFile = fopen(trace_file, "r");
    // int cnt = 0;
    while(!feof(TraceFile)){

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

                // threshold for GC
                if(ftl.FreeList.cnt <= 100) {
                    GC(&ftl);
                }

                if(len <= 0)break;
            }
            break;
        }
    } 
    // printf("%lld %lld\n", totalReq, writeReq);
    


    fclose(TraceFile);
	// printf("simulation done\n");

    print(&ftl);
    FTLfree(&ftl);
}
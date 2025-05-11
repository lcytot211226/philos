#include "ftl.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


void FTLinit(FTL *FTLptr,FILE *fp){
    byte1 buffer[1024], junk[105];

    Config *C = &(FTLptr->config);

    // Total Physical Size (bytes)
    fgets(buffer, 1024, fp);
    sscanf(buffer, "%s	%lld", junk, &(C->PSize));
    assert(strcmp(junk, "PsizeByte") == 0);

    // Total Logical Size (bytes)
    fgets(buffer, 1024, fp);
	sscanf(buffer,"%s	%lld", junk, &(C->LSize));
	assert(strcmp(junk,"LsizeByte")==0);

    // Size per block
    fgets(buffer, 1024, fp);
    sscanf(buffer, "%s %lld", junk, &(C->blockSize));
    assert(strcmp(junk,"blockSizeByte")==0);

    // Size per page
    fgets(buffer, 1024, fp);
    sscanf(buffer, "%s %lld", junk, &(C->pageSize));
    assert(strcmp(junk,"pageSizeByte")==0);

    // Size per sector
    fgets(buffer, 1024, fp);
    sscanf(buffer, "%s %lld", junk, &(C->sectorSize));
    assert(strcmp(junk,"sectorSizeByte")==0);

    fclose(fp); 

    assert(C->LSize % C->blockSize == 0);
	assert(C->PSize % C->blockSize == 0);
	assert(C->blockSize % C->pageSize == 0);
	assert(C->pageSize % C->sectorSize == 0);
	assert(C->LSize < C->PSize);

	C->blocksInL = C->LSize     / C->blockSize;
	C->blocksInP = C->PSize     / C->blockSize;
	C->pagesInBlock    = C->blockSize / C->pageSize;
	C->sectorsInPage   = C->pageSize  / C->sectorSize;

	// block page
	FTLptr->blocks = (Block *) calloc(C->blocksInP, sizeof(Block));

	FTLptr->blocks[0].pages = (Page *) calloc(C->blocksInP * C->pagesInBlock, sizeof(Page));
	for( byte8 i = 0; i < C->blocksInP ; i++ ) {
		// distribute pages
		FTLptr->blocks[i].pages = FTLptr->blocks[0].pages + i * C->pagesInBlock;

		// reset block
		FTLptr->blocks[i].eraseCnt = 0;
		FTLptr->blocks[i].validPagesCnt = C->pagesInBlock;
		// reset page
		for( byte8 j = 0; j < C->pagesInBlock; j++ ) {
			FTLptr->blocks[i].pages[j].state = 0;
			// FTLptr->blocks[i].pages[j].sectors = 0x00000000;
		}
	}
	
	// GClist
	FTLptr->GCList = (List *) calloc(C->pagesInBlock+1, sizeof(List));
	for( byte8 i = 0; i <= C->pagesInBlock; i++ ) 
		FTLptr->GCList[i].cnt = 0;
	

	// freelist
	FTLptr->FreeList.head = 0;
	FTLptr->FreeList.tail = C->blocksInP-1;
	FTLptr->FreeList.cnt  = C->blocksInP;
	FTLptr->FreeList.usedPage = 0;
	for( byte8 i = 0; i < C->blocksInP-1 ; i++ ) {
		FTLptr->blocks[i].FreeNext = i+1;
	}

	// table
	FTLptr->mapTable = (Table*) calloc(C->LSize / C->pageSize, sizeof(Table));
	for( byte8 i = 0; i < C->LSize / C->pageSize; i++ )
		FTLptr->mapTable[i].used = 0;

	// writebuf
	FTLptr->writeBuf.currentPage = FTL_NULL;

	// data
	FTLptr->recordData.actualWrite = 0;
	FTLptr->recordData.hostWrite = 0;
	FTLptr->recordData.eraseTimes = 0;
}

void FTLfree(FTL *FTLptr){
	free(FTLptr->blocks[0].pages);
	free(FTLptr->blocks);
	free(FTLptr->mapTable);
	free(FTLptr->GCList);
}
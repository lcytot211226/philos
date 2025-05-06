#include "ftl.h"
#include <stdlib.h>
#include <stdio.h>

void writeSector(FTL *FTLptr, byte8 sectorNum) {
    
    Config *C = &(FTLptr->config);
    WriteBuf *wbuf = &(FTLptr->writeBuf);
    Table *maptable = (FTLptr->mapTable);

    byte8 LPageNo = sectorNum / C->sectorsInPage;
    byte4 LSectorNo = sectorNum % C->sectorsInPage;

    // need to write buffer to new posi
    if (wbuf->currentPage != FTL_NULL && wbuf->currentPage != LPageNo) { 
        
        // mark old Ppage as invalid
        if(maptable[wbuf->currentPage].used) {
            markPageInvalid(FTLptr, &maptable[wbuf->currentPage]);
        } 
        
        byte4 newPageNo, newBlockNo;

        // write buffer to new page
        WriteBufferToNewPage(FTLptr, &newPageNo, &newBlockNo);
        // printf("%d\n", newPageNo);
        // update mapping table
        maptable[wbuf->currentPage].pageNo = newPageNo;
        maptable[wbuf->currentPage].blockNo = newBlockNo;
        maptable[wbuf->currentPage].used = 1;

        // copy data from LPageNo to buffer
        // negelect...
    }
    wbuf->currentPage = LPageNo;

}

void markPageInvalid(FTL *FTLptr, Table *entry) {
    FTLptr->blocks[entry->blockNo].pages[entry->pageNo].state = 2;
    FTLptr->blocks[entry->blockNo].validPagesCnt--;

}

void WriteBufferToNewPage(FTL *FTLptr, byte4 *newPageNo, byte4 *newBlockNo) {
    *newBlockNo = FTLptr->FreeList.head;
    *newPageNo  = FTLptr->FreeList.usedPage;
    FTLptr->FreeList.usedPage++;
    if(FTLptr->FreeList.usedPage >= FTLptr->config.pagesInBlock) {
        FTLptr->FreeList.usedPage = 0;
        FTLptr->FreeList.head = FTLptr->blocks[FTLptr->FreeList.head].FreeNext;
    }
        
}
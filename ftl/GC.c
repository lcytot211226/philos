#include "ftl.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

// Greedy
void GC(FTL *FTLptr) {
    // flush buffer
    // here spending all day on this damn bug, be aware of sequence
    if(FTLptr->writeBuf.currentPage != FTL_NULL){
        flushBuffer(FTLptr);
        FTLptr->writeBuf.currentPage = FTL_NULL;
    }

    // get victim
    byte8 victimBlock;
    getVictim(FTLptr, &victimBlock);
    // printf("%lld\n", victimBlock);
    // sleep(1);
    // move valid data
    Config *C = &(FTLptr->config);
    for(byte4 i = 0; i<C->pagesInBlock; i++) {
        if(FTLptr->blocks[victimBlock].pages[i].state == 1) {
            GCMovePage(FTLptr, &victimBlock, &i);
        }
    }

    // update victim block
    FTLptr->blocks[victimBlock].eraseCnt++;
    FTLptr->blocks[victimBlock].validPagesCnt = C->pagesInBlock;
    for(byte4 i = 0; i<C->pagesInBlock; i++) {
        FTLptr->blocks[victimBlock].pages[i].state = 0;
    }

    // join to FreeList
    FTLptr->FreeList.cnt++;
    FTLptr->blocks[FTLptr->FreeList.tail].FreeNext = victimBlock;
    FTLptr->FreeList.tail = victimBlock;

}


void getVictim(FTL *FTLptr, byte8 *victimBlock) {
    for(byte8 i=0;i<=FTLptr->config.pagesInBlock;i++) {
        if(FTLptr->GCList[i].cnt > 0) {
            *victimBlock = FTLptr->GCList[i].head;
            FTLptr->GCList[i].cnt--;
            
            // update list
            if(FTLptr->GCList[i].cnt!=0) {
                FTLptr->GCList[i].head = FTLptr->blocks[*victimBlock].GCNext;
            }
            // printf("%lld : %lld\n",i, FTLptr->blocks[*victimBlock].validPagesCnt);
            break;
        }
    }
}

void GCMovePage(FTL *FTLptr, byte4 *oldBlock, byte4 *oldPage) {
    if(FTLptr->config.isActualWrite == 1) FTLptr->recordData.actualWrite++;      
    byte8 LPageNo =  FTLptr->blocks[*oldBlock].pages[*oldPage].LPage;
    FTLptr->blocks[*oldBlock].pages[*oldPage].state = 2;

    byte4 newPageNo, newBlockNo;
    WriteNewPage(FTLptr, &newPageNo, &newBlockNo);

    FTLptr->blocks[newBlockNo].pages[newPageNo].LPage = LPageNo;
    Table *maptable = (FTLptr->mapTable);
    maptable[LPageNo].pageNo  = newPageNo;
    maptable[LPageNo].blockNo = newBlockNo;
    
}
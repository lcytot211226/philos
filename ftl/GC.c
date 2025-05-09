#include "ftl.h"
#include <stdlib.h>
#include <stdio.h>

// Greedy
void GC(FTL *FTLptr) {
    // get victim
    byte8 victimBlock;
    getVictim(FTLptr, &victimBlock);

    // update victim block
    FTLptr->blocks[victimBlock].eraseCnt++;
    FTLptr->blocks[victimBlock].validPagesCnt = FTLptr->config.pagesInBlock;

    // join to FreeList
    FTLptr->FreeList.cnt++;
    FTLptr->blocks[FTLptr->FreeList.tail].FreeNext = victimBlock;
    FTLptr->FreeList.tail = victimBlock;

}


void getVictim(FTL *FTLptr, byte8 *victimBlock) {
    for(byte8 i=0;i<FTLptr->config.pagesInBlock;i++) {
        if(FTLptr->GCList[i].cnt > 0) {
            *victimBlock = FTLptr->GCList[i].head;
            FTLptr->GCList[i].cnt--;

            // update list
            if(FTLptr->GCList[i].cnt!=0) {
                FTLptr->GCList[i].head = FTLptr->blocks[*victimBlock].GCNext;
            }
            
        }
    }
}
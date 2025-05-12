#include "ftl.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void writeSector(FTL *FTLptr, byte8 sectorNum) {
    
    Config *C = &(FTLptr->config);
    WriteBuf *wbuf = &(FTLptr->writeBuf);
    Table *maptable = (FTLptr->mapTable);

    byte8 LPageNo = sectorNum / C->sectorsInPage;
    // byte4 LSectorNo = sectorNum % C->sectorsInPage;

    // need to write buffer to new posi
    if (wbuf->currentPage != FTL_NULL && wbuf->currentPage != LPageNo) { 
        flushBuffer(FTLptr);
    }
    wbuf->currentPage = LPageNo;

}

void flushBuffer(FTL *FTLptr) {
    if(FTLptr->config.isActualWrite == 1) FTLptr->recordData.actualWrite++;
    Table *maptable = FTLptr->mapTable;
    WriteBuf *wbuf = &FTLptr->writeBuf;
    // mark old Ppage as invalid
    if(maptable[wbuf->currentPage].used == 1) {
        markPageInvalid(FTLptr, &maptable[wbuf->currentPage]);
    } 
    
    byte4 newPageNo, newBlockNo;

    // write buffer to new page
    WriteNewPage(FTLptr, &newPageNo, &newBlockNo);
    // printf("%d\n", newPageNo);
    // update mapping table
    FTLptr->blocks[newBlockNo].pages[newPageNo].LPage = wbuf->currentPage;

    maptable[wbuf->currentPage].pageNo = newPageNo;
    maptable[wbuf->currentPage].blockNo = newBlockNo;
    maptable[wbuf->currentPage].used = 1;
}

void markPageInvalid(FTL *FTLptr, Table *entry) {
    byte4 block_no = entry->blockNo;
    Block *block = &(FTLptr->blocks[block_no]);
    // assert(block->validPagesCnt != 0);
    
    block->pages[entry->pageNo].state = 2;

    // update GCList
    // exclude freelist block
    if(entry->blockNo != FTLptr->FreeList.head) {
        if(FTLptr->GCList[block->validPagesCnt].cnt > 1) {
            if       (block_no == FTLptr->GCList[block->validPagesCnt].head) {
                FTLptr->GCList[block->validPagesCnt].head = block->GCNext;

            } else if(block_no == FTLptr->GCList[block->validPagesCnt].tail) {
                FTLptr->GCList[block->validPagesCnt].tail = block->GCPrev;

            } else {
                FTLptr->blocks[block->GCNext].GCPrev = block->GCPrev;
                FTLptr->blocks[block->GCPrev].GCNext = block->GCNext;
            }
            
        }
        FTLptr->GCList[block->validPagesCnt].cnt--;
        block->validPagesCnt--;
        // add to GCList[k-1]
        if(FTLptr->GCList[block->validPagesCnt].cnt == 0) {
            FTLptr->GCList[block->validPagesCnt].head = block_no;
            FTLptr->GCList[block->validPagesCnt].tail = block_no;
        } else {
            byte4 tail_block_no = FTLptr->GCList[block->validPagesCnt].tail;
            Block *tail_block = &(FTLptr->blocks[tail_block_no]);

            block->GCPrev = tail_block_no;
            tail_block->GCNext = block_no;

            FTLptr->GCList[block->validPagesCnt].tail = block_no;
        }
        FTLptr->GCList[block->validPagesCnt].cnt++;
    } else {
        block->validPagesCnt--;
    }

}

void WriteNewPage(FTL *FTLptr, byte4 *newPageNo, byte4 *newBlockNo) {
    // choose Freelist head
    *newBlockNo = FTLptr->FreeList.head;
    *newPageNo  = FTLptr->FreeList.usedPage;
    assert(FTLptr->blocks[*newBlockNo].pages[*newPageNo].state==0);
    FTLptr->blocks[*newBlockNo].pages[*newPageNo].state = 1;
    FTLptr->FreeList.usedPage++;

    if(FTLptr->FreeList.usedPage >= FTLptr->config.pagesInBlock) {

        byte4 block_no = FTLptr->FreeList.head;
        Block *block = &(FTLptr->blocks[block_no]);

        if(FTLptr->GCList[block->validPagesCnt].cnt == 0) {
            FTLptr->GCList[block->validPagesCnt].head = block_no;
            FTLptr->GCList[block->validPagesCnt].tail = block_no;
        } else {
            byte4 tail_block_no = FTLptr->GCList[block->validPagesCnt].tail;
            Block *tail_block = &(FTLptr->blocks[tail_block_no]);

            block->GCPrev = tail_block_no;
            tail_block->GCNext = block_no;

            FTLptr->GCList[block->validPagesCnt].tail = block_no;
        }
        FTLptr->GCList[block->validPagesCnt].cnt++;

        FTLptr->FreeList.cnt--;
        FTLptr->FreeList.usedPage = 0;
        FTLptr->FreeList.head = block->FreeNext;

    }
        
}
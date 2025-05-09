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
    byte4 block_no = entry->blockNo;
    Block *block = &(FTLptr->blocks[block_no]);
    
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
    } else {
        block->validPagesCnt--;
    }

}

void WriteBufferToNewPage(FTL *FTLptr, byte4 *newPageNo, byte4 *newBlockNo) {
    // choose Freelist head
    *newBlockNo = FTLptr->FreeList.head;
    *newPageNo  = FTLptr->FreeList.usedPage;
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

        FTLptr->FreeList.cnt--;
        FTLptr->FreeList.usedPage = 0;
        FTLptr->FreeList.head = block->FreeNext;


        
    }
        
}
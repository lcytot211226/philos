#define FTL_NULL 0xffffffff
#define MAX_BUFFER_SIZE 1024
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef int64_t			byte8;
typedef uint32_t	    byte4;
typedef unsigned char	byte1;	

typedef struct { 
	// byte8	sectors;
	byte8   LPage;
	byte4	state; // 0 = free, 1 = valid, 2 = invalid
} Page;

typedef struct {
	Page	*pages;
	byte4	eraseCnt;
	/* 
		seen free and valid as same
		blocks don't exist in FreeList and GCList simultaneously
	*/
	byte4	validPagesCnt; 

	byte4	GCNext;
	byte4   GCPrev;

	byte4   FreeNext;
} Block;

typedef struct {
	byte4	head;
	byte4	tail;
	byte4	cnt;

	byte4 usedPage;
} List;

typedef struct {
	byte4	pageNo;
	byte4	blockNo;
	byte1	used;
} Table;

typedef struct {
	byte8   currentPage;
	// byte1   *usedSector;
	// byte4   usedNum;
} WriteBuf;


typedef struct {
	byte8	sectorSize;
	byte8	pageSize;
	byte8	blockSize;
	byte8	PSize;
	byte8	LSize;

	byte8   blocksInL;  // blocks in logical
	byte8   blocksInP;
	byte8   pagesInBlock;     // pages in block
	byte8   sectorsInPage;    // 

} Config;

typedef struct {
	byte8 actualWrite;
	byte8 hostWrite;
	byte8 eraseTimes; // block
} Data;

typedef struct {
	Config   config;
	Block    *blocks;
	List     *GCList;
	List     FreeList;
	Table    *mapTable;
	WriteBuf writeBuf;
	Data     recordData;
} FTL; 


void FTLinit(FTL *FTLptr,FILE *fp);
void FTLfree(FTL *FTLptr);

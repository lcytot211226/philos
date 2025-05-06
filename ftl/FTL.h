#define MLC_PAGE_FREE		0xffffffff
#define MLC_LOGBLOCK_NULL	0xffffffff
#define MLC_BLOCK_FREE		0xffffffff
#define MLC_LRU_EMPTY		0xffffffff
#define MLC_NOT_FOUNT		0xfffffffe

typedef __int64			byte16;
typedef uint32_t        byte8; 
typedef unsigned short	byte4;	
typedef unsigned char	byte1;	

typedef struct {
	byte4	state; // 0 = free, 1 = valid, 2 = invalid
    byte8   sector;
} Page;

typedef struct {
	Page	*pages;
	byte8	eraseCnt;
	byte8	validPagesCnt;
    
    byte8   GCListNext;
    byte8   GCListPrev;
} Block;

typedef struct {
    byte8   head;
    byte8   tail;
    byte8   cnt;
} List;

//////////////////////////////////////////////////////////////////////////////////////////////////

// TABLEs
//=========================
typedef struct
{
	byte8	activePageNo;
	DWORD	activeBlockNo;  //the address of large granularity or sub mapping table  
	DWORD	activeGCBlockNo;
	DWORD	activeGCPageNo;
}MLCregTableElement;

typedef struct
{
	DWORD	pageNo;
	DWORD	blockNo;  // the address of large granularity or sub mapping table 
	DWORD	regionNo;
	DWORD	pageTimeStamp;
}MLCpageTableElement;       


// CONFIG
//=========================
typedef struct
{
	DWORD	pageSizeByte;
	DWORD	pageSizeSector;
	DWORD	blockSizeByte;
	DWORD	blockSizePage;
	DWORD	blockSizeSector;
	I64		PsizeByte;
	DWORD	PsizeBlock;
	DWORD	PsizePage;
	I64		LsizeByte;
	DWORD	LsizeSector;
	DWORD	LsizePage;
	DWORD	LsizeBlock;
	DWORD	sectorSizeByte;
	DWORD	spareBlock;
	DWORD	copyPages;
}MLCconfig;


// Statiscics
//=========================
typedef struct
{
	DWORD	pageWrite;
	DWORD	pageRead;

	DWORD	userPageWrite;
	DWORD	userPageRead;

	DWORD	blockErase;
	DWORD	maxCopy;

	DWORD	reqCount;
	DWORD	*regGCBlockCount;

}MLCstat;


// MLC
//=========================
typedef struct{

	//===STRUCTUREs===================
	MLCblock	*blocks;				// Block
	MLClist		Sparelist;				// The free space we can use 
	MLClist		*GClist;				// The space which is used to GC
	MLClist		unRefList;				// 
	DWORD		systemTime;				// useless in this simulation

	//===TABLEs=======================	
	MLCpageTableElement		*mapTable;		// The table which store the mapping between Logical and physical
	MLCregTableElement	*regionTable;		// check
	DWORD					*repTable;		// to chain the log blocks. Having PsizeBlock elements
	DWORD					*predTable;	
	/*
		By using repTable(i.e. Replace Table) and Predecessor Table.
		We can find the block quickly because they use both to find the wanted block while GC without iterate all the list.
	*/

	//===SETTING & DATA===============
	MLCconfig	config;					// check
	DWORD		*regionBlockCounter;	// check
	MLCstat		stat;					// check


	//===PARAMETERS===================

	// LRU GC
	DWORD		REGION_NUMBER;
	DWORD		PROMOTE_THRESHOLD;
	DWORD		DEMOTE_THRESHOLD;

	// self-tunning stable time GC
	DWORD		STABLE_TIME_ORI;
	DWORD		STABLE_TIME;
	DWORD		STABLE_TIME_TUNNING_SCALE;
	double		OLD_COUNT_TH;

}MLC;

void MLCinit(MLC *MLCptr,FILE *fp);
void MLCfree(MLC *MLCptr);
void MLCconvertReq(MLC *MLCptr,DWORD *offsetSector,DWORD *lenSector);
void MLCwriteSector(MLC *MLCptr,DWORD offsetSector,DWORD lenSector);
void MLCinsertList(MLC *MLCptr, MLClist *list, DWORD block, int mode);
void MLCremoveList(MLC *MLCptr,MLClist *list, DWORD block, DWORD pred);
DWORD MLCfindPred(MLC *MLCptr, MLClist *list, DWORD block);
DWORD MLCgetSpareBlockGCtoGC(MLC *MLCptr,DWORD rNo);
DWORD MLCgetGCvictimGreedy(MLC *MLCptr);

// No definition of implementation
// DWORD MLCgetGCvictimCAT(MLC *MLCptr);
// DWORD MLCporTHtail(MLC *MLCptr);
// DWORD MLCgetSpareBlockGCtoLOG(MLC *MLCptr,DWORD rNo);
// DWORD MLCporTHtop(MLC *MLCptr);
// DWORD MLCgetGCvictim2G(MLC *MLCptr);
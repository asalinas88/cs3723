#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cs3723p1.h"

/***************************************
Purpose:
Initialization phase. Initializaes memory to one huge free node.

Parameters:
StorageMAnager *pMgr
MMResult *pmmResult

Returns:
N/A

Notes:
Set pMgr->pFreeHead to starting address of that huge node
 ***************************************/
void mmInit(StorageManager *pMgr)
{
	// code here

	pMgr->pFreeHead = (FreeNode *) pMgr->pBeginStorage;
	// zero out heap
	memset(pMgr->pBeginStorage, '\0', pMgr->iHeapSize);

	// set cGC to 'F'
	pMgr->pFreeHead->cGC = 'F';

	// set the shNodeSize
	pMgr->pFreeHead->shNodeSize = pMgr->iHeapSize;


}

/***************************************
  void * mmAllocate(StorageManager *pMgr, short shDataSize, short shNodeType, char sbData[], MMResult *pmmResult)

Purpose:
Usage phase. Satisfies a memory allocation request using the first
node from the free list that is big enough.

Parameters:
Storage Manager *pMgr
short shDataSize
short shNodeType
char sbData[]

Returns:
N/A
 *****************************************/

void * mmAllocate(StorageManager *pMgr, short shDataSize, short shNodeType, char sbData[], MMResult *pmmResult)
{

	// points to head of free node list
	FreeNode *tmp = pMgr->pFreeHead;
	FreeNode *parent = NULL;

	// declare an in use node
	InUseNode *inUse;

	//FreeNode *pParent = pMgr->pFreeHead;
	int minSize = pMgr->iMinimumNodeSize;

	// this is the amount we allocate
	int szAllocate = shDataSize + NODE_OVERHEAD_SZ;


        while (tmp != NULL)
        {
            //int szAllocate = shDataSize + NODE_OVERHEAD_SZ;
            if (tmp->shNodeSize >= szAllocate)
            {
                break;
            }
            parent = tmp;
            tmp = tmp->pFreeNext;
        }// end while

	if (tmp == NULL)
	{
		pmmResult->rc = RC_NOT_AVAIL;
		strcpy(pmmResult->szErrorMessage, "Null node\n");
                return NULL;
	}

	if (parent == NULL)
		pMgr->pFreeHead = tmp->pFreeNext;
	else
		parent->pFreeNext = tmp->pFreeNext;


	if ((tmp->shNodeSize - szAllocate) >= minSize)
	{
		short dataDiff = tmp->shNodeSize - szAllocate;
		FreeNode *newData = (FreeNode *)((char *)tmp + szAllocate);
		newData->cGC = 'F';
		newData->shNodeSize = dataDiff;
		newData->pFreeNext = pMgr->pFreeHead;
		pMgr->pFreeHead = newData;
	}
        else if ((tmp->shNodeSize - szAllocate) <= minSize)
        {
           szAllocate = tmp->shNodeSize;
        } // end else if

    

	inUse = (InUseNode *)tmp;
	memcpy(inUse->sbData, sbData, shDataSize);
	inUse->cGC = 'U';
	inUse->shNodeType = shNodeType;
        inUse->shNodeSize = szAllocate;

    return (InUseNode *)((char *)inUse + NODE_OVERHEAD_SZ);
    
}


/***************************************
  void mmMark(StorageManager *pMgr, MMResult *pmmResult)
Purpose:
First subphase of Garbage Collection.
Beginning w/ the start of the heap, sequentially mark every
node as 'C'

Parameters:
StorageManager *pMgr
MMResult *pmmResult

Returns:
N/A

Notes:
Look @ driver's smDump function to see how to advance
through heap touching every adjacent node
 ***************************************/
void mmMark(StorageManager *pMgr, MMResult *pmmResult){
	// code here
	char *pCh;
	short shTempSize;
	InUseNode *pAlloc;
        
	for (pCh = pMgr->pBeginStorage; pCh < pMgr->pEndStorage; pCh += shTempSize)
	{
		pAlloc = (InUseNode *)pCh;
		shTempSize = pAlloc->shNodeSize;
		pAlloc->cGC = 'C';

	}
}
/***************************************
void mmFollow(StorageManager *pMgr, void *pUserData, MMResult *pmmResult)

Purpose:
    Recuresively follow each non-null pointer referenced by particular node.

Parameters:
StorageManager *pMgr
void *pUserData
MMResult *pmmResult

Returns:
N/A

Notes:
N/A
 ***************************************/
void mmFollow(StorageManager *pMgr, void *pUserData, MMResult *pmmResult)
{
    // code here
    char *pCh;
    int iAt;
    InUseNode *inUse = (InUseNode *)((char *)pUserData - NODE_OVERHEAD_SZ);
    MetaAttr *pAttr;
    void **ppNode;


    // if current node's cGC is 'U' return
    if (pUserData == NULL || inUse->cGC == 'U')
    {
        return;
    }
    else
    {
        inUse->cGC = 'U';
        // find next by looping through meta data
        for (iAt = pMgr->nodeTypeM[inUse->shNodeType].shBeginMetaAttr; pMgr->metaAttrM[iAt].shNodeType == inUse->shNodeType; iAt++)
        {
            pAttr = &(pMgr->metaAttrM[iAt]);
            if(pAttr->cDataType == 'P')
            {
                ppNode = (void **) &(inUse->sbData[pAttr->shOffset]);
                mmFollow(pMgr, *ppNode, pmmResult); 
            } // closes if statement
        } // closes for loop
    


    }


}

/***************************************
  void mmCollect(StorageManager *pMgr, MMResult *pmmResult)

Purpose:
Traverse through heap and collect all nodes that are candidates for GC

Parameters:
StorageManager *pMgr
MMResult *pmmResult

Returns:
N/A

Notes:
N/A
 ***************************************/
void mmCollect(StorageManager *pMgr, MMResult *pmmResult)
{
    char *pCh;
    short shTempSize;
    short shTotalFreeNode;
    FreeNode *pPrev;
    FreeNode *pCurr;
  

    // set free head to null
    pMgr->pFreeHead = NULL;
    for (pCh = pMgr->pBeginStorage; pCh < pMgr->pEndStorage; pCh += shTempSize)
    {
        pCurr = (FreeNode *)pCh;
        shTempSize = pCurr->shNodeSize;
        shTotalFreeNode = 0;

        // check to see if current node equals c
        if (pCurr->cGC == 'C')
        {
            printf("\tCollecting %08lX\n", ULAddr(pCurr));
            // get total size of the free node
            shTotalFreeNode = pCurr->shNodeSize;
            pPrev = pCurr;
            // check is the adjacent node is a free node
            pCurr = (FreeNode *)((char *)pCurr + pCurr->shNodeSize);

            // while the current node is a candidate
            // and while we haven't reached the end of the heap
            while (pCurr->cGC == 'C' && pMgr->pEndStorage)
            {
                printf("\tCombining %08lX with %08lX\n", ULAddr(pPrev), ULAddr(pCurr));
               // add temp size to current node size to keep traversing through heap
                shTempSize += pCurr->shNodeSize;
                // add node size to total
                shTotalFreeNode += pCurr->shNodeSize;
                // capture next node
                pCurr = (FreeNode *)((char *)pCurr + pCurr->shNodeSize);
                
             }// end while
             // mark prev node as free
             pPrev->cGC = 'F';
             pPrev->shNodeSize = shTotalFreeNode;

             // add free node to front of list
             pPrev->pFreeNext = pMgr->pFreeHead;
             pMgr->pFreeHead = pPrev;
        }// end if
        
    }// end for loop 


}

/***************************************
  void mmAssoc(StorageManager *pMgr, void *pUserDataFrom, char szAttrName[], void *pUserDataTo, MMResult *pmmResult)

Purpose: 
Sets a user pointer in the specified user data node to a new referenced user data node

Parameters:
StorageManager *pMgr
void *pUserDataFrom
char szAttrName[]
void *pUserDataTo
MMResult *pmmResult

Returns:
N/A

Notes:
N/A
***************************************/
void mmAssoc(StorageManager *pMgr, void *pUserDataFrom, char szAttrName[], void *pUserDataTo, MMResult *pmmResult)
{
    InUseNode *inUse = (InUseNode *)((char *)pUserDataFrom - NODE_OVERHEAD_SZ);
    int iAt;           // index for metaAttrM
    char *pCh;
    void **ppNode;
    MetaAttr *pAttr;
    // variable to see if found
    bool found;

    if(pUserDataFrom == NULL || inUse->cGC != 'U')
    {
        return;
    }// end if
    else
    {
        // initialize to false
        found = false;
        for (iAt = pMgr->nodeTypeM[inUse->shNodeType].shBeginMetaAttr; pMgr->metaAttrM[iAt].shNodeType == inUse->shNodeType; iAt++)
        {
            pAttr = &(pMgr->metaAttrM[iAt]);
            if (strcmp(pAttr->szAttrName, szAttrName) == 0)
            {
                // found a match!
                found = true;
                if (pAttr->cDataType != 'P')
                {
                    pmmResult->rc = RC_ASSOC_ATTR_NOT_PTR;
                    strcpy(pmmResult->szErrorMessage, "Attribute name ");
                    strcat(pmmResult->szErrorMessage, szAttrName);
                    strcat(pmmResult->szErrorMessage, " for ASSOC not a pointer attribute.");
                    return;
                } // end if
                ppNode = (void **)&(inUse->sbData[pAttr->shOffset]);
                *ppNode = pUserDataTo;
                
            }// end if
        
         }// end for loop
         
         // if we didn't find a match
         if (!found)
         {
             pmmResult->rc = RC_ASSOC_ATTR_NOT_FOUND;
             strcpy(pmmResult->szErrorMessage, "Attribute name ");
             strcat(pmmResult->szErrorMessage, szAttrName);
             strcat(pmmResult->szErrorMessage, " for ASSOC not found for the from node.");
             return;
             
         } // end if
        
     } // end else
}

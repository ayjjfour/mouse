// EventHostObj.cpp: implementation of the EventHostObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EventHostObj.h"
#include "event_st.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
//	Interfaces
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EventHostObj::EventHostObj() : m_dwRef(0)
{
	cur=NULL;
	this->evtMap=new NODE;
	ZeroMemory(this->evtMap, sizeof(NODE));
	evtMap->nlevel=-1;
	pStandalongHandle=NULL;
//	this->ppExternalNode=NULL;
}

EventHostObj::~EventHostObj()
{
	NODE * temp;
	cur=evtMap;
	try {
		while (cur!=NULL) {
			temp=cur;
			cur=cur->pNext;
			delete temp;
		}
	}
	catch(...) {
	}
}

//////////////////////////////////////////////////////////////////////
//	IGenetic

//DEL HRESULT EventHostObj::QueryInterface(const char * lpszURI, void ** ppGenetic)
//DEL {
//DEL 	if (ppGenetic==NULL) return S_FAIL;
//DEL 
//DEL 	*ppGenetic=NULL;
//DEL 
//DEL 	if (strcmp(lpszURI, "IEventHost")==0) *ppGenetic=(void *)this;
//DEL 	else return IGeneticImpl::QueryInterface(lpszURI, ppGenetic);
//DEL 
//DEL 	AddRef();
//DEL 	return S_OK;
//DEL }

//////////////////////////////////////////////////////////////////////
//	IEventHost

STDMETHODIMP EventHostObj::attach(void * pNotify, int nlevel)
{
	ASSERT(pNotify);

//	if (pNotify==NULL) return E_INVALIDARG;

	if (nlevel<0) return E_INVALIDARG;

	NODE * node=new NODE;
	node->peh=pNotify;
	node->nlevel=nlevel;
	node->pPrev=NULL;
	node->pNext=NULL;
	node->bEnable=TRUE;

	NODE * temp=evtMap;
	//	remove the same node if existed
	while (temp) {
		if (temp->peh==pNotify) {
			__detach(temp);
			break;
		}
		temp=temp->pNext;
	}
	
	//	add the node to the proper place
	temp=evtMap;
	while (temp->nlevel>nlevel) temp=temp->pNext;

	node->pNext=temp;
	node->pPrev=temp->pPrev;
	temp->pPrev=node;
	if (node->pPrev!=NULL) node->pPrev->pNext=node;
	else evtMap=node;

	return (int)node;
}

STDMETHODIMP EventHostObj::detach(void * pNotify)
{
	ASSERT(pNotify);

	NODE * temp=evtMap;
	//	remove the same node if existed
	while (temp) {
		if (temp->peh==pNotify) {
			__detach(temp);
			return S_OK;
		}
		temp=temp->pNext;
	}

	return E_FAIL;
}

STDMETHODIMP EventHostObj::enableNotify(void * pNotify, BOOL bEnabled)
{
	ASSERT(pNotify);

	NODE * temp=evtMap;
	//	remove the same node if existed
	while (temp) {
		if (temp->peh==pNotify) {
			__enable(temp, bEnabled);
			return S_OK;
		}
		temp=temp->pNext;
	}

	return E_FAIL;
}

HRESULT EventHostObj::__detach(HANDLE hEvent)
{
	ASSERT(hEvent);

	NODE * temp=(NODE *)hEvent;
	if (temp->pPrev) temp->pPrev->pNext=temp->pNext;
	else {
		evtMap=temp->pNext;
		if (evtMap) evtMap->pPrev=NULL;
	}
	if (temp->pNext) temp->pNext->pPrev=temp->pPrev;
	
	if (pStandalongHandle==temp) pStandalongHandle=NULL;

	if (cur == temp) cur=temp->pNext;

	delete temp;
	
	return S_OK;
}

HRESULT EventHostObj::__enable(HANDLE hEvent, BOOL bEnable)
{
	if (hEvent==NULL) return E_INVALIDARG;

	NODE * temp=(NODE *)hEvent;

	temp->bEnable = bEnable;

	return S_OK;
}

//	support functions
bool EventHostObj::doNext(int status)
{
	if (FAILED(status)) return true;

	if (pStandalongHandle!=NULL) {
		if (status&S_RELEASECAPTURE) {
			pStandalongHandle=NULL;
		}
		return false;
	}

	if (status&S_CAPTURE) {						//capture  subsqence events
		if (pStandalongHandle==NULL && cur!=NULL) pStandalongHandle=cur;
	}
	if (status&S_CANCELPROCESS) {			//cancel the process link
		if (status&S_PROCESSED) {
			if (cur!=NULL && cur->pPrev!=NULL && cur->nlevel>=cur->pPrev->nlevel) {		//put the current node to higher level
				NODE * temp=cur->pPrev;
				temp->pNext=cur->pNext;
				cur->pNext=temp;
				cur->pPrev=temp->pPrev;
				temp->pPrev=cur;
				if (cur->pPrev!=NULL) cur->pPrev->pNext=cur;
				else evtMap=cur;
				if (temp->pNext!=NULL) temp->pNext->pPrev=temp;
			}
		}
		return false;
	}

	return true;
}

LPUNKNOWN EventHostObj::BeginEnum()
{
/*	if (pStandalongHandle!=NULL) {
		cur=NULL;
		return (LPIGenetic)pStandalongHandle;
	}
	else {
		cur=(*ppExternalNode==NULL? evtMap: *ppExternalNode);
		return (LPIGenetic)cur->peh;
	}*/

	NODE *  temp=(NODE *)BeginEnumNode();
	
	if (temp) return (LPUNKNOWN)temp->peh;
	else return NULL;
}

LPUNKNOWN EventHostObj::EnumNext()
{
/*	if (pStandalongHandle!=NULL && cur==NULL) return NULL;

	if (cur==NULL) cur=(*ppExternalNode==NULL? evtMap: *ppExternalNode);
	else cur=cur->pNext;

	if (cur!=NULL) return (LPIGenetic)cur->peh;
	else return NULL;*/

	NODE * temp=(NODE *)EnumNextNode();

	if (temp) return (LPUNKNOWN)temp->peh;
	else return NULL;
}

//DEL int EventHostObj::SetHandle(void **ppEvtMap)
//DEL {
//DEL 	ppExternalNode=(NODE **)ppEvtMap;
//DEL 
//DEL 	return S_OK;
//DEL }

void * EventHostObj::BeginEnumNode()
{
	if (pStandalongHandle!=NULL) {
		cur=NULL;
		if (pStandalongHandle->bEnable) return pStandalongHandle;
		else return NULL;
	}
	else {
		cur = evtMap;
		while (cur) {
			if (!(cur->bEnable)) cur=cur->pNext;
			else break;
		}

		return cur;
	}
}

void * EventHostObj::EnumNextNode()
{
	if (pStandalongHandle!=NULL && cur==NULL) return NULL;

	if (cur==NULL) return NULL;//cur=(ppExternalNode==NULL? evtMap: *ppExternalNode);
	else cur=cur->pNext;

	while (cur) {
		if (!cur->bEnable) cur=cur->pNext;
		else break;
	}

	return cur;
}

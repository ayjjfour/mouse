#ifndef __UTILITY_H__
#define __UTILITY_H__

#ifndef _DEBUG
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#else
#define SAFE_RELEASE(p)      { if(p) { try {(p)->Release();}catch(...){}; (p)=NULL; } }
#endif		//_DEBUG

#define SAFE_FREELIB(lib)	 { if(lib) { ::FreeLibrary(lib); lib=NULL; } }
#define SAFE_DELETE(p)		 { if(p) { delete (p); (p)=NULL; } }
#endif

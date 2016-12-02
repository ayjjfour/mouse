#ifndef _ICANVAS_H_
#define _ICANVAS_H_

#include "ITrace.h"
///	Blit ��ʽ
/////////////////////////

#define BLIT_NORMAL	0				//������BLIT��������͸��ɫ��Alpha
#define BLIT_COPY	0				//ͬ��
#define BLIT_ADDITIVE	0x1			//ɫ�ʵ���
#define BLIT_SUBTRACT	0x2			//ɫ��˥��
#define BLIT_TRANS		0x3			//alpha BLIT
#define BLIT_MASK		0x4			//͸��ɫBLIT
#define BLIT_BACK		0x5			//����COPY ��Ŀ�굽Դ
#define BLIT_XOR		0x4			

#define FONT_ALPHA		0
#define FONT_NOALPHA	1
//////////////////////////

#pragma pack (push)
#pragma pack (1)

//16λ��ṹ
struct WPixel {
	union {
		WORD color;								// 16bit �߲�
		struct {
			WORD blue:5;						// 5bit ��ɫ
			WORD green:6;						// 6bit ��ɫ
			WORD red:5;							// 5bit ��ɫ
		};
	};
	WPixel() {}
	WPixel(DWORD c) {color=(WORD)((c>>8&0xf800)|(c>>5&0x7e0)|(c>>3&0x1f));}	//��DWORDת��
	WPixel(int r,int g,int b) : red(r>>3),green(g>>2),blue(b>>3) {}
	operator DWORD() const {return (color<<5&0x7fc00)|(color<<8&0xf80000)|((color<<3|(color&7))&0x3ff);}
};

#pragma pack (pop)


struct ITTFont;
struct IAlphaBitmap;

struct IBitmap
{
	//��ʼ������
	virtual BOOL Create(int nWidth, int nHeight) = NULL;		//����λͼ
	virtual BOOL Create(int nWidth, int nHeight, void* ptr) = NULL;	//��������������λͼ
	virtual BOOL Create(IBitmap* parent, int x, int y, int nWidth, int nHeight) = NULL; //�Ӹ�λͼ�ϴ�������
	virtual IBitmap* SubBitmap(int x, int y, int nWidth, int nHeight) = NULL;	//������λͼ

	virtual int GetWidth() const = NULL;		//��ÿ��
	virtual int GetHeight() const = NULL;		//��ø߶�

	//��ͼ���
	virtual BOOL Clear(WPixel clr, RECT* pRc = NULL) = NULL;	//����
	virtual BOOL Draw(int dx, int dy, IBitmap* bmp, DWORD dwStyle = BLIT_NORMAL) = NULL;//�ѱ��λͼ���Ƶ���λͼ
	virtual BOOL Draw(int dx, int dy, int w, int h, IBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_NORMAL) = NULL; //��ǿ��Draw
	virtual BOOL DrawAlpha(int dx, int dy, IAlphaBitmap* bmp, DWORD dwStyle = BLIT_COPY) = NULL;//�ѱ��λͼ���Ƶ���λͼ
	virtual BOOL DrawAlpha(int dx, int dy, int w, int h, IAlphaBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_COPY) = NULL; //��ǿ��AlphaDraw
	
	virtual void Draw(int x, int y, const char* s, WPixel clr) = NULL; //��ӡASCII�ַ�
	virtual BOOL DrawBox(RECT& rc, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255) = NULL;
	virtual BOOL DrawLine(POINT& ptFrom, POINT& ptTo, WPixel clr, DWORD dwStyle = BLIT_NORMAL, int alpha = 255) = NULL;

	virtual BOOL Blit2DC(HDC hDC,int dx,int dy,int x,int y,int w,int h)  = NULL;	// BLIT һ�����β��ֵ� DC
	//����
	virtual WPixel* operator[](int y)  const = NULL;		// ����һ��ɨ����ָ��
	virtual void* GetBmpStruct() const = NULL;		//��������
	
	
	virtual BOOL Load(void *bmpdata) = NULL;				// ������������λͼ
	virtual BOOL Save(const char *filename) const = NULL;	//����λͼ

	virtual void SetUserData(DWORD userdata) = NULL;				// �����û�����
	virtual DWORD GetUserData() const = NULL;		//�õ��û�����

	
	virtual BOOL IsValid() const = NULL;	//�Ƿ�Ϊ��Ч
	virtual void Release() = NULL;
	virtual void SetData(void*p) = NULL;	//�����ڲ�������

	virtual BOOL Scroll(int x, int y) = NULL;	//����Bitmap����
	virtual BOOL DrawText(ITTFont*,int x, int y, const char* str, WPixel color, DWORD style=FONT_ALPHA) = NULL;	//���TTF����
	virtual BOOL IsTransparent(POINT& ptSelect) = NULL; // ����ĳ�����Ƿ�͸����add by PeakGao in 2002.10.22
	//�ӱ�λͼ�ϴ���һ��������Tile��λͼ(�����θ�λͼ, ptLeftTopΪ���θ���󶥵�)
	virtual IBitmap* SubTile(POINT ptLeftTop) = NULL;
	virtual BOOL LoadEx(void* customdata) = NULL;
};

struct IAlphaBitmap : public IBitmap
{
	virtual BOOL LoadTGA(const void *tgadata) = NULL;		//����TGAͼƬ
	virtual BOOL SaveTGA(const char *filename) const = NULL;	//��Ϊ32λTGA
	virtual BYTE* ChannelLine(int y) const = NULL;				//�õ�ͨ��ָ��
	virtual void ClearChannel(int alpha) = NULL;				// ��� Alpha Channel
	virtual BOOL Combine(IAlphaBitmap* bmp) = NULL;				//�ϲ�����AlphaBitmap
	virtual BOOL Load2Bmp(const void* bmpdata, const void* alphadata) = NULL;	//�Ӷ���Bitmap������AlphaBitmap����һ����ͨ��ͼ��ֻ��Ϊ256ɫ
	virtual BOOL Load2BmpEx(IBitmap* pBmp, int nBmpX, int nBmpY,
							IBitmap* pAlpha, int nAlphaX, int nAlphaY,
							int nWidth, int nHeight) = NULL; // ������IBitmap������AlphaBitmap����һ����ͨ��ͼ��ֻ��Ϊ256ɫ	
};

struct ICanvas : public IBitmap
{
	//��IBitmap�Ľ���
	virtual IBitmap* SelectBitmap(IBitmap* pBitmap) = NULL;		//ѡ��һ��IBitmap
	virtual IBitmap* GetBitmap(void) const = NULL;
	
	//��Canvas�Ľ���
	virtual BOOL Draw(int x, int y, ICanvas* pCanvas, DWORD dwStyle = BLIT_NORMAL) = NULL;	//�ѱ��λͼ���Ƶ���λͼ
	virtual BOOL Draw(int x, int y, IBitmap* bmp, DWORD dwStyle = BLIT_NORMAL) = NULL;	//�ѱ��λͼ���Ƶ���λͼ
	virtual BOOL Draw(int dx, int dy, int w, int h, IBitmap* bmp, int sx, int sy, DWORD dwStyle = BLIT_NORMAL) = NULL; //��ǿ��Draw
	virtual BOOL Draw(int dx, int dy, int w, int h, ICanvas* bmp, int sx, int sy, DWORD dwStyle = BLIT_NORMAL) = NULL; //��ǿ��Draw
	virtual BOOL Update2DC(HDC hDC,int dx,int dy,int x,int y,int w,int h) = NULL;	// BLIT һ�����β��ֵ� DC
	virtual void Draw(int x, int y, const char* s, WPixel clr) = NULL; //��ӡASCII�ַ�

	//������븲�Ǿ��ι���
	//������ι���
	virtual void AddDirtyRect(RECT& area) = NULL;
	virtual void AddDirtyRect(int x, int y, int w, int h) = NULL;

	virtual void AddInOutRect(RECT& area) = NULL;
	
	virtual void AddInOutWholeScreen() = NULL;
	virtual void AddInOutOverlay(RECT& area) = NULL;

	virtual void AddOverlay(RECT& area) = NULL;
	virtual void ResetDirtyRect(void) = NULL;		//�����о�����0
	virtual void AddWholeScreen(void) = NULL;
	virtual void Merge(void) = NULL;

	virtual void* GetDirtyList() = NULL;
	virtual void AddDirtyList(void* pList) = NULL;
	virtual BOOL IsInDirty(RECT& rc) = NULL;


	//������ι���
	virtual void AddOutDirtyRect(RECT& area) = NULL;
	virtual void AddOutDirtyRect(int x, int y, int w, int h) = NULL;
	virtual void AddOutOverlay(RECT& area) = NULL;
	virtual void ResetOutDirtyRect(void) = NULL;		//�����о����� 0
	virtual void AddOutWholeScreen(void) = NULL;
	virtual void MergeOut(void) = NULL;

	virtual void* GetOutDirtyList() = NULL;
	virtual void AddOutDirtyList(void* pList) = NULL;

	virtual BOOL IsInEqualOut() = NULL;			//��������Ƿ������������
	virtual void SetInEqualOut(BOOL) = NULL;	
	virtual BOOL IsOutDirty(RECT& rc) = NULL;	
	
	virtual void DrawDirtyList(ICanvas* pICanvas,WPixel color) = NULL;
	virtual void DrawOutDirtyList(ICanvas *pICanvas, WPixel color) = NULL;

};

struct ITTFont
{
	virtual BOOL Create(HFONT font, int alpha = 16, int cachesize = 128) = NULL;
	virtual void* GetChar(unsigned int c) = NULL;	// ��ȡһ���ֵ�����

	virtual int GetHeight() const = NULL;		// ��ȡ����߶�
	
	virtual void Lock() = NULL;				// Lock Cache Buffer (����ɾ��)
	virtual void UnLock() = NULL;			// Unlock Cache Buffer

	virtual DWORD GetLength(const char* str) = NULL;	//�õ��ַ����ĳ���
	virtual DWORD GetCharLength(const WORD c) = NULL;	//�õ�һ���ֵĳ���
	

	virtual void Release() = NULL;
	
};

/*
#ifdef CANVAS_EXPORTS

extern "C" __declspec(dllexport) BOOL CreateIBitmap(DWORD dwVersion, IBitmap** ppIBitmap, ITrace* pTrace = NULL);
extern "C" __declspec(dllexport) BOOL CreateICanvas(DWORD dwVersion, ICanvas** ppICanvas, ITrace* pTrace = NULL);

#else

extern "C" __declspec(dllimport) BOOL CreateIBitmap(DWORD dwVersion, IBitmap** ppIBitmap, ITrace* pTrace = NULL);
extern "C" __declspec(dllimport) BOOL CreateICanvas(DWORD dwVersion, ICanvas** ppICanvas, ITrace* pTrace = NULL);

#endif //CANVAS_EXPORTS
*/
class ICanvasHelper
{
	typedef BOOL (*ProcCreateIBitmap)(DWORD dwVersion, IBitmap** ppIBitmap, ITrace* pTrace = NULL);
	typedef BOOL (*ProcCreateICanvas)(DWORD dwVersion, ICanvas** ppICanvas, ITrace* pTrace = NULL);
	typedef BOOL (*ProcCreateITTFont)(DWORD dwVersion, ITTFont** ppITTFont, ITrace* pTrace = NULL);
	typedef BOOL (*ProcCreateIAlphaBitmap)(DWORD dwVersion, IAlphaBitmap** ppIBitmap, ITrace* pTrace = NULL);
public:
	
	ICanvasHelper() : m_hdll(0){}
	~ICanvasHelper()
	{
		if(m_hdll)
			FreeLibrary(m_hdll);
	}

	void LoadDLL()
	{
#ifdef _DEBUG
		m_hdll = LoadLibrary("Canvasd.dll");
#else
		m_hdll = LoadLibrary("Canvas.dll");
#endif
		
		if(m_hdll == NULL)
			throw "Can not load Canvas.dll";
	}

	BOOL CreateIBitmap(IBitmap** ppIBitmap, ITrace* pTrace = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				LoadDLL();
			}

			ProcCreateIBitmap proc;
			proc = (ProcCreateIBitmap)GetProcAddress(m_hdll, "CreateIBitmap");

			if(proc == NULL)
				throw "Can not Get CreateIBitmap!";

			return proc(1, ppIBitmap, pTrace);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			return FALSE;
		}

		catch(...)
		{
			return FALSE;
		}
	}

	BOOL CreateIAlphaBitmap(IAlphaBitmap** ppIBitmap, ITrace* pTrace = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				LoadDLL();
			}

			ProcCreateIAlphaBitmap proc;
			proc = (ProcCreateIAlphaBitmap)GetProcAddress(m_hdll, "CreateIAlphaBitmap");

			if(proc == NULL)
				throw "Can not Get CreateIBitmap!";

			return proc(1, ppIBitmap, pTrace);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			return FALSE;
		}

		catch(...)
		{
			return FALSE;
		}
	}

	BOOL CreateICanvas(ICanvas** ppICanvas, ITrace* pTrace = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				LoadDLL();
			}

			ProcCreateICanvas proc;
			proc = (ProcCreateICanvas)GetProcAddress(m_hdll, "CreateICanvas");

			if(proc == NULL)
				throw "Can not Get CreateICanvas!";

			return proc(1, ppICanvas, pTrace);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			return FALSE;
		}

		catch(...)
		{
			return FALSE;
		}
	}

	BOOL CreateITTFont(ITTFont** ppITTFont, ITrace* pTrace = NULL)
	{
		try
		{
			if(m_hdll == NULL)
			{
				LoadDLL();
			}

			ProcCreateITTFont proc;
			proc = (ProcCreateITTFont)GetProcAddress(m_hdll, "CreateITTFont");

			if(proc == NULL)
				throw "Can not Get CreateITTFont!";

			return proc(1, ppITTFont, pTrace);
		}
		catch(LPCSTR szMsg)
		{
			OutputDebugString(szMsg);
			return FALSE;
		}

		catch(...)
		{
			return FALSE;
		}
	}

private:
	HINSTANCE m_hdll;

};


////////////////////////////////////////////////////////////////////////////
#endif //_ICANVAS_H

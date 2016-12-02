#ifndef __HTMLENCODE_H__
#define __HTMLENCODE_H__

//该函数将传入字符串中的<转成&lt; >转成&gt;
//返回字符串将永远指向一个内部存储区

static char _htmlEncode_ret_buf[1024];

inline char * htmlEncode(const char * lpSrc) {
	ASSERT(lpSrc);

	int nLength =strlen(lpSrc);
	if (nLength>=sizeof(_htmlEncode_ret_buf)) return NULL;

	int offsetInRetBuf=0;
	for (int i=0; i<nLength; i++) {
		if (lpSrc[i]=='<') {
			if (offsetInRetBuf+4>=sizeof(_htmlEncode_ret_buf)) return NULL;
			strcpy(&_htmlEncode_ret_buf[offsetInRetBuf], "&lt;");
			offsetInRetBuf+=4;
		}
		else if (lpSrc[i]=='>') {
			if (offsetInRetBuf+4>=sizeof(_htmlEncode_ret_buf)) return NULL;
			strcpy(&_htmlEncode_ret_buf[offsetInRetBuf], "&gt;");
			offsetInRetBuf+=4;
		}
		else {
			if (offsetInRetBuf+1>=sizeof(_htmlEncode_ret_buf)) return NULL;
			_htmlEncode_ret_buf[offsetInRetBuf] =lpSrc[i];
			offsetInRetBuf++;
		}
	}

	_htmlEncode_ret_buf[offsetInRetBuf] ='\0';

	return _htmlEncode_ret_buf;
};

#endif //__HTMLENCODE_H__

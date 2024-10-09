/*
 * FILE: EncStr.h
 *
 * DESCRIPTION: cooperate with the pwpacker to remove the information text in the rdata section of the PE file
 *
 * CREATED BY: linzhehui, 2009
 *
 * HISTORY: Last updated 2009/05/22, use Micro and Class to facilitate the Calling replacement.
 * HISTORY: LZH  updated 2009/06/01, replace LPSTR with LPCSTR, LPWSTR with LPCWSTR to Generate compile errors when using in dangerous ways.
 */

// �����ַ���������غ�ʹ��˵��
// ��;����ϼӿǻ����ⲿ�����ߣ������ڱ����Ķ����ƴ����г���ָʾ�������ı�(ͨ����ֻ�����ݶ�)����Ӱ������������֣��Ӵ������Է����Ѷȡ�
// ���ܣ������ж�����ܻ�ԭ���룬�����ַ�����ԭ����΢���Ӷ���CPU,�����㷨���úܸ��ӿ����𵽺ܺñ���Ч����CPU�����������Ժ��Բ���
// ���룺���ֱ�����÷�ʽ, _A Ϊansi code, _W ΪUnicode, _T �����UNICODE ������ݵĵ��÷�ʽ������TCHAR���Լ����ǳ������� _AL����ֵĵط�
// ���ã�
// 1. ��Ϊ��������ֱ�ӵ��á������к������ó����ַ��������г��Ͼ�����ʹ�á�
//    PARAM_ENCSTR_A(const CHAR*) PARAM_ENCSTR_W(const WCHAR*) PARAM_ENCSTR_T(const TCHAR*) ������Ϊ����������
//    �����ַ�����Ķ��岿��,����ʵ�ʵ��ù��캯��,��˿���ֱ��ʹ�ô���ӿ�.
// 2. ��Ϊ�ֲ���ȫ�ֱ�������,������Ϊ����������.
//    VAR_ENCSTR_A(xxx,"abcd"); �滻  char *xxx="abcd";
//    VAR_ENCSTR_W(xxx,"abcd"); �滻 wchar *xxx="abcd";
//    VAR_ENCSTR_T(xxx,"abcd"); �滻 TCHAR *xxx="abcd";  ����ʹ��ֱ���ñ��� xxx
//    ע�⣬�˴�����Ǿֲ���������ָ�����ÿռ�ֻ���ھֲ�������������Χ�������Ҫȫ��ʹ�ã�����ȫ�ֱ�����STATIC_VAR��ʽ
// 3. ��̬����.������ͬȫ�ֺ���
//    STATIC_VAR_ENCSTR_A(xxx,"abcd"); �滻  const static char  *xxx="abcd"; 
// 4. const ������⣬�����ַ�������������������Ϊ����ʹ�ã�����Ϊ�˵���ʱ�ķ������ͬʱ�ṩ LPSTR��LPCSTR ���ֲ�����ʽ�������������ܻ���Բ���WARNING��


#ifndef _ENCSTR_H_
#define _ENCSTR_H_


#define __ENC_STR_START      "S*4&"
#define __ENC_STR_END        "E*4&"
#define __ENC_STR_STARTX     "S*4&\0\1\2\3"
#define __ENC_STR_ENDX       "E*4&\0\1\2\3"  
#define __UENC_STR_START     L"S*4&"
#define __UENC_STR_END       L"E*4&"

#define __UENC_STR_STARTX    L"S*4&\0\1\2\3"
#define __UENC_STR_ENDX      L"E*4&\0\1\2\3"

#define __ATTACH_END_STR(X) X __ENC_STR_END 
#define __ENC_STR(X)   __ENC_STR_START __ATTACH_END_STR(X)

#define __UATTACH_END_STR(X) X __UENC_STR_END 
#define __UENC_STR(X)   __UENC_STR_START __UATTACH_END_STR(X)
#define MAX_FIND_LEN 1024


#include <time.h>
#include <Windows.h>

BOOL   EncryptStr(char* pStr);
char*  DecryptStr(const char* pStr);
void   FreeDecryptStr(char* NewStr);
BOOL   EncryptStrW(WCHAR* pStr);
WCHAR* DecryptStrW(const WCHAR* pStr);
void   FreeDecryptStrW(WCHAR* NewStr);


//class XEncryptStr
//{
//public:
//	XEncryptStr(const CHAR* str)
//	{
//		m_str = DecryptStr(str);
//		m_wcs = NULL;
//	}
//	XEncryptStr(const WCHAR* wcs)
//	{
//		m_wcs = DecryptStrW(wcs);
//		m_str = NULL;
//	}
//	virtual ~XEncryptStr()
//	{
//		if(m_str)
//			FreeDecryptStr(m_str);
//		if(m_wcs)
//			FreeDecryptStrW(m_wcs);
//	}
//	operator LPSTR()
//	{
//		return m_str;
//	}
//	operator LPWSTR()
//	{
//		return m_wcs;
//	}
//private:
//	CHAR  *m_str;
//	WCHAR *m_wcs;
// };


class XEncryptStrA
{
public:
	XEncryptStrA(const CHAR* str)
	{
		m_str = NULL;
		m_str = DecryptStr(str);
	}
	virtual ~XEncryptStrA()
	{
		if(m_str)
			FreeDecryptStr(m_str);
		m_str = NULL;
	}
	/*
	operator LPSTR()
	{
		return m_str;
	}
	*/
	operator LPCSTR()
	{
		return (const CHAR*)m_str;
	}
	XEncryptStrA& operator =(const CHAR* str)
	{
		if(m_str)
		{
			FreeDecryptStr(m_str);
			m_str = NULL;
		}
		m_str = DecryptStr(str);
		return *this;
	}
	/*
	operator =(CHAR* str)
	{
		if(m_str)
		{
			FreeDecryptStr(m_str);
			m_str = NULL;
		}
		m_str = DecryptStr(str);
	}
	*/

private:
	CHAR  *m_str;
};

class XEncryptStrW
{
public:
	XEncryptStrW(const WCHAR* wcs)
	{
		m_wcs = NULL;
		m_wcs = DecryptStrW(wcs);
	}
	virtual ~XEncryptStrW()
	{
		if(m_wcs)
			FreeDecryptStrW(m_wcs);
		m_wcs = NULL;
	}
	/*
	operator LPWSTR()
	{
		return m_wcs;
	}
	*/
	operator LPCWSTR()
	{
		return (const WCHAR*)m_wcs;
	}
	XEncryptStrW& operator =(const WCHAR* wcs)
	{
		if(m_wcs)
		{
			FreeDecryptStrW(m_wcs);
			m_wcs = NULL;
		}
		m_wcs = DecryptStrW(wcs);
		return *this;
	}
	/*
	operator =(WCHAR* wcs)
	{
		if(m_wcs)
		{
			FreeDecryptStrW(m_wcs);
			m_wcs = NULL;
		}
		m_wcs = DecryptStrW(wcs);
	}
	*/
private:
	WCHAR  *m_wcs;
};


#if defined(_ENCRYPTION_STRING)
	// encrypt ansi string.
	#define _EA(a)   LPCSTR(XEncryptStrA(__ENC_STR(a)))
	// encrypt unicode string.
	#define _EW(a)   LPCWSTR(XEncryptStrW(__UENC_STR(L##a)))
#else
	// encrypt ansi string.
	#define _EA(a)   LPCSTR(a)	
	// encrypt unicode string.	
	#define _EW(a)   LPCWSTR(L##a)
#endif

#ifdef UNICODE
// encrypt ansi or unicode string.
#define _ET _EW	
#else
// encrypt ansi or unicode string.
#define _ET _EA 
#endif


//////////////////////////////////////////////////////////////////////////
//       ������õĺ�ӿڲ���, �����޸��ϴ���
//////////////////////////////////////////////////////////////////////////

//#define PARAM_ENCSTR_A(a)    LPCSTR(XEncryptStrA( __ENC_STR(a)))
//#define PARAM_ENCSTR_W(a)   LPCWSTR(XEncryptStrW(__UENC_STR(a)))
//
//#ifdef UNICODE
//#define PARAM_ENCSTR_T PARAM_ENCSTR_W
//#else
//#define PARAM_ENCSTR_T PARAM_ENCSTR_A
// #endif

//#define VAR_ENCSTR_A(a,b) XEncryptStrA  a( __ENC_STR(b))
//#define VAR_ENCSTR_W(a,b) XEncryptStrW  a(__UENC_STR(b))
//
//#ifdef UNICODE
//#define VAR_ENCSTR_T VAR_ENCSTR_W
//#else
//#define VAR_ENCSTR_T VAR_ENCSTR_A
//#endif
//
//#define STATIC_VAR_ENCSTR_A(a,b) static XEncryptStrA  a(__ENC_STR(b))
//#define STATIC_VAR_ENCSTR_W(a,b) static XEncryptStrW a(__UENC_STR(b))
//
//#ifdef UNICODE
//#define STATIC_VAR_ENCSTR_T STATIC_VAR_ENCSTR_W
//#else
//#define STATIC_VAR_ENCSTR_T STATIC_VAR_ENCSTR_A
//#endif
//
////////////////////////////////////////////////////////////////////////////
////        �ַ�������ӿڲ���,��ֱ�ӱ�������,����ֱ�Ӷ�����ַ��������޸�
////////////////////////////////////////////////////////////////////////////
//#ifdef UNICODE
//#define XEncryptStrT XEncryptStrW
//#define ENC_STR_T    ENC_STR_W
//#else
//#define XEncryptStrT XEncryptStrA
//#define ENC_STR_T    ENC_STR_A
//#endif
//
///*
//    �ϴ���
//	char *buf[2] ={"abcd", "cdef")}
//	int c =0;
//	c= c*c;
//	MessageBox(buf[0],buf[1]),MB_OK);
//
//	�¼��ܴ���
//	XEncryptStrA buf[2] ={ENC_STR_A("abcd"), ENC_STR_A("cdef") }
//	int c =0;
//	c= c*c;
//	MessageBox(buf[0],buf[1],MB_OK);
//
//*/
//
//
//
//#define ENC_STR_A     __ENC_STR
//#define ENC_STR_W     __UENC_STR
//#define POINTER_ENCSTR_A(a)    LPSTR(XEncryptStrA(a))
//#define POINTER_ENCSTR_W(a)   LPWSTR(XEncryptStrW(a))
//
//#ifdef UNICODE
//#define ENC_STR_T ENC_STR_W
//#define POINTER_ENCSTR_T POINTER_ENCSTR_W
//#else
//#define ENC_STR_T ENC_STR_A
//#define POINTER_ENCSTR_T POINTER_ENCSTR_A
//#endif
//
//
//
//#endif
//
//
//
//
////////////////////////////////////////////////////////////////////////////
////                        �����ϰ汾����
////////////////////////////////////////////////////////////////////////////
////
////
//// 	һ��һ��ʹ�����������ʱ���ַ���ǰ���� NewEncStrT (NewEncStrA, NewEncStrW)������ʱ��������ı�׼��ʽ��
//// 	TCHAR *x = "Demo";
//// 	-------------------
//// 	TCHAR *x = NewEncStrT("Demo");
//// 	... //other code
//// 	DeleteEncStrT(x);
//// 	�������ö��κ���
//// 	�ο�Xglb_ErrorOutput �Ķ��巽ʽֱ�Ӷ�����غ���������꣬Ȼ����õ�ʱ��ֱ�ӽ��������滻�ɺ����ơ�
//// 	������κ���(VC6 ��֧�ֱ�κ�)
//// 	1. �ο�Xa_LogOutput �����Ķ��壬����ʱ �� CHAR* p=NewEncStrA(szMsg); Xa_LogOutput(ilevel,p,...);
//// 	2. ���ݲ�������д���ɸ�����꣬����ʱ���ݲ��������滻��ͬ�ĺ����ơ�  Xa_LogOutput_P02  ��  Xa_LogOutput_P10
//// 	�ġ��ַ�����ĳ��Ի�������ֵ������ CopyAssignMentA��CopyAssignMenW��CopyAssignMentT ���
//
//
//
////void   Xa_LogOutput(int iLevel, const char* szMsg, ...); //Xa_LogOutput(ilevel,,...);
////���������ʹ�÷�ʽ��, ��Ҫ�Լ��ڲ�ʹ��ʱ�ͷ��ڴ�
//
//#define NewEncStrA(a)    DecryptStr(__ENC_STR(a))
//#define DeleteEncStrA(a) FreeDecryptStr(a)
//
//#define NewEncStrW(a)	 DecryptStrW(__UENC_STR(a))
//#define DeleteEncStrW(a) FreeDecryptStrW(a) 
//
//// ֧��UNICODE �궨��ķ�ʽ
//#ifdef  UNICODE
//#define NewEncStrT(a)    NewEncStrW(a)
//#define DeleteEncStrT(a) DeleteEncStrW(a)
//#else
//#define NewEncStrT(a)    NewEncStrA(a)
//#define DeleteEncStrT(a) DeleteEncStrA(a)
//#endif
//
///*
//// ��ֵ�������ţ����������� CString ��ĳ�ʼ��
//#define CopyAssignMentA(a,b) {\
//	CHAR *p1 = NewEncStrA(b); \
//	a = p1; \
//	DeleteEncStrA(p1); \
//} \
//
//#define CopyAssignMentW(a,b) {\
//	WCHAR *p1 = NewEncStrW(b); \
//	a = p1; \
//	DeleteEncStrW(p1); \
//} \
//
//#define CopyAssignMentT(a,b) {\
//	TCHAR *p1 = NewEncStrT(b); \
//	a = p1; \
//	DeleteEncStrT(p1); \
//} \
//
//
//// �޸���һЩ�꣬�����޸�ԭ�е�һЩ��������
//#define XRuntimeDebugInfoW_P03(a,b,c) { \
//	WCHAR *p1 = NewEncStrW(c); \
//	if(p1!=NULL) \
//	{ \
//	a->RuntimeDebugInfo(b, p1); \
//	DeleteEncStrW(p1); \
//	} \
//else{}\
//} \
//
//#define XRuntimeDebugInfoA_P03(a,b,c) { \
//	CHAR *p1 = NewEncStrA(c); \
//	if(p1!=NULL) \
//	{ \
//	a->RuntimeDebugInfo(b, p1); \
//	DeleteEncStrW(p1); \
//	} \
//else{}\
//} \
//
//
//#define XRuntimeDebugInfoW_P04(a,b,c,d) { \
//	WCHAR *p1 = NewEncStrW(c); \
//	if(p1!=NULL) \
//	{ \
//	a->RuntimeDebugInfo(b, p1, d); \
//	DeleteEncStrW(p1); \
//	} \
//else{}\
//} \
//
//#define XRuntimeDebugInfoA_P04(a,b,c,d) { \
//	CHAR *p1 = NewEncStrA(c); \
//	if(p1!=NULL) \
//	{ \
//	a->RuntimeDebugInfo(b, p1, d); \
//	DeleteEncStrW(p1); \
//	} \
//else{}\
//} \
//
////g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("NET - Server disconnected"));
//#ifdef  UNICODE
//#define XRuntimeDebugInfoT_P03    XRuntimeDebugInfoW_P03
//#define XRuntimeDebugInfoT_P04    XRuntimeDebugInfoW_P04
//#else
//#define XRuntimeDebugInfoT_P03    XRuntimeDebugInfoA_P03
//#define XRuntimeDebugInfoT_P04    XRuntimeDebugInfoA_P04
//#endif
//*/
//
//
//#define XOutputDebugStringW(a){ \
//	WCHAR *p1 = NewEncStrW(a); \
//	OutputDebugStringW(a); \
//	DeleteEncStrW(p1); \
//} \
//
//#define XOutputDebugStringA(a){ \
//	CHAR *p1 = NewEncStrA(a); \
//	OutputDebugString(a); \
//	DeleteEncStrA(p1); \
//} \
//
//#ifdef  UNICODE
//#define XOutputDebugStringT(a)    XOutputDebugStringW(a)
//#else
//#define XOutputDebugStringT(a)    XOutputDebugStringA(a
//#endif
//
//
//
//#define Xglb_ErrorOutput(a,b,c) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//	{ \
//		glb_ErrorOutput(a, p1, c); \
//		DeleteEncStrA(p1); \
//	} \
//	else{}\
//} \
//
//#define Xa_LogOutput(a,b) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//	{ \
//		a_LogOutput(a, p1); \
//		DeleteEncStrA(p1); \
//	} \
//	else{}\
//} \
//	
//#define Xa_LogOutput_P02(a,b) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//	{ \
//		a_LogOutput(a, p1); \
//		DeleteEncStrA(p1); \
//	} \
//	else{}\
//} \
//  
//#define Xa_LogOutput_P03(a,b,c) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//	{ \
//	a_LogOutput(a, p1,c); \
//	DeleteEncStrA(p1); \
//	} \
//	else{}\
//} \
//
//#define Xa_LogOutput_P04(a,b,c,d) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//{ \
//	a_LogOutput(a, p1,c,d); \
//	DeleteEncStrA(p1); \
//} \
//else{}\
//} \
//
//#define Xa_LogOutput_P05(a,b,c,d,e) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//{ \
//	a_LogOutput(a, p1,c,d,e); \
//	DeleteEncStrA(p1); \
//} \
//else{}\
//} \
//
//#define Xa_LogOutput_P06(a,b,c,d,e,f) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//{ \
//	a_LogOutput(a, p1,c,d,e,f); \
//	DeleteEncStrA(p1); \
//} \
//else{}\
//} \
//
//#define Xa_LogOutput_P07(a,b,c,d,e,f,g) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//{ \
//	a_LogOutput(a, p1,c,d,e,f,g); \
//	DeleteEncStrA(p1); \
//} \
//else{}\
//} \
//
//#define Xa_LogOutput_P08(a,b,c,d,e,f,g ,h) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//{ \
//	a_LogOutput(a, p1,c,d,e,f,g,h); \
//	DeleteEncStrA(p1); \
//} \
//else{}\
//} \
//
//
//#define Xa_LogOutput_P09(a,b,c,d,e,f,g ,h,i) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//{ \
//	a_LogOutput(a, p1,c,d,e,f,g,h,i); \
//	DeleteEncStrA(p1); \
//} \
//else{}\
//} \
//	
//#define Xa_LogOutput_P10(a,b,c,d,e,f,g ,h,i,j) { \
//	CHAR *p1 = NewEncStrA(b); \
//	if(p1!=NULL) \
//{ \
//	a_LogOutput(a, p1,c,d,e,f,g,h,i,j); \
//	DeleteEncStrA(p1); \
//} \
//else{}\
//} \
//

#endif
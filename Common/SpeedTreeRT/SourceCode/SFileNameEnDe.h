
#pragma once

//ȭ���̸��� ������ �̸����� ����, �ݴ뵵 ����
//������ ȭ�� �̸��� ��ġ�� �ȵȴ�. ���ɼ���???

class SFileNameEnDe
{
public:
	SFileNameEnDe();
	virtual ~SFileNameEnDe();

	static void Encode( char * pInName, char * pOutName );
	static void Encode( char * pInName, std::string & pOutName );
	static void Decode( char * pInName, char * pOutName );
	static void Decode( char * pInName, std::string & pOutName );
	static bool IsEncoded( const char * pInName );
};
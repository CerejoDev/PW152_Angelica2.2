#ifndef BASE64_H
#define BASE64_H

int	base64_encode(unsigned char *__in_stream, int __inlen, char *__out_stream);

// __in�е��ַ���һ����4��������
// __out�Ĵ�СӦ���� __in * 3 /4
// һ�δ���__in��4���ַ������__out�����������ַ�
//  ǰ�����ǲ���Ϊ��������
int	base64_decode(char *__in_stream, int __inlen, unsigned char *__out_stream);

#endif
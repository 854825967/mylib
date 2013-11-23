#ifndef _AES_H__
#define _AES_H__

class AES
{
public:
	AES(unsigned char* key);
	~AES();

	// 加密过程
	unsigned char* Cipher(unsigned char* input);
	// 解密过程
	unsigned char* InvCipher(unsigned char* input);
	// 外部数据加密过程
	void* Cipher(void* input, int length=0);
	// 外部数据解密过程
	void* InvCipher(void* input, int length);

private:
	unsigned char Sbox[256];
	unsigned char InvSbox[256];
	unsigned char w[11][4][4];

	void KeyExpansion(unsigned char* key, unsigned char w[][4][4]);
	unsigned char FFmul(unsigned char a, unsigned char b);

	void SubBytes(unsigned char state[][4]);
	void ShiftRows(unsigned char state[][4]);
	void MixColumns(unsigned char state[][4]);
	void AddRoundKey(unsigned char state[][4], unsigned char k[][4]);

	void InvSubBytes(unsigned char state[][4]);
	void InvShiftRows(unsigned char state[][4]);
	void InvMixColumns(unsigned char state[][4]);
};

#endif//_AES_H__

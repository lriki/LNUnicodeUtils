#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../LNUnicodeUtils.h"

using namespace LN;

#define TEST_EQ(a, b) if (a != b) { printf("[Failed] line:%d\n", __LINE__); exit(1); }

void ReadAllBytes(const char* filePath, void* buffer, int size)
{
	FILE* fp = fopen(filePath, "r");
	if (!fp) {
		printf("file not found.");
		exit(1);
	}
	fread(buffer, 1, size, fp);
	fclose(fp);
}


UTFConversionOptions gCommonOption = { 0 };

int main()
{
	gCommonOption.ReplacementChar = '?';

	// UTF8 の不正なシーケンスチェック
	{
		// /(スラッシュ) を 0xC0 0xAF でも表現できるが、これは不正
		UTF8 utf8Buf[3] = { 0xC0, 0xAF, 0x00 };
		UTF16 utf16Buf[3] = { 0 };
		UTFConversionResult r = UnicodeUtils::ConvertUTF8toUTF16(
			utf8Buf, 3, utf16Buf, 3, &gCommonOption);
		TEST_EQ(UTFConversionResult_SourceIllegal, r);
	}

	// UTF8 文字数チェック
	{
		// "Aｱ𩸽/" (末尾は不正/)
		UTF8 utf8Buf[] = { 
			0x41, 
			0xEF, 0xBD, 0xB1, 
			0xF0, 0xA9, 0xB8, 0xBD,
			0xC0, 0xAF
		};
		int count;
		UTFConversionResult r = UnicodeUtils::GetUTF8CharCount(utf8Buf, sizeof(utf8Buf), false, &count);
		TEST_EQ(4, count);

		// 不正チェック
		r = UnicodeUtils::GetUTF8CharCount(utf8Buf, sizeof(utf8Buf), true, &count);
		TEST_EQ(UTFConversionResult_SourceIllegal, r);
	}

	// UTF16 文字数チェック
	{
		// "Aｱ𩸽?" (末尾が不正サロゲートペア)
		UTF16 utf16Buf[] = { 
			0x0041,
			0xFF71, 
			0xD867, 0xDE3D,
			0xD867,			// 上位サロゲートだけ
			0x0041,
		};
		int count;
		UTFConversionResult r = UnicodeUtils::GetUTF16CharCount(utf16Buf, sizeof(utf16Buf) / sizeof(UTF16), false, &count);
		TEST_EQ(5, count);

		// 不正チェック
		r = UnicodeUtils::GetUTF16CharCount(utf16Buf, sizeof(utf16Buf) / sizeof(UTF16), true, &count);
		TEST_EQ(UTFConversionResult_SourceIllegal, r);
	}


	// Length チェック
	{
		// "Aｱ𩸽"
		UTF8 utf8Buf[] = { 0x41, 0xEF, 0xBD, 0xF0, 0xA9, 0xB8, 0xBD, 0x00 };
		size_t len = UnicodeUtils::GetUTF8Length(utf8Buf);
		TEST_EQ(7, len);

		// NULL
		len = UnicodeUtils::GetUTF8Length(NULL);
		TEST_EQ(0, len);
	}

	// Length チェック
	{
		// "𩸽AB" UTF16
		UTF16 utf16Buf[] = { 0xD867, 0xDE3D, 0x0041, 0x0042, 0x0000, 0x0000 };
		size_t len = UnicodeUtils::GetUTF16Length(utf16Buf);
		TEST_EQ(4, len);

		// NULL
		len = UnicodeUtils::GetUTF16Length(NULL);
		TEST_EQ(0, len);
	}

	// Length チェック
	{
		// "𩸽AB" UTF32
		UTF32 utf32Buf[] = { 0x00029E3D, 0x00000041, 0x00000042, 0x00000000 };
		size_t len = UnicodeUtils::GetUTF32Length(utf32Buf);
		TEST_EQ(3, len);

		// NULL
		len = UnicodeUtils::GetUTF32Length(NULL);
		TEST_EQ(0, len);
	}


	{
		LN::UTF8 utf8Buf[8192] = { 0 };
		ReadAllBytes("../Test/UTF8LF.txt", utf8Buf, sizeof(utf8Buf));
		
		LN::UTF16 utf16Buf[8192] = { 0 };
		LN::UTFConversionOptions options = { 0 };
		options.ReplacementChar = '?';
		LN::UTFConversionResult r = LN::UnicodeUtils::ConvertUTF8toUTF16(
			utf8Buf, LN::UnicodeUtils::GetUTF8Length(utf8Buf) + 1, utf16Buf, 8192, &options);
		TEST_EQ(LN::UTFConversionResult_Success, r);

		LN::UTF8 utf8Buf2[8192] = { 0 };
		//UTFConversionOptions options = { 0 };
		//options.ReplacementChar = '?';
		/*LN::UTFConversionResult */r = LN::UnicodeUtils::ConvertUTF16toUTF8(
			utf16Buf, LN::UnicodeUtils::GetUTF16Length(utf16Buf) + 1, utf8Buf2, 8192, &options);
		TEST_EQ(LN::UTFConversionResult_Success, r);

		TEST_EQ(0, memcmp(utf8Buf, utf8Buf2, sizeof(utf8Buf)));
	}


	printf("[Successed.]\n");
	return 0;
}



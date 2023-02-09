/*
* Copyright 2021 gitlost
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "CharacterSet.h"
#include "TextDecoder.h"
#include "TextUtfEncoding.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace ZXing;
using namespace ZXing::TextUtfEncoding;
using namespace testing;

TEST(TextDecoderTest, AppendBINARY_ASCII)
{
	uint8_t data[256];
	for (int i = 0; i < 256; i++) {
		data[i] = (uint8_t)i;
	}

	{
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::BINARY);
		EXPECT_THAT(str, ElementsAreArray(data, sizeof(data)));
	}

	{
		// Accepts non-ASCII
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::ASCII);
		EXPECT_THAT(str, ElementsAreArray(data, sizeof(data)));
	}
}

TEST(TextDecoderTest, AppendAllASCIIRange00_7F)
{
	uint8_t data[0x80];
	uint8_t dataUnicodeBig[0x80 * 2];
	for (int i = 0; i < 0x80; i++) {
		data[i] = (uint8_t)i;
		dataUnicodeBig[i << 1] = 0;
		dataUnicodeBig[(i << 1) + 1] = (uint8_t)i;
	}

	for (int i = 0; i < static_cast<int>(CharacterSet::CharsetCount); i++) {
		std::wstring str;
		if (i == static_cast<int>(CharacterSet::UnicodeBig)) {
			TextDecoder::Append(str, dataUnicodeBig, sizeof(dataUnicodeBig), static_cast<CharacterSet>(i));
		} else {
			TextDecoder::Append(str, data, sizeof(data), static_cast<CharacterSet>(i));
		}
		EXPECT_THAT(str, ElementsAreArray(data, sizeof(data))) << "charset: " << i;
	}
}

TEST(TextDecoderTest, AppendISO8859Range80_9F)
{
	uint8_t data[0xA0 - 0x80];
	for (int i = 0x80; i < 0xA0; i++) {
		data[i - 0x80] = (uint8_t)i;
	}
	static const CharacterSet isos[] = {
		CharacterSet::ISO8859_1, CharacterSet::ISO8859_2, CharacterSet::ISO8859_3, CharacterSet::ISO8859_4,
		CharacterSet::ISO8859_5, CharacterSet::ISO8859_6, CharacterSet::ISO8859_7, CharacterSet::ISO8859_8,
		CharacterSet::ISO8859_7, CharacterSet::ISO8859_8, CharacterSet::ISO8859_9, CharacterSet::ISO8859_10,
		// CharacterSet::ISO8859_11 extended with 9 CP874 codepoints in 0x80-9F range
		CharacterSet::ISO8859_13, CharacterSet::ISO8859_14, CharacterSet::ISO8859_15, CharacterSet::ISO8859_16
	};

	for (CharacterSet iso : isos) {
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), iso);
		EXPECT_THAT(str, ElementsAreArray(data, sizeof(data))) << "iso: " << static_cast<int>(iso);
	}

	// CharacterSet::ISO8859_11 - note maps non-defined to U+FFFD
	std::wstring str;
	TextDecoder::Append(str, data, sizeof(data), CharacterSet::ISO8859_11);
	EXPECT_EQ(str, L"\u20AC\uFFFD\uFFFD\uFFFD\uFFFD\u2026\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD"
				   L"\uFFFD\u2018\u2019\u201C\u201D\u2022\u2013\u2014\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD");
}

TEST(TextDecoderTest, AppendShift_JIS)
{
	{
		// Shift JIS 0x5C (backslash in ASCII) normally mapped to U+00A5 (Yen sign), but direct ASCII mapping used
		static const uint8_t data[] = { 0x5C };
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::Shift_JIS);
		EXPECT_EQ(str, std::wstring(L"\u005C")); // Would normally be "\u00A5"
		EXPECT_EQ(ToUtf8(str), "\\"); // "¥" ditto
	}

	{
		// Shift JIS 0x815F goes to U+FF3C (full width reverse solidus i.e. backslash)
		static const uint8_t data[] = { 0x81, 0x5F };
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::Shift_JIS);
		EXPECT_EQ(str, std::wstring(L"\uFF3C"));
		EXPECT_EQ(ToUtf8(str), "＼");
	}

	{
		// Shift JIS 0xA5 (Yen sign in ISO/IEC 8859-1) goes to U+FF65 (half-width katakana middle dot)
		static const uint8_t data[] = { 0xA5 };
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::Shift_JIS);
		EXPECT_EQ(str, std::wstring(L"\uFF65"));
		EXPECT_EQ(ToUtf8(str), "･");
	}

	{
		// Shift JIS 0x7E (tilde in ASCII) normally mapped to U+203E (overline), but direct ASCII mapping used
		static const uint8_t data[] = { 0x7E };
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::Shift_JIS);
		EXPECT_EQ(str, std::wstring(L"~")); // Would normally be "\u203E"
		EXPECT_EQ(ToUtf8(str), "~"); // "‾" ditto
	}

	{
		static const uint8_t data[] = { 'a', 0x83, 0xC0, 'c', 0x84, 0x47, 0xA5, 0xBF, 0x81, 0x5F, 0x93, 0x5F,
										0xE4, 0xAA, 0x83, 0x65 };
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::Shift_JIS);
		EXPECT_EQ(str, std::wstring(L"a\u03B2c\u0416\uFF65\uFF7F\uFF3C\u70B9\u8317\u30C6"));
		EXPECT_EQ(ToUtf8(str), "aβcЖ･ｿ＼点茗テ");
	}
}

TEST(TextDecoderTest, AppendBig5)
{
	{
		static const uint8_t data[] = { 0xA1, 0x5A }; // Drawings box light left in Big5-2003; not in original Big5
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::Big5);
		EXPECT_EQ(str, std::wstring(L"\u2574"));
		EXPECT_EQ(ToUtf8(str), "╴");
	}

	{
		static const uint8_t data[] = { 0xA1, 0x56 }; // En dash U+2013 in Big5, horizontal bar U+2015 in Big5-2003
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::Big5);
		EXPECT_EQ(str, std::wstring(L"\u2013"));
		EXPECT_EQ(ToUtf8(str), "–");
	}

	{
		static const uint8_t data[] = { 0x1, ' ', 0xA1, 0x71, '@', 0xC0, 0x40, 0xF9, 0xD5, 0x7F };
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::Big5);
		EXPECT_EQ(str, std::wstring(L"\u0001 \u3008@\u9310\u9F98\u007F"));
		EXPECT_EQ(ToUtf8(str), "\x01 〈@錐龘\x7F");
	}
}

TEST(TextDecoderTest, AppendGB2312)
{
	{
		static const uint8_t data[] = { 'a', 0xA6, 0xC2, 'c', 0xA1, 0xA4, 0xA1, 0xAA, 0xA8, 0xA6, 'Z' };
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::GB2312);
		EXPECT_EQ(str, std::wstring(L"a\u03B2c\u00B7\u2014\u00E9Z"));
		EXPECT_EQ(ToUtf8(str), "aβc·—éZ");
	}
}

TEST(TextDecoderTest, AppendGB18030)
{
	{
		static const uint8_t data[] = { 'a', 0xA6, 0xC2, 'c', 0x81, 0x39, 0xA7, 0x39, 0xA1, 0xA4, 0xA1, 0xAA,
										0xA8, 0xA6, 'Z' };
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::GB18030);
		EXPECT_EQ(str, std::wstring(L"a\u03B2c\u30FB\u00B7\u2014\u00E9Z"));
		EXPECT_EQ(ToUtf8(str), std::string("aβc・·—éZ"));
	}
}

TEST(TextDecoderTest, AppendEUC_KR)
{
	{
		static const uint8_t data[] = { 0xA2, 0xE6 }; // Euro sign U+20AC added KS X 1001:1998, not supported
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::EUC_KR);
		EXPECT_EQ(str, std::wstring(L"\uFFFD"));
		EXPECT_EQ(ToUtf8(str), std::string("\xEF\xBF\xBD"));
	}

	{
		static const uint8_t data[] = { 'a', 0xA4, 0xA1, 'Z' };
		std::wstring str;
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::EUC_KR);
		EXPECT_EQ(str, std::wstring(L"a\u3131Z"));
		EXPECT_EQ(ToUtf8(str), std::string("aㄱZ"));
	}
}

TEST(TextDecoderTest, AppendUnicodeBig)
{
	{
		std::wstring str;
		static const uint8_t data[] = { 0x00, 0x01, 0x00, 0x7F, 0x00, 0x80, 0x00, 0xFF, 0x01, 0xFF, 0x10, 0xFF,
										0xFF, 0xFD };
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::UnicodeBig);
		EXPECT_EQ(str, std::wstring(L"\u0001\u007F\u0080\u00FF\u01FF\u10FF\uFFFD"));
		EXPECT_EQ(ToUtf8(str), std::string("\x01\x7F\xC2\x80ÿǿჿ\xEF\xBF\xBD"));
	}

	{
		std::wstring str;
		static const uint8_t data[] = { 0xD8, 0x00, 0xDC, 0x00 }; // Surrogate pair U+10000
		TextDecoder::Append(str, data, sizeof(data), CharacterSet::UnicodeBig);
		EXPECT_EQ(str, std::wstring(L"\U00010000"));
		EXPECT_EQ(ToUtf8(str), std::string("𐀀"));
	}
}

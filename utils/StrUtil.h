/* Copyright 2015 the SumatraPDF project authors (see AUTHORS file).
   License: Simplified BSD (see COPYING.BSD) */

   // note: include BaseUtil.h instead of including directly

namespace str {

	enum TrimOpt {
		TrimLeft,
		TrimRight,
		TrimBoth
	};

	size_t Len(const char *s);
	size_t Len(const wchar_t *s);

	char *  Dup(const char *s);
	wchar_t * Dup(const wchar_t *s);

	void ReplacePtr(char **s, const char *snew);
	void ReplacePtr(const char **s, const char *snew);
	void ReplacePtr(wchar_t **s, const wchar_t *snew);

	char *  Join(const char *s1, const char *s2, const char *s3 = nullptr);
	char *  Join(const char *s1, const char *s2, const char *s3, Allocator *allocator);
	wchar_t * Join(const wchar_t *s1, const wchar_t *s2, const wchar_t *s3 = nullptr);

	bool Eq(const char *s1, const char *s2);
	bool Eq(const wchar_t *s1, const wchar_t *s2);
	bool EqI(const char *s1, const char *s2);
	bool EqI(const wchar_t *s1, const wchar_t *s2);
	bool EqIS(const char *s1, const char *s2);
	bool EqIS(const wchar_t *s1, const wchar_t *s2);
	bool EqN(const char *s1, const char *s2, size_t len);
	bool EqN(const wchar_t *s1, const wchar_t *s2, size_t len);
	bool EqNI(const char *s1, const char *s2, size_t len);
	bool EqNI(const wchar_t *s1, const wchar_t *s2, size_t len);

	template <typename T>
	inline bool IsEmpty(T *s) {
		return !s || (0 == *s);
	}

	template <typename T>
	inline bool StartsWith(const T* str, const T* txt) {
		return EqN(str, txt, Len(txt));
	}

	bool StartsWithI(const char *str, const char *txt);
	bool StartsWithI(const wchar_t *str, const wchar_t *txt);
	bool EndsWith(const char *txt, const char *end);
	bool EndsWith(const wchar_t *txt, const wchar_t *end);
	bool EndsWithI(const char *txt, const char *end);
	bool EndsWithI(const wchar_t *txt, const wchar_t *end);

	inline bool EqNIx(const char *s, size_t len, const char *s2) {
		return str::Len(s2) == len && str::StartsWithI(s, s2);
	}

	char *  DupN(const char *s, size_t lenCch);
	wchar_t * DupN(const wchar_t *s, size_t lenCch);

	char* ToLowerInPlace(char *s);
	wchar_t* ToLowerInPlace(wchar_t *s);

	char *  ToMultiByte(const wchar_t *txt, UINT CodePage, int cchTxtLen = -1);
	char *  ToMultiByte(const char *src, UINT CodePageSrc, UINT CodePageDest);
	wchar_t * ToWideChar(const char *src, UINT CodePage, int cbSrcLen = -1);
	void    Utf8Encode(char *& dst, int c);

	inline const char * FindChar(const char *str, const char c) {
		return strchr(str, c);
	}
	inline const wchar_t * FindChar(const wchar_t *str, const wchar_t c) {
		return wcschr(str, c);
	}
	inline char * FindChar(char *str, const char c) {
		return strchr(str, c);
	}
	inline wchar_t * FindChar(wchar_t *str, const wchar_t c) {
		return wcschr(str, c);
	}

	inline const char * FindCharLast(const char *str, const char c) {
		return strrchr(str, c);
	}
	inline const wchar_t * FindCharLast(const wchar_t *str, const wchar_t c) {
		return wcsrchr(str, c);
	}
	inline char * FindCharLast(char *str, const char c) {
		return strrchr(str, c);
	}
	inline wchar_t * FindCharLast(wchar_t *str, const wchar_t c) {
		return wcsrchr(str, c);
	}

	inline const char * Find(const char *str, const char *find) {
		return strstr(str, find);
	}
	inline const wchar_t * Find(const wchar_t *str, const wchar_t *find) {
		return wcsstr(str, find);
	}

	const char * FindI(const char *str, const char *find);
	const wchar_t * FindI(const wchar_t *str, const wchar_t *find);

	bool    BufFmtV(char *buf, size_t bufCchSize, const char *fmt, va_list args);
	char *  FmtV(const char *fmt, va_list args);
	char *  Format(const char *fmt, ...);
	bool    BufFmtV(wchar_t *buf, size_t bufCchSize, const wchar_t *fmt, va_list args);
	wchar_t * FmtV(const wchar_t *fmt, va_list args);
	wchar_t * Format(const wchar_t *fmt, ...);

	inline bool IsWs(char c) { return ' ' == c || '\t' <= c && c <= '\r'; }
	inline bool IsWs(wchar_t c) { return iswspace(c); }

	// Note: I tried an optimization: return (unsigned)(c - '0') < 10;
	// but it seems to mis-compile in release builds
	inline bool IsDigit(char c) {
		return '0' <= c && c <= '9';
	}

	inline bool IsDigit(wchar_t c) {
		return '0' <= c && c <= '9';
	}

	inline bool IsNonCharacter(wchar_t c) {
		return c >= 0xFFFE || (c & ~1) == 0xDFFE || (0xFDD0 <= c && c <= 0xFDEF);
	}

	size_t  TrimWS(wchar_t *s, TrimOpt opt = TrimBoth);
	void    TrimWsEnd(char *s, char *&e);

	size_t  TransChars(char *str, const char *oldChars, const char *newChars);
	size_t  TransChars(wchar_t *str, const wchar_t *oldChars, const wchar_t *newChars);
	char *  Replace(const char *s, const char *toReplace, const char *replaceWith);
	wchar_t * Replace(const wchar_t *s, const wchar_t *toReplace, const wchar_t *replaceWith);

	size_t  NormalizeWS(char *str);
	size_t  NormalizeWS(wchar_t *str);
	size_t  NormalizeNewlinesInPlace(char *s, char *e);
	size_t  NormalizeNewlinesInPlace(char *s);
	size_t  RemoveChars(char *str, const char *toRemove);
	size_t  RemoveChars(wchar_t *str, const wchar_t *toRemove);

	size_t  BufSet(char *dst, size_t dstCchSize, const char *src);
	size_t  BufSet(wchar_t *dst, size_t dstCchSize, const wchar_t *src);
	size_t  BufAppend(char *dst, size_t dstCchSize, const char *s);
	size_t  BufAppend(wchar_t *dst, size_t dstCchSize, const wchar_t *s);

	char *  MemToHex(const unsigned char *buf, size_t len);
	bool    HexToMem(const char *s, unsigned char *buf, size_t bufLen);

	wchar_t * FormatFloatWithThousandSep(double number, LCID locale = LOCALE_USER_DEFAULT);
	wchar_t * FormatNumWithThousandSep(size_t num, LCID locale = LOCALE_USER_DEFAULT);
	wchar_t * FormatRomanNumeral(int number);

	int     CmpNatural(const wchar_t *a, const wchar_t *b);

	const char  *   Parse(const char *str, const char *format, ...);
	const char  *   Parse(const char *str, size_t len, const char *format, ...);
	const wchar_t *   Parse(const wchar_t *str, const wchar_t *format, ...);

	size_t Utf8ToWcharBuf(const char *s, size_t sLen, wchar_t *bufOut, size_t cchBufOutSize);
	size_t WcharToUtf8Buf(const wchar_t *s, char *bufOut, size_t cbBufOutSize);

	namespace conv {

		inline wchar_t *  FromCodePage(const char *src, UINT cp) { return ToWideChar(src, cp); }
		inline char *   ToCodePage(const wchar_t *src, UINT cp) { return ToMultiByte(src, cp); }

		inline wchar_t *  FromUtf8(const char *src, size_t cbSrcLen) { return ToWideChar(src, CP_UTF8, (int)cbSrcLen); }
		inline wchar_t *  FromUtf8(const char *src) { return ToWideChar(src, CP_UTF8); }
		inline char *   ToUtf8(const wchar_t *src, size_t cchSrcLen) { return ToMultiByte(src, CP_UTF8, (int)cchSrcLen); }
		inline char *   ToUtf8(const wchar_t *src) { return ToMultiByte(src, CP_UTF8); }
		inline wchar_t *  FromAnsi(const char *src, size_t cbSrcLen = (size_t)-1) { return ToWideChar(src, CP_ACP, (int)cbSrcLen); }
		inline char *   ToAnsi(const wchar_t *src) { return ToMultiByte(src, CP_ACP); }
		char *          UnknownToUtf8(const char *src, size_t len = 0);

		size_t ToCodePageBuf(char *buf, int cbBufSize, const wchar_t *s, UINT cp);
		size_t FromCodePageBuf(wchar_t *buf, int cchBufSize, const char *s, UINT cp);

	} // namespace str::conv

}  // namespace str

namespace url {

	bool IsAbsolute(const wchar_t *url);
	void DecodeInPlace(char *urlUtf8);
	void DecodeInPlace(wchar_t *url);
	wchar_t *GetFullPath(const wchar_t *url);
	wchar_t *GetFileName(const wchar_t *url);

} // namespace url

namespace seqstrings {
	void         SkipStr(char *& s);
	void         SkipStr(const char *& s);
	int          StrToIdx(const char *strings, const char *toFind);
	int          StrToIdx(const char *strings, const wchar_t *toFind);
	const char * IdxToStr(const char *strings, int idx);

} // namespace seqstrings

#define _MemToHex(ptr) str::MemToHex((const unsigned char *)(ptr), sizeof(*ptr))
#define _HexToMem(txt, ptr) str::HexToMem(txt, (unsigned char *)(ptr), sizeof(*ptr))

#define UTF8_BOM    "\xEF\xBB\xBF"
#define UTF16_BOM   "\xFF\xFE"
#define UTF16BE_BOM "\xFE\xFF"

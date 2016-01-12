/* Copyright 2015 the SumatraPDF project authors (see AUTHORS file).
   License: Simplified BSD (see COPYING.BSD) */

#include"pch.h"
#include "BaseUtil.h"
#include "HtmlParserLookup.h"
#include"TextRender.h"


Rect MeasureTextAccurate(CanvasDrawingSession^ g, CanvasTextFormat^  f, const wchar_t *s, size_t len)
{
	if (0 == len)
		return Rect(0, 0, 0, 0); // TODO: should set height to font's height
								  // note: frankly, I don't g a difference between those StringFormat variations
	CanvasTextLayout^ textLayout = ref new CanvasTextLayout(g, ref new Platform::String(s), f, 0.0f, 0.0f);
	Rect bbox =  Rect( textLayout->DrawBounds.X, textLayout->DrawBounds.Y, textLayout->DrawBounds.Width, textLayout->DrawBounds.Height);
	return  bbox;
}


using namespace Windows::UI;
    TextRenderWin2d *TextRenderWin2d::Create(CanvasDrawingSession^ gfx,Rect(*measureAlgo)(CanvasDrawingSession^ g, CanvasTextFormat^  f, const wchar_t *s, size_t len)) {
	TextRenderWin2d *res = new TextRenderWin2d();
    res->gfx = gfx;
    res->currFont = nullptr;
    if (nullptr == measureAlgo)
        res->measureAlgo = MeasureTextAccurate;
    else
        res->measureAlgo = measureAlgo;
    // default to red to make mistakes stand out
	Color c;
	c.A = 0xff;
	c.R = 0xff;
	c.G = 0x0;
	c.B = 0x0;
    res->SetTextColor(c);
    return res;
}
	
	typedef Rect(*TextMeasureAlgorithm)(CanvasDrawingSession^ g, CanvasTextFormat^  f, const wchar_t *s, size_t len);
	Rect MeasureText(CanvasDrawingSession^ g, CanvasTextFormat^  f, const wchar_t *s, size_t len, TextMeasureAlgorithm algo)
	{
		if (-1 == len)
			len = str::Len(s);
		CrashIf(len > INT_MAX);
		if (algo)
			return algo(g, f, s, (int)len);
		Rect bbox = MeasureTextAccurate(g, f, s, (int)len);
		return bbox;
	}

void TextRenderWin2d::SetFont(CanvasTextFormat^ font) {
    currFont = font;
}

float TextRenderWin2d::GetCurrFontLineSpacing() { return currFont->LineSpacing; }

Rect TextRenderWin2d::Measure(const wchar_t *s, size_t sLen) {
    CrashIf(!currFont);
    return MeasureText(gfx, currFont, s, sLen, measureAlgo);
}

Rect TextRenderWin2d::Measure(const char *s, size_t sLen) {
    CrashIf(!currFont);
    size_t strLen = str::Utf8ToWcharBuf(s, sLen, txtConvBuf, dimof(txtConvBuf));
    return MeasureText(gfx, currFont, txtConvBuf, strLen, measureAlgo);
}

TextRenderWin2d::~TextRenderWin2d() { ::delete textColorBrush; }

void TextRenderWin2d::SetTextColor(Windows::UI::Color col)
{
    if (textColor.Equals(col) )
	{
        return;
    }
    textColor = col;
    ::delete textColorBrush;
    textColorBrush =  ref new CanvasSolidColorBrush(gfx,col);
}

void TextRenderWin2d::Draw(const wchar_t *s, size_t sLen, Rect &bb, bool isRtl) {
	gfx->DrawText(ref new Platform::String(s), (float)bb.X, (float)bb.Y, textColor, currFont);
  
}

void TextRenderWin2d::Draw(const char *s, size_t sLen, Rect &bb, bool isRtl) {
    size_t strLen = str::Utf8ToWcharBuf(s, sLen, txtConvBuf, dimof(txtConvBuf));
    Draw(txtConvBuf, strLen, bb, isRtl);
}

ITextRender *CreateTextRender(TextRenderMethod method, CanvasDrawingSession^ gfx, int dx, int dy) {
	ITextRender *res = nullptr;
	if (TextRenderMethodGdiplus == method) {
		
	}
	if (TextRenderMethodGdiplusQuick == method) {
		
	}
	if (TextRenderMethodGdi == method) {
		
	}
	if (TextRenderMethodHdc == method) {
		
	}
	if (TextRenderMethodWin2d_ == method) {
		res = TextRenderWin2d::Create(gfx);
	}
	CrashIf(!res);
	res->method = method;
	return res;
}

size_t StringLenForWidth(ITextRender *textMeasure, const WCHAR *s, size_t len, float dx) {
	Rect r = textMeasure->Measure(s, len);
	if (r.Width <= dx)
		return len;
	// make the best guess of the length that fits
	size_t n = (size_t)((dx / r.Width) * (float)len);
	CrashIf((0 == n) || (n > len));
	r = textMeasure->Measure(s, n);
	// find the length len of s that fits within dx iff width of len+1 exceeds dx
	int dir = 1; // increasing length
	if (r.Width > dx)
		dir = -1; // decreasing length
	for (;;) {
		n += dir;
		r = textMeasure->Measure(s, n);
		if (1 == dir) {
			// if advancing length, we know that previous string did fit, so if
			// the new one doesn't fit, the previous length was the right one
			if (r.Width > dx)
				return n - 1;
		}
		else {
			// if decreasing length, we know that previous string didn't fit, so if
			// the one one fits, it's of the correct length
			if (r.Width < dx)
				return n;
		}
	}
}

float GetSpaceDx(ITextRender *textMeasure) {
	Rect bbox;
	// this method seems to return (much) smaller size that measuring
	// the space itself
	bbox = textMeasure->Measure(L"wa", 2);
	float l1 = bbox.Width;
	bbox = textMeasure->Measure(L"w a", 3);
	float l2 = bbox.Width;
	float spaceDx2 = l2 - l1;
	return spaceDx2;
}
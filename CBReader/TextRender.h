/* Copyright 2015 the SumatraPDF project authors (see AUTHORS file).
   License: Simplified BSD (see COPYING.BSD) */
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::Graphics::Canvas::Brushes;
using namespace Microsoft::Graphics::Canvas::Text;
using namespace Windows::Foundation;
enum TextRenderMethod {
    TextRenderMethodGdiplus,      // uses MeasureTextAccurate, which is slower than MeasureTextQuick
    TextRenderMethodGdiplusQuick, // uses MeasureTextQuick
    TextRenderMethodGdi,
    TextRenderMethodHdc,
    // TODO: implement TextRenderDirectDraw
     TextRenderMethodWin2d_
};

class ITextRender {
  public:
    virtual void SetFont(CanvasTextFormat^ font) = 0;
    virtual void SetTextColor(Windows::UI::Color col) = 0;

    // this is only for the benefit of TextRenderGdi. In GDI+, Draw() uses
    // transparent background color (i.e. whatever is under).
    // GDI doesn't support such transparency so the best we can do is simulate
    // that if the background is solid color. It won't work in other cases
    virtual void SetTextBgColor(Windows::UI::Color col) = 0;

    virtual float GetCurrFontLineSpacing() = 0;

    virtual Rect Measure(const char *s, size_t sLen) = 0;
    virtual Rect Measure(const wchar_t *s, size_t sLen) = 0;

    // GDI+ calls cannot be done if we called Graphics::GetHDC(). However, getting/releasing
    // hdc is very expensive and kills performance if we do it for every Draw(). So we add
    // explicit Lock()/Unlock() calls (only important for TextDrawGdi) so that a caller
    // can batch Draw() calls to minimize GetHDC()/ReleaseHDC() calls
    virtual void Lock() = 0;
    virtual void Unlock() = 0;

    virtual void Draw(const char *s, size_t sLen, Rect &bb, bool isRtl) = 0;
    virtual void Draw(const wchar_t *s, size_t sLen, Rect &bb, bool isRtl) = 0;

    virtual ~ITextRender(){};

    TextRenderMethod method;
};



class TextRenderWin2d : public ITextRender {
  private:
    Rect (*measureAlgo)(CanvasDrawingSession^ g, CanvasTextFormat^  f, const wchar_t *s, size_t len);

    // We don't own gfx and currFont
	CanvasDrawingSession^ gfx;
	CanvasTextFormat^ currFont;
    Windows::UI::Color textColor;
	CanvasSolidColorBrush^  textColorBrush;
	wchar_t txtConvBuf[512];

	TextRenderWin2d()
        : gfx(nullptr), currFont(nullptr), textColorBrush(nullptr), textColor() {}

  public:
    static TextRenderWin2d *Create(CanvasDrawingSession^ gfx, Rect(*measureAlgo)(CanvasDrawingSession^ g, CanvasTextFormat^  f, const wchar_t *s, size_t len)=nullptr);

    void SetFont(CanvasTextFormat^ font) override;
    void SetTextColor(Windows::UI::Color col) override;
    void SetTextBgColor(Windows::UI::Color col) override { textColor = col; }

    float GetCurrFontLineSpacing() override;

    Rect Measure(const char *s, size_t sLen) override;
    Rect Measure(const wchar_t *s, size_t sLen) override;

    void Lock() override {}
    void Unlock() override {}

    void Draw(const char *s, size_t sLen, Rect &bb, bool isRtl) override;
    void Draw(const wchar_t *s, size_t sLen, Rect &bb, bool isRtl) override;

    ~TextRenderWin2d() override;
};



ITextRender *CreateTextRender(TextRenderMethod method, CanvasDrawingSession^ gfx, int dx, int dy);

size_t StringLenForWidth(ITextRender *textRender, const wchar_t *s, size_t len, float dx);
float GetSpaceDx(ITextRender *textRender);

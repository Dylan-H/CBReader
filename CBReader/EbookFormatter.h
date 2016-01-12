/* Copyright 2015 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

/* formatting extensions for Mobi */

class MobiDoc;

class MobiFormatter : public HtmlFormatter {
    // accessor to images (and other format-specific data)
    // it can be nullptr (enables testing by feeding raw html)
    MobiDoc *           doc;

    void HandleSpacing_Mobi(HtmlToken *t);
    virtual void HandleTagImg(HtmlToken *t);
    virtual void HandleHtmlTag(HtmlToken *t);

public:
    MobiFormatter(CanvasDrawingSession^ g, HtmlFormatterArgs *args, MobiDoc *doc);
};





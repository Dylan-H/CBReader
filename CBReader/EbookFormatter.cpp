/* Copyright 2015 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

// utils
#include"pch.h"
#include "BaseUtil.h"

#include "HtmlParserLookup.h"
#include "HtmlPullParser.h"
// rendering engines
#include "BaseEngine.h"
#include "EbookBase.h"
#include "MobiDoc.h"
#include "HtmlFormatter.h"
#include "EbookFormatter.h"

/* Mobi-specific formatting methods */

MobiFormatter::MobiFormatter(CanvasDrawingSession^ g, HtmlFormatterArgs* args, MobiDoc *doc) :
    HtmlFormatter(g,args), doc(doc)
{
    bool fromBeginning = (0 == args->reparseIdx);
    if (!doc || !fromBeginning)
        return;

    ImageData *img = doc->GetCoverImage();
    if (!img)
        return;

    // TODO: vertically center the cover image?
    EmitImage(img);
    // only add a new page if the image isn't broken
    if (currLineInstr.Count() > 0)
        ForceNewPage();
}

// parses size in the form "1em" or "3pt". To interpret ems we need emInPoints
// to be passed by the caller
static float ParseSizeAsPixels(const char *s, size_t len, float emInPoints)
{
    float sizeInPoints = 0;
    if (str::Parse(s, len, "%fem", &sizeInPoints)) {
        sizeInPoints *= emInPoints;
    } else if (str::Parse(s, len, "%fin", &sizeInPoints)) {
        sizeInPoints *= 72;
    } else if (str::Parse(s, len, "%fpt", &sizeInPoints)) {
        // no conversion needed
    } else if (str::Parse(s, len, "%fpx", &sizeInPoints)) {
        return sizeInPoints;
    } else {
        return 0;
    }
    // TODO: take dpi into account
    float sizeInPixels = sizeInPoints;
    return sizeInPixels;
}

void MobiFormatter::HandleSpacing_Mobi(HtmlToken *t)
{
    if (!t->IsStartTag())
        return;

    // best I can tell, in mobi <p width="1em" height="3pt> means that
    // the first line of the paragrap is indented by 1em and there's
    // 3pt top padding (the same seems to apply for <blockquote>)
    AttrInfo *attr = t->GetAttrByName("width");
    if (attr) {
        float lineIndent = ParseSizeAsPixels(attr->val, attr->valLen, CurrFont()->FontSize);
        // there are files with negative width which produces partially invisible
        // text, so don't allow that
        if (lineIndent > 0) {
            // this should replace the previously emitted paragraph/quote block
            EmitParagraph(lineIndent);
        }
    }
    attr = t->GetAttrByName("height");
    if (attr) {
        // for use it in FlushCurrLine()
        currLineTopPadding = ParseSizeAsPixels(attr->val, attr->valLen, CurrFont()->FontSize);
    }
}

// mobi format has image tags in the form:
// <img recindex="0000n" alt=""/>
// where recindex is the record number of pdb record
// that holds the image (within image record array, not a
// global record)
void MobiFormatter::HandleTagImg(HtmlToken *t)
{
    // we allow formatting raw html which can't require doc
    if (!doc)
        return;
    bool needAlt = true;
    AttrInfo *attr = t->GetAttrByName("recindex");
    if (attr) {
        int n;
        if (str::Parse(attr->val, attr->valLen, "%d", &n)) {
            ImageData *img = doc->GetImage(n);
            needAlt = !img || !EmitImage(img);
        }
    }
    if (needAlt && (attr = t->GetAttrByName("alt")) != nullptr)
        HandleText(attr->val, attr->valLen);
}

void MobiFormatter::HandleHtmlTag(HtmlToken *t)
{
    CrashIf(!t->IsTag());

    if (Tag_P == t->tag || Tag_Blockquote == t->tag) {
        HtmlFormatter::HandleHtmlTag(t);
        HandleSpacing_Mobi(t);
    } else if (Tag_Mbp_Pagebreak == t->tag) {
        ForceNewPage();
    } else if (Tag_A == t->tag) {
        HandleAnchorAttr(t);
        // handle internal and external links (prefer internal ones)
        if (!HandleTagA(t, "filepos"))
            HandleTagA(t);
    } else if (Tag_Hr == t->tag) {
        // imitating Kindle: hr is proceeded by an empty line
        FlushCurrLine(false);
        EmitEmptyLine(lineSpacing);
        EmitHr();
    } else {
        HtmlFormatter::HandleHtmlTag(t);
    }
}









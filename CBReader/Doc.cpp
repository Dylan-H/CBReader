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
#include "Doc.h"

Doc::Doc(const Doc& other)
{
    Clear();
    type = other.type;

    error = other.error;
    filePath.Set(str::Dup(other.filePath));
}

Doc& Doc::operator=(const Doc& other)
{
    if (this != &other) {
        type = other.type;
        error = other.error;
        filePath.Set(str::Dup(other.filePath));
    }
    return *this;
}

Doc::~Doc()
{
}

// delete underlying object
void Doc::Delete()
{
    switch (type) {

    case Doc_Mobi:
        delete mobiDoc;
        break;
    case Doc_None:
        break;
    default:
        CrashIf(true);
        break;
    }

    Clear();
}

Doc::Doc(MobiDoc *doc)
{
    Clear();
    type = doc ? Doc_Mobi : Doc_None;
    mobiDoc = doc;
}



void Doc::Clear()
{
    type = Doc_None;
    error = Error_None;
    filePath.Set(nullptr);
}

// the caller should make sure there is a document object
const wchar_t *Doc::GetFilePathFromDoc() const
{
    switch (type) {
    case Doc_Mobi:
        return mobiDoc->GetFileName();
    case Doc_None:
        return nullptr;
    default:
        CrashIf(true);
        return nullptr;
    }
}

const wchar_t *Doc::GetFilePath() const
{
    if (filePath) {
        // verify it's consistent with the path in the doc
        const wchar_t *docPath = GetFilePathFromDoc();
        CrashIf(docPath && !str::Eq(filePath, docPath));
        return filePath;
    }
    //CrashIf(!generic && !IsNone());
    return GetFilePathFromDoc();
}

const wchar_t *Doc::GetDefaultFileExt() const
{
    switch (type) {
    case Doc_Epub:
        return L".epub";
    case Doc_Mobi:
        return L".mobi";
    case Doc_Pdb:
        return L".pdb";
    case Doc_None:
        return nullptr;
    default:
        CrashIf(true);
        return nullptr;
    }
}

wchar_t *Doc::GetProperty(DocumentProperty prop) const
{
    switch (type) {
    case Doc_Mobi:
        return mobiDoc->GetProperty(prop);
    case Doc_None:
        return nullptr;
    default:
        CrashIf(true);
        return nullptr;
    }
}

const char *Doc::GetHtmlData(size_t &len) const
{
    switch (type) {

    case Doc_Mobi:
        return mobiDoc->GetHtmlData(len);
    default:
        CrashIf(true);
        return nullptr;
    }
}

size_t Doc::GetHtmlDataSize() const
{
    switch (type) {
    case Doc_Mobi:
        return mobiDoc->GetHtmlDataSize();

    default:
        CrashIf(true);
        return 0;
    }
}

ImageData *Doc::GetCoverImage() const
{
    switch (type) {
    case Doc_Mobi:
        return mobiDoc->GetCoverImage();
    case Doc_Epub:
    case Doc_Pdb:
    default:
        return nullptr;
    }
}

bool Doc::HasToc() const
{
    switch (type) {

    case Doc_Mobi:
        return mobiDoc->HasToc();
    default:
        return false;
    }
}

bool Doc::ParseToc(EbookTocVisitor *visitor) const
{
    switch (type) {

    case Doc_Mobi:
        return mobiDoc->ParseToc(visitor);
    default:
        return false;
    }
}

HtmlFormatter *Doc::CreateFormatter(CanvasDrawingSession^g, HtmlFormatterArgs *args) const
{
    switch (type) {
    case Doc_Mobi:
        return new MobiFormatter(g,args, mobiDoc);
    case Doc_Pdb:
        return new HtmlFormatter(g,args);
    default:
        CrashIf(true);
        return nullptr;
    }
}

Doc Doc::CreateFromFile(const wchar_t *filePath)
{
    Doc doc= Doc(MobiDoc::CreateFromFile(filePath));


    return doc;
}

bool Doc::IsSupportedFile(const wchar_t *filePath, bool sniff)
{
    return MobiDoc::IsSupportedFile(filePath, sniff) ;
}

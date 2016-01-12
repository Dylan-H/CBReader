/* Copyright 2015 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

// this is a collection of structs and classes that are
// useful for more than one ebook format

#pragma once
struct ImageData {
    char *      data;
    size_t      len;
};

class EbookTocVisitor {
public:
    virtual void Visit(const wchar_t *name, const wchar_t *url, int level) = 0;
    virtual ~EbookTocVisitor() { }
};

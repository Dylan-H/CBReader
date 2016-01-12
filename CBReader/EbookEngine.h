/* Copyright 2015 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

namespace EpubEngine {

bool IsSupportedFile(const wchar_t *fileName, bool sniff=false);
BaseEngine *CreateFromFile(const wchar_t *fileName);
BaseEngine *CreateFromStream(IStream *stream);

}

namespace Fb2Engine {

bool IsSupportedFile(const wchar_t *fileName, bool sniff=false);
BaseEngine *CreateFromFile(const wchar_t *fileName);
BaseEngine *CreateFromStream(IStream *stream);

}

namespace MobiEngine {

bool IsSupportedFile(const wchar_t *fileName, bool sniff=false);
BaseEngine *CreateFromFile(const wchar_t *fileName);
BaseEngine *CreateFromStream(IStream *stream);

}

namespace PdbEngine {

bool IsSupportedFile(const wchar_t *fileName, bool sniff=false);
BaseEngine *CreateFromFile(const wchar_t *fileName);

}

namespace ChmEngine {

bool IsSupportedFile(const wchar_t *fileName, bool sniff=false);
BaseEngine *CreateFromFile(const wchar_t *fileName);

}

namespace HtmlEngine {

bool IsSupportedFile(const wchar_t *fileName, bool sniff=false);
BaseEngine *CreateFromFile(const wchar_t *fileName);

}

namespace TxtEngine {

bool IsSupportedFile(const wchar_t *fileName, bool sniff=false);
BaseEngine *CreateFromFile(const wchar_t *fileName);

}

void SetDefaultEbookFont(const wchar_t *name, float size);

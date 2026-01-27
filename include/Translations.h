#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#include <wx/wx.h>

enum Language {
    LANGUAGE_ENGLISH,
    LANGUAGE_SERBIAN
};

wxString Translate(const wxString& key, Language language);

#endif // TRANSLATIONS_H

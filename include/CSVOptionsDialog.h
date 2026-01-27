#ifndef CSVOPTIONSDIALOG_H
#define CSVOPTIONSDIALOG_H

#include <wx/wx.h>
#include <wx/choice.h>
#include "CSVParser.h"

class CSVOptionsDialog : public wxDialog {
public:
    CSVOptionsDialog(wxWindow* parent, Encoding detectedEncoding, 
                     wxChar detectedSeparator, bool hasHeader = false);
    
    Encoding GetSelectedEncoding() const;
    wxChar GetSelectedSeparator() const;
    bool GetHasHeader() const;
    
private:
    wxChoice* encodingChoice;
    wxChoice* separatorChoice;
    wxCheckBox* headerCheckBox;
    
    Encoding detectedEnc;
    wxChar detectedSep;
    wxChar customSeparator;
    
    void OnSeparatorChoice(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    
    wxDECLARE_EVENT_TABLE();
};

#endif // CSVOPTIONSDIALOG_H

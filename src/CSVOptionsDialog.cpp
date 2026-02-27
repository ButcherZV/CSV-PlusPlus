#include "CSVOptionsDialog.h"

wxBEGIN_EVENT_TABLE(CSVOptionsDialog, wxDialog)
    EVT_CHOICE(wxID_ANY, CSVOptionsDialog::OnSeparatorChoice)
    EVT_BUTTON(wxID_OK, CSVOptionsDialog::OnOK)
wxEND_EVENT_TABLE()

CSVOptionsDialog::CSVOptionsDialog(wxWindow* parent, Encoding detectedEncoding,
                                   wxChar detectedSeparator, bool hasHeader, Language language)
    : wxDialog(parent, wxID_ANY, Translate("dialog_csv_options_title", language), wxDefaultPosition, wxSize(400, 250)),
      detectedEnc(detectedEncoding), detectedSep(detectedSeparator), customSeparator(','), lang(language) {
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Encoding selection
    wxStaticText* encLabel = new wxStaticText(this, wxID_ANY, Translate("dialog_encoding_label", lang));
    mainSizer->Add(encLabel, 0, wxALL, 5);
    
    wxArrayString encodings;
    encodings.Add("ANSI");
    encodings.Add("UTF-8");
    encodings.Add("UTF-16");
    
    encodingChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, encodings);
    
    // Set detected encoding as default
    switch (detectedEncoding) {
        case Encoding::ANSI:
            encodingChoice->SetSelection(0);
            break;
        case Encoding::UTF8:
        case Encoding::UTF8_BOM:
            encodingChoice->SetSelection(1);
            break;
        case Encoding::UTF16_LE:
        case Encoding::UTF16_BE:
            encodingChoice->SetSelection(2);
            break;
    }
    
    mainSizer->Add(encodingChoice, 0, wxALL | wxEXPAND, 5);
    
    // Separator selection
    wxStaticText* sepLabel = new wxStaticText(this, wxID_ANY, Translate("dialog_separator_label", lang));
    mainSizer->Add(sepLabel, 0, wxALL, 5);
    
    wxArrayString separators;
    separators.Add(Translate("menu_sep_comma", lang));
    separators.Add(Translate("menu_sep_semicolon", lang));
    separators.Add(Translate("menu_sep_tab", lang));
    separators.Add(Translate("menu_sep_custom", lang));
    
    separatorChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, separators);
    
    // Set detected separator as default
    if (detectedSeparator == ',') {
        separatorChoice->SetSelection(0);
    } else if (detectedSeparator == ';') {
        separatorChoice->SetSelection(1);
    } else if (detectedSeparator == '\t') {
        separatorChoice->SetSelection(2);
    } else {
        separatorChoice->SetSelection(0); // Default comma
    }
    
    mainSizer->Add(separatorChoice, 0, wxALL | wxEXPAND, 5);
    
    // Header checkbox
    headerCheckBox = new wxCheckBox(this, wxID_ANY, Translate("dialog_header_checkbox", lang));
    headerCheckBox->SetValue(hasHeader);
    mainSizer->Add(headerCheckBox, 0, wxALL, 5);
    
    // Buttons
    wxSizer* buttonSizer = CreateButtonSizer(wxOK | wxCANCEL);
    mainSizer->Add(buttonSizer, 0, wxALL | wxEXPAND, 10);
    
    SetSizer(mainSizer);
    Centre();
}

void CSVOptionsDialog::OnSeparatorChoice(wxCommandEvent& event) {
    if (separatorChoice->GetSelection() == 3) { // Custom
        wxTextEntryDialog dialog(this, Translate("dialog_custom_sep_message", lang),
                                Translate("dialog_custom_sep_title", lang), ",");
        if (dialog.ShowModal() == wxID_OK) {
            wxString value = dialog.GetValue();
            if (!value.IsEmpty()) {
                customSeparator = value[0];
            } else {
                // Reset to comma if empty
                separatorChoice->SetSelection(0);
                customSeparator = ',';
            }
        } else {
            // User cancelled, reset to previous selection
            separatorChoice->SetSelection(0);
        }
    }
}

void CSVOptionsDialog::OnOK(wxCommandEvent& event) {
    // Validate custom separator if selected
    if (separatorChoice->GetSelection() == 3 && customSeparator == '\0') {
        customSeparator = ',';
    }
    event.Skip();
}

Encoding CSVOptionsDialog::GetSelectedEncoding() const {
    switch (encodingChoice->GetSelection()) {
        case 0:
            return Encoding::ANSI;
        case 1:
            return Encoding::UTF8;
        case 2:
            return Encoding::UTF16_LE;
        default:
            return Encoding::UTF8;
    }
}

wxChar CSVOptionsDialog::GetSelectedSeparator() const {
    switch (separatorChoice->GetSelection()) {
        case 0:
            return ',';
        case 1:
            return ';';
        case 2:
            return '\t';
        case 3:
            return customSeparator;
        default:
            return ',';
    }
}

bool CSVOptionsDialog::GetHasHeader() const {
    return headerCheckBox->GetValue();
}

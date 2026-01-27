#include "MainFrame.h"
#include "CSVOptionsDialog.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/dcmemory.h>
#include <wx/mstream.h>
#include <wx/config.h>
#include <wx/html/htmlwin.h>
#include <wx/hyperlink.h>

// FileDropTarget implementation
bool FileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
    if (filenames.GetCount() > 0) {
        mainFrame->OpenFileFromDrop(filenames[0]);
        return true;
    }
    return false;
}

// Event table
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(ID_NEW, MainFrame::OnNew)
    EVT_MENU(ID_OPEN, MainFrame::OnOpen)
    EVT_MENU(ID_SAVE, MainFrame::OnSave)
    EVT_MENU(ID_CLOSE, MainFrame::OnClose)
    EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
    EVT_MENU(ID_UNDO, MainFrame::OnUndo)
    EVT_MENU(ID_REDO, MainFrame::OnRedo)
    EVT_MENU(ID_ADD_ROW_BELOW, MainFrame::OnAddRowBelow)
    EVT_MENU(ID_ADD_ROW_ABOVE, MainFrame::OnAddRowAbove)
    EVT_MENU(ID_ADD_COLUMN_LEFT, MainFrame::OnAddColumnLeft)
    EVT_MENU(ID_ADD_COLUMN_RIGHT, MainFrame::OnAddColumnRight)
    EVT_MENU(ID_DELETE_ROW, MainFrame::OnDeleteRow)
    EVT_MENU(ID_DELETE_COLUMN, MainFrame::OnDeleteColumn)
    EVT_MENU(ID_ENC_UTF8, MainFrame::OnEncodingChange)
    EVT_MENU(ID_ENC_ANSI, MainFrame::OnEncodingChange)
    EVT_MENU(ID_ENC_UTF16, MainFrame::OnEncodingChange)
    EVT_MENU(ID_SEP_COMMA, MainFrame::OnSeparatorChange)
    EVT_MENU(ID_SEP_SEMICOLON, MainFrame::OnSeparatorChange)
    EVT_MENU(ID_SEP_TAB, MainFrame::OnSeparatorChange)
    EVT_MENU(ID_SEP_CUSTOM, MainFrame::OnSeparatorChange)
    EVT_MENU(ID_LANG_ENGLISH, MainFrame::OnLanguageChange)
    EVT_MENU(ID_LANG_SERBIAN, MainFrame::OnLanguageChange)
    EVT_MENU(ID_HELP_INSTRUCTIONS, MainFrame::OnInstructions)
    EVT_MENU(ID_HELP_ABOUT, MainFrame::OnAbout)
    EVT_CHOICE(ID_FONT_SIZE_CHOICE, MainFrame::OnFontSizeChange)
    EVT_GRID_EDITOR_SHOWN(MainFrame::OnEditorShown)
    EVT_GRID_CELL_CHANGED(MainFrame::OnCellChanged)
    EVT_GRID_LABEL_LEFT_DCLICK(MainFrame::OnLabelDoubleClick)
    EVT_GRID_CELL_RIGHT_CLICK(MainFrame::OnRightClick)
    EVT_GRID_LABEL_RIGHT_CLICK(MainFrame::OnGridRightClick)
    EVT_GRID_COL_SIZE(MainFrame::OnColSize)
    EVT_GRID_SELECT_CELL(MainFrame::OnSelectCell)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 600)),
      currentEncoding(Encoding::UTF8),
      currentSeparator(','),
      isDirty(false),
      hasHeaderRow(false),
      currentLanguage(LANGUAGE_ENGLISH),
      currentFontSize(12),
      isRestoringState(false) {
    
    // Load language setting from registry
    wxConfig config("CSV++");
    long savedLang = 0;
    if (config.Read("Language", &savedLang)) {
        currentLanguage = (Language)savedLang;
    }
    
    // Create grid
    grid = new wxGrid(this, wxID_ANY);
    grid->CreateGrid(10, 5);
    grid->EnableEditing(true);
    grid->EnableDragGridSize(true);
    grid->EnableDragColSize(true);
    grid->EnableDragRowSize(false);
    grid->SetDefaultCellOverflow(false);
    
    // Apply initial font size to grid
    wxFont font = grid->GetDefaultCellFont();
    font.SetPointSize(currentFontSize);
    grid->SetDefaultCellFont(font);
    
    wxFont labelFont = grid->GetLabelFont();
    labelFont.SetPointSize(currentFontSize);
    grid->SetLabelFont(labelFont);
    
    // Apply initial grid dimensions
    ApplyGridDimensions();
    
    // Set default column labels
    for (int i = 0; i < grid->GetNumberCols(); ++i) {
        wxChar label = 'A' + i;
        grid->SetColLabelValue(i, wxString(label));
    }
    
    // Create UI elements
    CreateMenuBar();
    CreateToolBar();
    CreateStatusBar();
    
    // Set up drag and drop
    SetDropTarget(new FileDropTarget(this));
    
    // Update UI
    UpdateStatusBar();
    UpdateMenuChecks();
    UpdateUndoRedoButtons();
    
    // Check language menu based on loaded setting
    wxMenuBar* menuBar = GetMenuBar();
    if (menuBar) {
        menuBar->Check(ID_LANG_ENGLISH, currentLanguage == LANGUAGE_ENGLISH);
        menuBar->Check(ID_LANG_SERBIAN, currentLanguage == LANGUAGE_SERBIAN);
    }
    
    Centre();
}

void MainFrame::CreateMenuBar() {
    wxMenuBar* menuBar = new wxMenuBar();
    
    // File menu
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ID_NEW, Translate("menu_new", currentLanguage), Translate("menu_new_desc", currentLanguage));
    fileMenu->Append(ID_OPEN, Translate("menu_open", currentLanguage), Translate("menu_open_desc", currentLanguage));
    fileMenu->Append(ID_SAVE, Translate("menu_save", currentLanguage), Translate("menu_save_desc", currentLanguage));
    fileMenu->Append(ID_CLOSE, Translate("menu_close", currentLanguage), Translate("menu_close_desc", currentLanguage));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, Translate("menu_exit", currentLanguage), Translate("menu_exit_desc", currentLanguage));
    menuBar->Append(fileMenu, Translate("menu_file", currentLanguage));
    
    // Settings menu
    wxMenu* settingsMenu = new wxMenu();
    
    // Encoding submenu
    wxMenu* encodingMenu = new wxMenu();
    encodingMenu->AppendRadioItem(ID_ENC_ANSI, "ANSI");
    encodingMenu->AppendRadioItem(ID_ENC_UTF8, "UTF-8");
    encodingMenu->AppendRadioItem(ID_ENC_UTF16, "UTF-16");
    settingsMenu->AppendSubMenu(encodingMenu, Translate("menu_encoding", currentLanguage));
    
    // Separator submenu
    wxMenu* separatorMenu = new wxMenu();
    separatorMenu->AppendRadioItem(ID_SEP_COMMA, Translate("menu_sep_comma", currentLanguage));
    separatorMenu->AppendRadioItem(ID_SEP_SEMICOLON, Translate("menu_sep_semicolon", currentLanguage));
    separatorMenu->AppendRadioItem(ID_SEP_TAB, Translate("menu_sep_tab", currentLanguage));
    separatorMenu->AppendRadioItem(ID_SEP_CUSTOM, Translate("menu_sep_custom", currentLanguage));
    settingsMenu->AppendSubMenu(separatorMenu, Translate("menu_separator", currentLanguage));
    
    // Language submenu
    wxMenu* languageMenu = new wxMenu();
    languageMenu->AppendRadioItem(ID_LANG_ENGLISH, "English");
    languageMenu->AppendRadioItem(ID_LANG_SERBIAN, "Srpski");
    settingsMenu->AppendSubMenu(languageMenu, Translate("menu_language", currentLanguage));
    
    menuBar->Append(settingsMenu, Translate("menu_settings", currentLanguage));
    
    // Help menu
    wxMenu* helpMenu = new wxMenu();
    helpMenu->Append(ID_HELP_INSTRUCTIONS, Translate("menu_help_instructions", currentLanguage), Translate("menu_help_instructions_desc", currentLanguage));
    helpMenu->Append(ID_HELP_ABOUT, Translate("menu_help_about", currentLanguage), Translate("menu_help_about_desc", currentLanguage));
    menuBar->Append(helpMenu, Translate("menu_help", currentLanguage));
    
    SetMenuBar(menuBar);
}

void MainFrame::CreateToolBar() {
    toolBar = wxFrame::CreateToolBar(wxTB_HORIZONTAL | wxTB_FLAT);
    
    // Helper to load BMP icon from resources folder
    auto loadIcon = [](const wxString& filename) -> wxBitmap {
        wxString bmpPath = wxString::Format("resources/%s.bmp", filename);
        if (wxFileExists(bmpPath)) {
            wxBitmap bmp(bmpPath, wxBITMAP_TYPE_BMP);
            if (bmp.IsOk()) {
                wxLogDebug("Loaded icon: %s (%dx%d, depth: %d)", bmpPath, bmp.GetWidth(), bmp.GetHeight(), bmp.GetDepth());
                return bmp;
            } else {
                wxLogDebug("Failed to load icon: %s (file exists but bmp.IsOk() = false)", bmpPath);
            }
        } else {
            wxLogDebug("Icon file not found: %s", bmpPath);
        }
        // Return empty bitmap if loading fails
        return wxBitmap(32, 32);
    };
    
    toolBar->AddTool(ID_NEW, Translate("toolbar_new", currentLanguage), loadIcon("file"), Translate("toolbar_new_hint", currentLanguage));
    toolBar->AddTool(ID_OPEN, Translate("toolbar_open", currentLanguage), loadIcon("folder"), Translate("toolbar_open_hint", currentLanguage));
    toolBar->AddTool(ID_SAVE, Translate("toolbar_save", currentLanguage), loadIcon("device-floppy"), Translate("toolbar_save_hint", currentLanguage));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_UNDO, Translate("toolbar_undo", currentLanguage), loadIcon("undo"), loadIcon("undo-disabled"), wxITEM_NORMAL, Translate("toolbar_undo_hint", currentLanguage));
    toolBar->AddTool(ID_REDO, Translate("toolbar_redo", currentLanguage), loadIcon("redo"), loadIcon("redo-disabled"), wxITEM_NORMAL, Translate("toolbar_redo_hint", currentLanguage));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_ADD_ROW_BELOW, Translate("toolbar_add_row_below", currentLanguage), loadIcon("row-insert-bottom"), Translate("toolbar_add_row_below_hint", currentLanguage));
    toolBar->AddTool(ID_ADD_ROW_ABOVE, Translate("toolbar_add_row_above", currentLanguage), loadIcon("row-insert-top"), Translate("toolbar_add_row_above_hint", currentLanguage));
    toolBar->AddTool(ID_DELETE_ROW, Translate("toolbar_del_row", currentLanguage), loadIcon("row-remove"), Translate("toolbar_del_row_hint", currentLanguage));
    toolBar->AddTool(ID_ADD_COLUMN_LEFT, Translate("toolbar_add_col_left", currentLanguage), loadIcon("column-insert-left"), Translate("toolbar_add_col_left_hint", currentLanguage));
    toolBar->AddTool(ID_ADD_COLUMN_RIGHT, Translate("toolbar_add_col_right", currentLanguage), loadIcon("column-insert-right"), Translate("toolbar_add_col_right_hint", currentLanguage));
    toolBar->AddTool(ID_DELETE_COLUMN, Translate("toolbar_del_col", currentLanguage), loadIcon("column-remove"), Translate("toolbar_del_col_hint", currentLanguage));
    
    toolBar->AddSeparator();
    
    // Font size label and dropdown
    toolBar->AddControl(new wxStaticText(toolBar, wxID_ANY, Translate("toolbar_font_size", currentLanguage) + ": "));
    wxArrayString fontSizes;
    for (int i = 8; i <= 24; i += 2) {
        fontSizes.Add(wxString::Format("%d", i));
    }
    fontSizeChoice = new wxChoice(toolBar, ID_FONT_SIZE_CHOICE, wxDefaultPosition, wxSize(60, -1), fontSizes);
    fontSizeChoice->SetSelection(2); // Default to 12
    toolBar->AddControl(fontSizeChoice);
    
    toolBar->Realize();
}

void MainFrame::CreateStatusBar() {
    statusBar = wxFrame::CreateStatusBar(1);
}

void MainFrame::OnNew(wxCommandEvent& event) {
    if (!PromptSaveChanges()) {
        return;
    }
    
    ClearGrid();
    currentFile.Clear();
    currentEncoding = Encoding::UTF8;
    currentSeparator = ',';
    hasHeaderRow = false;
    
    // Create empty grid
    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }
    if (grid->GetNumberCols() > 0) {
        grid->DeleteCols(0, grid->GetNumberCols());
    }
    
    grid->AppendRows(10);
    grid->AppendCols(5);
    
    // Apply grid dimensions based on current font size
    ApplyGridDimensions();
    
    // Set default column labels
    for (int i = 0; i < grid->GetNumberCols(); ++i) {
        wxChar label = 'A' + i;
        grid->SetColLabelValue(i, wxString(label));
    }
    
    SetDirty(false);
    undoStack.clear();
    redoStack.clear();
    UpdateStatusBar();
    UpdateMenuChecks();
    UpdateUndoRedoButtons();
}

void MainFrame::OnOpen(wxCommandEvent& event) {
    if (!PromptSaveChanges()) {
        return;
    }
    
    wxFileDialog openFileDialog(this, "Open CSV file", "", "",
                               "CSV files (*.csv)|*.csv|Text files (*.txt)|*.txt|All files (*.*)|*.*",
                               wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }
    
    wxString filename = openFileDialog.GetPath();
    OpenFileFromDrop(filename);
}

void MainFrame::OpenFileFromDrop(const wxString& filename) {
    // Auto-detect encoding and separator
    Encoding detectedEnc = CSVParser::DetectEncoding(filename);
    
    std::vector<std::vector<wxString>> tempData;
    wxChar detectedSep = ',';
    bool tempHeader = false;
    
    CSVParser parser;
    if (!parser.ReadFile(filename, tempData, detectedSep, detectedEnc, tempHeader)) {
        wxMessageBox("Failed to read file!", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Show options dialog
    CSVOptionsDialog dialog(this, detectedEnc, detectedSep, true);
    if (dialog.ShowModal() == wxID_OK) {
        Encoding selectedEnc = dialog.GetSelectedEncoding();
        wxChar selectedSep = dialog.GetSelectedSeparator();
        bool hasHeader = dialog.GetHasHeader();
        
        LoadCSVFile(filename, selectedEnc, selectedSep, hasHeader);
    }
}

void MainFrame::LoadCSVFile(const wxString& filename, Encoding encoding,
                           wxChar separator, bool hasHeader) {
    std::vector<std::vector<wxString>> data;
    CSVParser parser;
    
    if (!parser.ReadFile(filename, data, separator, encoding, hasHeader)) {
        wxMessageBox("Failed to load CSV file!", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    if (data.empty()) {
        wxMessageBox("CSV file is empty!", "Warning", wxOK | wxICON_WARNING);
        return;
    }
    
    // Clear existing grid
    ClearGrid();
    
    // Determine grid size
    int rows = data.size();
    int cols = 0;
    for (const auto& row : data) {
        cols = wxMax(cols, (int)row.size());
    }
    
    if (hasHeader && rows > 0) {
        rows--;
    }
    
    // Create grid
    if (rows > 0 && cols > 0) {
        grid->AppendRows(rows);
        grid->AppendCols(cols);
        
        // Apply grid dimensions based on current font size
        ApplyGridDimensions();
        
        // Set headers if present
        int startRow = 0;
        if (hasHeader && !data.empty()) {
            for (int col = 0; col < cols && col < (int)data[0].size(); ++col) {
                grid->SetColLabelValue(col, data[0][col]);
            }
            startRow = 1;
        } else {
            // Default column labels
            for (int col = 0; col < cols; ++col) {
                wxChar label = 'A' + col;
                grid->SetColLabelValue(col, wxString(label));
            }
        }
        
        // Fill data
        for (int row = startRow; row < (int)data.size(); ++row) {
            for (int col = 0; col < (int)data[row].size() && col < cols; ++col) {
                grid->SetCellValue(row - startRow, col, data[row][col]);
            }
        }
    }
    
    currentFile = filename;
    currentEncoding = encoding;
    currentSeparator = separator;
    hasHeaderRow = hasHeader;
    SetDirty(false);
    
    undoStack.clear();
    redoStack.clear();
    
    UpdateStatusBar();
    UpdateMenuChecks();
    UpdateUndoRedoButtons();
    
    SetTitle("CSV++ - " + wxFileName(filename).GetFullName());
}

void MainFrame::OnSave(wxCommandEvent& event) {
    if (currentFile.IsEmpty()) {
        wxFileDialog saveFileDialog(this, "Save CSV file", "", "",
                                   "CSV files (*.csv)|*.csv|Text files (*.txt)|*.txt",
                                   wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        
        if (saveFileDialog.ShowModal() == wxID_CANCEL) {
            return;
        }
        
        currentFile = saveFileDialog.GetPath();
    }
    
    SaveCSVFile(currentFile);
}

void MainFrame::SaveCSVFile(const wxString& filename) {
    std::vector<std::vector<wxString>> data;
    
    // Add headers if present
    if (hasHeaderRow) {
        std::vector<wxString> headers;
        for (int col = 0; col < grid->GetNumberCols(); ++col) {
            headers.push_back(grid->GetColLabelValue(col));
        }
        data.push_back(headers);
    }
    
    // Add data rows
    for (int row = 0; row < grid->GetNumberRows(); ++row) {
        std::vector<wxString> rowData;
        for (int col = 0; col < grid->GetNumberCols(); ++col) {
            rowData.push_back(grid->GetCellValue(row, col));
        }
        data.push_back(rowData);
    }
    
    CSVParser parser;
    if (parser.WriteFile(filename, data, currentSeparator, currentEncoding)) {
        SetDirty(false);
        currentFile = filename;
        SetTitle("CSV++ - " + wxFileName(filename).GetFullName());
        wxMessageBox("File saved successfully!", "Success", wxOK | wxICON_INFORMATION);
    } else {
        wxMessageBox("Failed to save file!", "Error", wxOK | wxICON_ERROR);
    }
}

void MainFrame::OnClose(wxCommandEvent& event) {
    if (PromptSaveChanges()) {
        ClearGrid();
        currentFile.Clear();
        SetTitle("CSV++");
        SetDirty(false);
    }
}

void MainFrame::OnQuit(wxCommandEvent& event) {
    if (PromptSaveChanges()) {
        Close(true);
    }
}

void MainFrame::OnUndo(wxCommandEvent& event) {
    if (undoStack.empty()) {
        return;
    }
    
    // Save current state to redo stack
    redoStack.push_back(GetCurrentState());
    if (redoStack.size() > MAX_UNDO_LEVELS) {
        redoStack.pop_front();
    }
    
    // Restore from undo stack
    GridState state = undoStack.back();
    undoStack.pop_back();
    RestoreState(state);
    
    UpdateUndoRedoButtons();
    SetDirty(true);
}

void MainFrame::OnRedo(wxCommandEvent& event) {
    if (redoStack.empty()) {
        return;
    }
    
    // Save current state to undo stack
    undoStack.push_back(GetCurrentState());
    if (undoStack.size() > MAX_UNDO_LEVELS) {
        undoStack.pop_front();
    }
    
    // Restore from redo stack
    GridState state = redoStack.back();
    redoStack.pop_back();
    RestoreState(state);
    
    UpdateUndoRedoButtons();
    SetDirty(true);
}

void MainFrame::OnAddRowBelow(wxCommandEvent& event) {
    SaveState();
    
    int currentRow = grid->GetGridCursorRow();
    if (currentRow < 0) {
        currentRow = grid->GetNumberRows() - 1;
    }
    
    grid->InsertRows(currentRow + 1, 1);
    
    // Set row height based on current font size
    int rowHeight = currentFontSize * 2 + 8;
    grid->SetRowSize(currentRow + 1, rowHeight);
    
    UpdateStatusBar();
    SetDirty(true);
}

void MainFrame::OnAddRowAbove(wxCommandEvent& event) {
    SaveState();
    
    int currentRow = grid->GetGridCursorRow();
    if (currentRow < 0) {
        currentRow = 0;
    }
    
    grid->InsertRows(currentRow, 1);
    
    // Set row height based on current font size
    int rowHeight = currentFontSize * 2 + 8;
    grid->SetRowSize(currentRow, rowHeight);
    
    UpdateStatusBar();
    SetDirty(true);
}

void MainFrame::OnAddColumnRight(wxCommandEvent& event) {
    SaveState();
    
    int currentCol = grid->GetGridCursorCol();
    if (currentCol < 0) {
        currentCol = grid->GetNumberCols() - 1;
    }
    
    grid->InsertCols(currentCol + 1, 1);
    
    // Set default label
    int newCol = currentCol + 1;
    wxChar label = 'A' + newCol;
    if (newCol < 26) {
        grid->SetColLabelValue(newCol, wxString(label));
    } else {
        grid->SetColLabelValue(newCol, wxString::Format("Col%d", newCol + 1));
    }
    
    // Set column width based on current font size
    int colWidth = (currentFontSize * 60) / 8;
    grid->SetColSize(newCol, colWidth);
    
    UpdateStatusBar();
    SetDirty(true);
}

void MainFrame::OnAddColumnLeft(wxCommandEvent& event) {
    SaveState();
    
    int currentCol = grid->GetGridCursorCol();
    if (currentCol < 0) {
        currentCol = 0;
    }
    
    grid->InsertCols(currentCol, 1);
    
    // Set default label
    wxChar label = 'A' + currentCol;
    if (currentCol < 26) {
        grid->SetColLabelValue(currentCol, wxString(label));
    } else {
        grid->SetColLabelValue(currentCol, wxString::Format("Col%d", currentCol + 1));
    }
    
    // Set column width based on current font size
    int colWidth = (currentFontSize * 60) / 8;
    grid->SetColSize(currentCol, colWidth);
    
    UpdateStatusBar();
    SetDirty(true);
}

void MainFrame::OnDeleteRow(wxCommandEvent& event) {
    wxArrayInt selectedRows = grid->GetSelectedRows();
    
    if (selectedRows.GetCount() == 0) {
        int currentRow = grid->GetGridCursorRow();
        if (currentRow >= 0) {
            SaveState();
            grid->DeleteRows(currentRow, 1);
            UpdateStatusBar();
            SetDirty(true);
        }
    } else {
        SaveState();
        // Delete in reverse order to maintain indices
        for (int i = selectedRows.GetCount() - 1; i >= 0; --i) {
            grid->DeleteRows(selectedRows[i], 1);
        }
        UpdateStatusBar();
        SetDirty(true);
    }
}

void MainFrame::OnDeleteColumn(wxCommandEvent& event) {
    wxArrayInt selectedCols = grid->GetSelectedCols();
    
    if (selectedCols.GetCount() == 0) {
        int currentCol = grid->GetGridCursorCol();
        if (currentCol >= 0) {
            SaveState();
            grid->DeleteCols(currentCol, 1);
            UpdateStatusBar();
            SetDirty(true);
        }
    } else {
        SaveState();
        // Delete in reverse order to maintain indices
        for (int i = selectedCols.GetCount() - 1; i >= 0; --i) {
            grid->DeleteCols(selectedCols[i], 1);
        }
        UpdateStatusBar();
        SetDirty(true);
    }
}

void MainFrame::OnEncodingChange(wxCommandEvent& event) {
    switch (event.GetId()) {
        case ID_ENC_UTF8:
            currentEncoding = Encoding::UTF8;
            break;
        case ID_ENC_ANSI:
            currentEncoding = Encoding::ANSI;
            break;
        case ID_ENC_UTF16:
            currentEncoding = Encoding::UTF16_LE;
            break;
    }
    
    UpdateMenuChecks();
    UpdateStatusBar();
    SetDirty(true);
}

void MainFrame::OnSeparatorChange(wxCommandEvent& event) {
    switch (event.GetId()) {
        case ID_SEP_COMMA:
            currentSeparator = ',';
            break;
        case ID_SEP_SEMICOLON:
            currentSeparator = ';';
            break;
        case ID_SEP_TAB:
            currentSeparator = '\t';
            break;
        case ID_SEP_CUSTOM:
            {
                wxTextEntryDialog dialog(this, Translate("dialog_custom_sep_message", currentLanguage),
                                        Translate("dialog_custom_sep_title", currentLanguage), wxString(currentSeparator));
                if (dialog.ShowModal() == wxID_OK) {
                    wxString value = dialog.GetValue();
                    if (!value.IsEmpty()) {
                        currentSeparator = value[0];
                    }
                }
            }
            break;
    }
    
    UpdateMenuChecks();
    UpdateStatusBar();
    SetDirty(true);
}

void MainFrame::OnEditorShown(wxGridEvent& event) {
    if (!isRestoringState) {
        SaveState();
    }
    event.Skip();
}

void MainFrame::OnCellChanged(wxGridEvent& event) {
    SetDirty(true);
    event.Skip();
}

void MainFrame::OnLabelDoubleClick(wxGridEvent& event) {
    if (event.GetCol() >= 0) {
        // Edit column header
        wxString currentLabel = grid->GetColLabelValue(event.GetCol());
        wxTextEntryDialog dialog(this, Translate("dialog_col_header_message", currentLanguage),
                                Translate("dialog_col_header_title", currentLanguage), currentLabel);
        
        if (dialog.ShowModal() == wxID_OK) {
            SaveState();
            wxString newLabel = dialog.GetValue();
            grid->SetColLabelValue(event.GetCol(), newLabel);
            hasHeaderRow = true;
            SetDirty(true);
        }
    }
    event.Skip();
}

void MainFrame::OnRightClick(wxGridEvent& event) {
    wxMenu menu;
    
    // Helper to load and resize icon for context menu
    auto loadIcon = [](const wxString& filename) -> wxBitmap {
        wxString bmpPath = wxString::Format("resources/%s.bmp", filename);
        if (wxFileExists(bmpPath)) {
            wxBitmap bmp(bmpPath, wxBITMAP_TYPE_BMP);
            if (bmp.IsOk()) {
                // Resize to 16x16 for menu icons
                if (bmp.GetWidth() != 16 || bmp.GetHeight() != 16) {
                    wxImage img = bmp.ConvertToImage();
                    img = img.Scale(16, 16, wxIMAGE_QUALITY_HIGH);
                    bmp = wxBitmap(img);
                }
                return bmp;
            }
        }
        return wxBitmap(16, 16);
    };
    
    wxMenuItem* addRowBelowItem = new wxMenuItem(&menu, ID_ADD_ROW_BELOW, Translate("toolbar_add_row_below", currentLanguage));
    addRowBelowItem->SetBitmap(loadIcon("row-insert-bottom"));
    menu.Append(addRowBelowItem);
    
    wxMenuItem* addRowAboveItem = new wxMenuItem(&menu, ID_ADD_ROW_ABOVE, Translate("toolbar_add_row_above", currentLanguage));
    addRowAboveItem->SetBitmap(loadIcon("row-insert-top"));
    menu.Append(addRowAboveItem);
    
    wxMenuItem* deleteRowItem = new wxMenuItem(&menu, ID_DELETE_ROW, Translate("toolbar_del_row", currentLanguage));
    deleteRowItem->SetBitmap(loadIcon("row-remove"));
    menu.Append(deleteRowItem);
    
    menu.AppendSeparator();
    
    wxMenuItem* addColLeftItem = new wxMenuItem(&menu, ID_ADD_COLUMN_LEFT, Translate("toolbar_add_col_left", currentLanguage));
    addColLeftItem->SetBitmap(loadIcon("column-insert-left"));
    menu.Append(addColLeftItem);
    
    wxMenuItem* addColRightItem = new wxMenuItem(&menu, ID_ADD_COLUMN_RIGHT, Translate("toolbar_add_col_right", currentLanguage));
    addColRightItem->SetBitmap(loadIcon("column-insert-right"));
    menu.Append(addColRightItem);
    
    wxMenuItem* deleteColItem = new wxMenuItem(&menu, ID_DELETE_COLUMN, Translate("toolbar_del_col", currentLanguage));
    deleteColItem->SetBitmap(loadIcon("column-remove"));
    menu.Append(deleteColItem);
    
    PopupMenu(&menu);
    event.Skip();
}

void MainFrame::OnGridRightClick(wxGridEvent& event) {
    OnRightClick(event);
}

void MainFrame::UpdateStatusBar() {
    wxString encStr;
    switch (currentEncoding) {
        case Encoding::UTF8:
        case Encoding::UTF8_BOM:
            encStr = "UTF-8";
            break;
        case Encoding::ANSI:
            encStr = "ANSI";
            break;
        case Encoding::UTF16_LE:
        case Encoding::UTF16_BE:
            encStr = "UTF-16";
            break;
    }
    
    wxString sepStr;
    if (currentSeparator == ',') sepStr = ",";
    else if (currentSeparator == ';') sepStr = ";";
    else if (currentSeparator == '\t') sepStr = "Tab";
    else sepStr = wxString(currentSeparator);
    
    // Get cell coordinates
    wxString coords = "";
    int row = grid->GetGridCursorRow();
    int col = grid->GetGridCursorCol();
    if (row >= 0 && col >= 0) {
        coords = wxString::Format(" [%d,%d]", col, row);
    }
    
    wxString status = wxString::Format("%s: %d | %s: %d | %s: %s | %s: %s%s",
                                      Translate("status_rows", currentLanguage),
                                      grid->GetNumberRows(),
                                      Translate("status_columns", currentLanguage),
                                      grid->GetNumberCols(),
                                      Translate("status_encoding", currentLanguage),
                                      encStr,
                                      Translate("status_separator", currentLanguage),
                                      sepStr,
                                      coords);
    statusBar->SetStatusText(status);
}

void MainFrame::UpdateMenuChecks() {
    wxMenuBar* menuBar = GetMenuBar();
    
    // Update encoding checks
    menuBar->Check(ID_ENC_UTF8, currentEncoding == Encoding::UTF8 || currentEncoding == Encoding::UTF8_BOM);
    menuBar->Check(ID_ENC_ANSI, currentEncoding == Encoding::ANSI);
    menuBar->Check(ID_ENC_UTF16, currentEncoding == Encoding::UTF16_LE || currentEncoding == Encoding::UTF16_BE);
    
    // Update separator checks
    menuBar->Check(ID_SEP_COMMA, currentSeparator == ',');
    menuBar->Check(ID_SEP_SEMICOLON, currentSeparator == ';');
    menuBar->Check(ID_SEP_TAB, currentSeparator == '\t');
    menuBar->Check(ID_SEP_CUSTOM, currentSeparator != ',' && currentSeparator != ';' && currentSeparator != '\t');
}

void MainFrame::SaveState() {
    undoStack.push_back(GetCurrentState());
    if (undoStack.size() > MAX_UNDO_LEVELS) {
        undoStack.pop_front();
    }
    redoStack.clear();
    UpdateUndoRedoButtons();
}

void MainFrame::RestoreState(const GridState& state) {
    isRestoringState = true;
    
    // Save current column widths before clearing
    std::vector<int> savedColWidths;
    for (int col = 0; col < grid->GetNumberCols(); ++col) {
        savedColWidths.push_back(grid->GetColSize(col));
    }
    
    ClearGrid();
    
    if (state.rows > 0 && state.cols > 0) {
        grid->AppendRows(state.rows);
        grid->AppendCols(state.cols);
        
        // Restore headers
        for (int col = 0; col < state.cols && col < (int)state.headers.size(); ++col) {
            grid->SetColLabelValue(col, state.headers[col]);
        }
        
        // Restore previously saved column widths (preserve user adjustments)
        for (int col = 0; col < state.cols && col < (int)savedColWidths.size(); ++col) {
            grid->SetColSize(col, savedColWidths[col]);
        }
        // For any new columns beyond saved widths, use default
        int defaultColWidth = (currentFontSize * 60) / 8;
        for (int col = savedColWidths.size(); col < state.cols; ++col) {
            grid->SetColSize(col, defaultColWidth);
        }
        
        // Apply row heights based on current font size
        int rowHeight = currentFontSize * 2 + 8;
        for (int i = 0; i < grid->GetNumberRows(); ++i) {
            grid->SetRowSize(i, rowHeight);
        }
        grid->SetColLabelSize(rowHeight);
        
        // Restore data
        for (int row = 0; row < state.rows && row < (int)state.data.size(); ++row) {
            for (int col = 0; col < state.cols && col < (int)state.data[row].size(); ++col) {
                grid->SetCellValue(row, col, state.data[row][col]);
            }
        }
    }
    
    isRestoringState = false;
    
    UpdateStatusBar();
}

GridState MainFrame::GetCurrentState() {
    GridState state;
    state.rows = grid->GetNumberRows();
    state.cols = grid->GetNumberCols();
    
    // Save headers
    for (int col = 0; col < state.cols; ++col) {
        state.headers.push_back(grid->GetColLabelValue(col));
    }
    
    // Save data
    for (int row = 0; row < state.rows; ++row) {
        std::vector<wxString> rowData;
        for (int col = 0; col < state.cols; ++col) {
            rowData.push_back(grid->GetCellValue(row, col));
        }
        state.data.push_back(rowData);
    }
    
    return state;
}

void MainFrame::ClearGrid() {
    if (grid->GetNumberRows() > 0) {
        grid->DeleteRows(0, grid->GetNumberRows());
    }
    if (grid->GetNumberCols() > 0) {
        grid->DeleteCols(0, grid->GetNumberCols());
    }
}

void MainFrame::SetDirty(bool dirty) {
    isDirty = dirty;
    wxString title = "CSV++";
    if (!currentFile.IsEmpty()) {
        title += " - " + wxFileName(currentFile).GetFullName();
    }
    if (isDirty) {
        title += " *";
    }
    SetTitle(title);
}

bool MainFrame::PromptSaveChanges() {
    if (isDirty) {
        int result = wxMessageBox(Translate("prompt_save_message", currentLanguage), Translate("prompt_save_title", currentLanguage),
                                 wxYES_NO | wxCANCEL | wxICON_QUESTION);
        if (result == wxYES) {
            wxCommandEvent dummy;
            OnSave(dummy);
            return !isDirty; // Return false if save was cancelled
        } else if (result == wxCANCEL) {
            return false;
        }
    }
    return true;
}

void MainFrame::UpdateUndoRedoButtons() {
    toolBar->EnableTool(ID_UNDO, !undoStack.empty());
    toolBar->EnableTool(ID_REDO, !redoStack.empty());
}

void MainFrame::OnColSize(wxGridSizeEvent& event) {
    // Don't save state on column resize - just let it happen
    event.Skip();
}

void MainFrame::OnSelectCell(wxGridEvent& event) {
    event.Skip();
    if (grid && statusBar) {
        // Get coordinates directly from the event
        int row = event.GetRow();
        int col = event.GetCol();
        if (row >= 0 && col >= 0) {
            wxString coords = wxString::Format("[%d,%d]", col, row);
            // Update just the coordinates part of status bar
            wxString encStr;
            switch (currentEncoding) {
                case Encoding::UTF8:
                case Encoding::UTF8_BOM:
                    encStr = "UTF-8";
                    break;
                case Encoding::ANSI:
                    encStr = "ANSI";
                    break;
                case Encoding::UTF16_LE:
                case Encoding::UTF16_BE:
                    encStr = "UTF-16";
                    break;
            }
            
            wxString sepStr;
            if (currentSeparator == ',') sepStr = ",";
            else if (currentSeparator == ';') sepStr = ";";
            else if (currentSeparator == '\t') sepStr = "Tab";
            else sepStr = wxString(currentSeparator);
            
            wxString status = wxString::Format("%s: %d | %s: %d | %s: %s | %s: %s %s",
                                              Translate("status_rows", currentLanguage),
                                              grid->GetNumberRows(),
                                              Translate("status_columns", currentLanguage),
                                              grid->GetNumberCols(),
                                              Translate("status_encoding", currentLanguage),
                                              encStr,
                                              Translate("status_separator", currentLanguage),
                                              sepStr,
                                              coords);
            statusBar->SetStatusText(status);
        }
    }
}

void MainFrame::OnLanguageChange(wxCommandEvent& event) {
    if (event.GetId() == ID_LANG_ENGLISH) {
        currentLanguage = LANGUAGE_ENGLISH;
    } else if (event.GetId() == ID_LANG_SERBIAN) {
        currentLanguage = LANGUAGE_SERBIAN;
    }
    
    // Save language setting to registry
    wxConfig config("CSV++");
    config.Write("Language", (long)currentLanguage);
    config.Flush();
    
    UpdateUILanguage();
}

void MainFrame::OnFontSizeChange(wxCommandEvent& event) {
    int selection = fontSizeChoice->GetSelection();
    if (selection != wxNOT_FOUND) {
        wxString sizeStr = fontSizeChoice->GetString(selection);
        currentFontSize = wxAtoi(sizeStr);
        
        // Apply font size to all cells
        wxFont font = grid->GetDefaultCellFont();
        font.SetPointSize(currentFontSize);
        grid->SetDefaultCellFont(font);
        
        // Apply to label cells as well
        wxFont labelFont = grid->GetLabelFont();
        labelFont.SetPointSize(currentFontSize);
        grid->SetLabelFont(labelFont);
        
        // Apply grid dimensions (rows and columns) based on font size
        ApplyGridDimensions();
        
        // Refresh grid
        grid->ForceRefresh();
    }
}

void MainFrame::ApplyGridDimensions() {
    // Calculate row height based on font size
    // Formula: fontSize * 2 + 8 pixels
    int rowHeight = currentFontSize * 2 + 8;
    
    // Set row heights for all rows
    for (int i = 0; i < grid->GetNumberRows(); ++i) {
        grid->SetRowSize(i, rowHeight);
    }
    
    // Set column label height
    grid->SetColLabelSize(rowHeight);
    
    // Calculate column width based on font size
    // Base width for font size 8 is 60 pixels
    int colWidth = (currentFontSize * 60) / 8;
    
    // Set column widths for all columns
    for (int i = 0; i < grid->GetNumberCols(); ++i) {
        grid->SetColSize(i, colWidth);
    }
}

void MainFrame::UpdateUILanguage() {
    // Recreate menu bar with new language
    SetMenuBar(nullptr);
    CreateMenuBar();
    
    // Update toolbar
    toolBar->Destroy();
    CreateToolBar();
    
    // Update menu checks
    UpdateMenuChecks();
    
    // Check language menu
    wxMenuBar* menuBar = GetMenuBar();
    menuBar->Check(ID_LANG_ENGLISH, currentLanguage == LANGUAGE_ENGLISH);
    menuBar->Check(ID_LANG_SERBIAN, currentLanguage == LANGUAGE_SERBIAN);
    
    // Update status bar
    UpdateStatusBar();
    
    // Update undo/redo buttons
    UpdateUndoRedoButtons();
}

void MainFrame::OnInstructions(wxCommandEvent& event) {
    wxString htmlFile = (currentLanguage == LANGUAGE_SERBIAN) ? 
        "resources/instructions_sr.html" : "resources/instructions_en.html";
    
    if (!wxFileExists(htmlFile)) {
        wxMessageBox(Translate("error_instructions_not_found", currentLanguage), 
                    "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    wxDialog* dlg = new wxDialog(this, wxID_ANY, Translate("menu_help_instructions", currentLanguage), 
                                 wxDefaultPosition, wxSize(700, 500));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    wxHtmlWindow* html = new wxHtmlWindow(dlg, wxID_ANY, wxDefaultPosition, 
                                          wxSize(680, 450), wxHW_SCROLLBAR_AUTO);
    html->LoadPage(htmlFile);
    sizer->Add(html, 1, wxALL | wxEXPAND, 10);
    
    wxButton* closeBtn = new wxButton(dlg, wxID_OK, Translate("button_close", currentLanguage));
    sizer->Add(closeBtn, 0, wxALL | wxALIGN_CENTER, 10);
    
    dlg->SetSizer(sizer);
    dlg->ShowModal();
    dlg->Destroy();
}

void MainFrame::OnAbout(wxCommandEvent& event) {
    wxString aboutText = (currentLanguage == LANGUAGE_SERBIAN) ?
        wxString::FromUTF8("CSV++ - Aplikacija za uređivanje CSV datoteka\n\n"
                          "Verzija 1.0\n\n"
                          "Jednostavna i brza aplikacija za rad sa CSV datotekama.\n"
                          "Podržava različite kodiranja i separatore.\n\n"
                          "GitHub repozitorijum:\n") :
        wxString("CSV++ - CSV File Editor\n\n"
                "Version 1.0\n\n"
                "Simple and fast application for working with CSV files.\n"
                "Supports different encodings and separators.\n\n"
                "GitHub Repository:\n");
    
    wxDialog* dlg = new wxDialog(this, wxID_ANY, Translate("menu_help_about", currentLanguage),
                                 wxDefaultPosition, wxSize(400, 250));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* text = new wxStaticText(dlg, wxID_ANY, aboutText);
    sizer->Add(text, 0, wxALL, 15);
    
    wxHyperlinkCtrl* link = new wxHyperlinkCtrl(dlg, wxID_ANY, 
        "https://github.com/YourUsername/CSV--",
        "https://github.com/YourUsername/CSV--");
    sizer->Add(link, 0, wxALL | wxALIGN_CENTER, 10);
    
    wxButton* closeBtn = new wxButton(dlg, wxID_OK, "OK");
    sizer->Add(closeBtn, 0, wxALL | wxALIGN_CENTER, 10);
    
    dlg->SetSizer(sizer);
    dlg->ShowModal();
    dlg->Destroy();
}


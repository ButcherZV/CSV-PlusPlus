#include "MainFrame.h"
#include "CSVOptionsDialog.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/filename.h>
#include <wx/artprov.h>
#include <wx/dcmemory.h>
#include <wx/mstream.h>

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
    EVT_MENU(ID_ADD_ROW, MainFrame::OnAddRow)
    EVT_MENU(ID_ADD_COLUMN, MainFrame::OnAddColumn)
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
    EVT_GRID_CELL_CHANGED(MainFrame::OnCellChanged)
    EVT_GRID_LABEL_LEFT_DCLICK(MainFrame::OnLabelDoubleClick)
    EVT_GRID_CELL_RIGHT_CLICK(MainFrame::OnRightClick)
    EVT_GRID_LABEL_RIGHT_CLICK(MainFrame::OnGridRightClick)
    EVT_GRID_COL_SIZE(MainFrame::OnColSize)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 600)),
      currentEncoding(Encoding::UTF8),
      currentSeparator(','),
      isDirty(false),
      hasHeaderRow(false),
      currentLanguage(LANGUAGE_ENGLISH) {
    
    // Create grid
    grid = new wxGrid(this, wxID_ANY);
    grid->CreateGrid(10, 5);
    grid->EnableEditing(true);
    grid->EnableDragGridSize(true);
    grid->EnableDragColSize(true);
    grid->EnableDragRowSize(false);
    
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
    
    Centre();
}

void MainFrame::CreateMenuBar() {
    wxMenuBar* menuBar = new wxMenuBar();
    
    // File menu
    wxMenu* fileMenu = new wxMenu();
    fileMenu->Append(ID_NEW, Translate("menu_new"), Translate("menu_new_desc"));
    fileMenu->Append(ID_OPEN, Translate("menu_open"), Translate("menu_open_desc"));
    fileMenu->Append(ID_SAVE, Translate("menu_save"), Translate("menu_save_desc"));
    fileMenu->Append(ID_CLOSE, Translate("menu_close"), Translate("menu_close_desc"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, Translate("menu_exit"), Translate("menu_exit_desc"));
    menuBar->Append(fileMenu, Translate("menu_file"));
    
    // Settings menu
    wxMenu* settingsMenu = new wxMenu();
    
    // Encoding submenu
    wxMenu* encodingMenu = new wxMenu();
    encodingMenu->AppendRadioItem(ID_ENC_UTF8, "UTF-8");
    encodingMenu->AppendRadioItem(ID_ENC_ANSI, "ANSI");
    encodingMenu->AppendRadioItem(ID_ENC_UTF16, "UTF-16");
    settingsMenu->AppendSubMenu(encodingMenu, Translate("menu_encoding"));
    
    // Separator submenu
    wxMenu* separatorMenu = new wxMenu();
    separatorMenu->AppendRadioItem(ID_SEP_COMMA, Translate("menu_sep_comma"));
    separatorMenu->AppendRadioItem(ID_SEP_SEMICOLON, Translate("menu_sep_semicolon"));
    separatorMenu->AppendRadioItem(ID_SEP_TAB, Translate("menu_sep_tab"));
    separatorMenu->AppendRadioItem(ID_SEP_CUSTOM, Translate("menu_sep_custom"));
    settingsMenu->AppendSubMenu(separatorMenu, Translate("menu_separator"));
    
    // Language submenu
    wxMenu* languageMenu = new wxMenu();
    languageMenu->AppendRadioItem(ID_LANG_ENGLISH, "English");
    languageMenu->AppendRadioItem(ID_LANG_SERBIAN, "Srpski");
    settingsMenu->AppendSubMenu(languageMenu, Translate("menu_language"));
    
    menuBar->Append(settingsMenu, Translate("menu_settings"));
    
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
        return wxBitmap(48, 48);
    };
    
    toolBar->AddTool(ID_NEW, Translate("toolbar_new"), loadIcon("file"), Translate("toolbar_new_hint"));
    toolBar->AddTool(ID_OPEN, Translate("toolbar_open"), loadIcon("folder"), Translate("toolbar_open_hint"));
    toolBar->AddTool(ID_SAVE, Translate("toolbar_save"), loadIcon("device-floppy"), Translate("toolbar_save_hint"));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_UNDO, Translate("toolbar_undo"), loadIcon("undo"), Translate("toolbar_undo_hint"), wxITEM_NORMAL, loadIcon("undo-disabled"));
    toolBar->AddTool(ID_REDO, Translate("toolbar_redo"), loadIcon("redo"), Translate("toolbar_redo_hint"), wxITEM_NORMAL, loadIcon("redo-disabled"));
    toolBar->AddSeparator();
    toolBar->AddTool(ID_ADD_ROW, Translate("toolbar_add_row"), loadIcon("row-insert-bottom"), Translate("toolbar_add_row_hint"));
    toolBar->AddTool(ID_ADD_COLUMN, Translate("toolbar_add_col"), loadIcon("column-insert-right"), Translate("toolbar_add_col_hint"));
    toolBar->AddTool(ID_DELETE_ROW, Translate("toolbar_del_row"), loadIcon("row-remove"), Translate("toolbar_del_row_hint"));
    toolBar->AddTool(ID_DELETE_COLUMN, Translate("toolbar_del_col"), loadIcon("column-remove"), Translate("toolbar_del_col_hint"));
    
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

void MainFrame::OnAddRow(wxCommandEvent& event) {
    SaveState();
    
    int currentRow = grid->GetGridCursorRow();
    if (currentRow < 0) {
        currentRow = grid->GetNumberRows();
    }
    
    grid->InsertRows(currentRow, 1);
    UpdateStatusBar();
    SetDirty(true);
}

void MainFrame::OnAddColumn(wxCommandEvent& event) {
    SaveState();
    
    int currentCol = grid->GetGridCursorCol();
    if (currentCol < 0) {
        currentCol = grid->GetNumberCols();
    }
    
    grid->InsertCols(currentCol, 1);
    
    // Set default label
    wxChar label = 'A' + currentCol;
    if (currentCol < 26) {
        grid->SetColLabelValue(currentCol, wxString(label));
    } else {
        grid->SetColLabelValue(currentCol, wxString::Format("Col%d", currentCol + 1));
    }
    
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
                wxTextEntryDialog dialog(this, "Enter custom separator character:",
                                        "Custom Separator", wxString(currentSeparator));
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

void MainFrame::OnCellChanged(wxGridEvent& event) {
    SaveState();
    SetDirty(true);
    event.Skip();
}

void MainFrame::OnLabelDoubleClick(wxGridEvent& event) {
    if (event.GetCol() >= 0) {
        // Edit column header
        wxString currentLabel = grid->GetColLabelValue(event.GetCol());
        wxTextEntryDialog dialog(this, "Enter column header:",
                                "Edit Column Header", currentLabel);
        
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
    menu.Append(ID_ADD_ROW, "Insert Row");
    menu.Append(ID_DELETE_ROW, "Delete Row");
    menu.AppendSeparator();
    menu.Append(ID_ADD_COLUMN, "Insert Column");
    menu.Append(ID_DELETE_COLUMN, "Delete Column");
    
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
    
    wxString status = wxString::Format("%s: %d | %s: %d | %s: %s | %s: %s",
                                      Translate("status_rows"),
                                      grid->GetNumberRows(),
                                      Translate("status_columns"),
                                      grid->GetNumberCols(),
                                      Translate("status_encoding"),
                                      encStr,
                                      Translate("status_separator"),
                                      sepStr);
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
    ClearGrid();
    
    if (state.rows > 0 && state.cols > 0) {
        grid->AppendRows(state.rows);
        grid->AppendCols(state.cols);
        
        // Restore headers
        for (int col = 0; col < state.cols && col < (int)state.headers.size(); ++col) {
            grid->SetColLabelValue(col, state.headers[col]);
        }
        
        // Restore data
        for (int row = 0; row < state.rows && row < (int)state.data.size(); ++row) {
            for (int col = 0; col < state.cols && col < (int)state.data[row].size(); ++col) {
                grid->SetCellValue(row, col, state.data[row][col]);
            }
        }
    }
    
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
        int result = wxMessageBox("Do you want to save changes?", "Save Changes",
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

void MainFrame::OnLanguageChange(wxCommandEvent& event) {
    if (event.GetId() == ID_LANG_ENGLISH) {
        currentLanguage = LANGUAGE_ENGLISH;
    } else if (event.GetId() == ID_LANG_SERBIAN) {
        currentLanguage = LANGUAGE_SERBIAN;
    }
    
    UpdateUILanguage();
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

wxString MainFrame::Translate(const wxString& key) {
    if (currentLanguage == LANGUAGE_SERBIAN) {
        // Serbian translations with proper UTF-8 characters
        if (key == "menu_file") return wxString::FromUTF8("&Fajl");
        if (key == "menu_new") return wxString::FromUTF8("&Novo\tCtrl+N");
        if (key == "menu_new_desc") return wxString::FromUTF8("Napravi novu CSV datoteku");
        if (key == "menu_open") return wxString::FromUTF8("&Otvori...\tCtrl+O");
        if (key == "menu_open_desc") return wxString::FromUTF8("Otvori CSV datoteku");
        if (key == "menu_save") return wxString::FromUTF8("&Sačuvaj\tCtrl+S");
        if (key == "menu_save_desc") return wxString::FromUTF8("Sačuvaj CSV datoteku");
        if (key == "menu_close") return wxString::FromUTF8("&Zatvori");
        if (key == "menu_close_desc") return wxString::FromUTF8("Zatvori trenutnu datoteku");
        if (key == "menu_exit") return wxString::FromUTF8("&Izlaz");
        if (key == "menu_exit_desc") return wxString::FromUTF8("Izlaz iz aplikacije");
        
        if (key == "menu_settings") return wxString::FromUTF8("&Podešavanja");
        if (key == "menu_encoding") return wxString::FromUTF8("Kodiranje");
        if (key == "menu_separator") return wxString::FromUTF8("Separator");
        if (key == "menu_language") return wxString::FromUTF8("Jezik");
        if (key == "menu_sep_comma") return wxString::FromUTF8("Zapeta (,)");
        if (key == "menu_sep_semicolon") return wxString::FromUTF8("Tačka-zapeta (;)");
        if (key == "menu_sep_tab") return wxString::FromUTF8("Tab");
        if (key == "menu_sep_custom") return wxString::FromUTF8("Prilagođeno...");
        
        if (key == "toolbar_new") return wxString::FromUTF8("Novo");
        if (key == "toolbar_new_hint") return wxString::FromUTF8("Nova datoteka");
        if (key == "toolbar_open") return wxString::FromUTF8("Otvori");
        if (key == "toolbar_open_hint") return wxString::FromUTF8("Otvori datoteku");
        if (key == "toolbar_save") return wxString::FromUTF8("Sačuvaj");
        if (key == "toolbar_save_hint") return wxString::FromUTF8("Sačuvaj datoteku");
        if (key == "toolbar_undo") return wxString::FromUTF8("Poništi");
        if (key == "toolbar_undo_hint") return wxString::FromUTF8("Poništi");
        if (key == "toolbar_redo") return wxString::FromUTF8("Ponovi");
        if (key == "toolbar_redo_hint") return wxString::FromUTF8("Ponovi");
        if (key == "toolbar_add_row") return wxString::FromUTF8("Dodaj red");
        if (key == "toolbar_add_row_hint") return wxString::FromUTF8("Dodaj red");
        if (key == "toolbar_add_col") return wxString::FromUTF8("Dodaj kolonu");
        if (key == "toolbar_add_col_hint") return wxString::FromUTF8("Dodaj kolonu");
        if (key == "toolbar_del_row") return wxString::FromUTF8("Obriši red");
        if (key == "toolbar_del_row_hint") return wxString::FromUTF8("Obriši red");
        if (key == "toolbar_del_col") return wxString::FromUTF8("Obriši kolonu");
        if (key == "toolbar_del_col_hint") return wxString::FromUTF8("Obriši kolonu");
        
        if (key == "status_rows") return wxString::FromUTF8("Redova");
        if (key == "status_columns") return wxString::FromUTF8("Kolona");
        if (key == "status_encoding") return wxString::FromUTF8("Kodiranje");
        if (key == "status_separator") return wxString::FromUTF8("Separator");
    }
    
    // Default English
    if (key == "menu_file") return "&File";
    if (key == "menu_new") return "&New\tCtrl+N";
    if (key == "menu_new_desc") return "Create new CSV file";
    if (key == "menu_open") return "&Open...\tCtrl+O";
    if (key == "menu_open_desc") return "Open CSV file";
    if (key == "menu_save") return "&Save\tCtrl+S";
    if (key == "menu_save_desc") return "Save CSV file";
    if (key == "menu_close") return "&Close";
    if (key == "menu_close_desc") return "Close current file";
    if (key == "menu_exit") return "E&xit";
    if (key == "menu_exit_desc") return "Exit application";
    
    if (key == "menu_settings") return "&Settings";
    if (key == "menu_encoding") return "Encoding";
    if (key == "menu_separator") return "Separator";
    if (key == "menu_language") return "Language";
    if (key == "menu_sep_comma") return "Comma (,)";
    if (key == "menu_sep_semicolon") return "Semicolon (;)";
    if (key == "menu_sep_tab") return "Tab";
    if (key == "menu_sep_custom") return "Custom...";
    
    if (key == "toolbar_new") return "New";
    if (key == "toolbar_new_hint") return "New file";
    if (key == "toolbar_open") return "Open";
    if (key == "toolbar_open_hint") return "Open file";
    if (key == "toolbar_save") return "Save";
    if (key == "toolbar_save_hint") return "Save file";
    if (key == "toolbar_undo") return "Undo";
    if (key == "toolbar_undo_hint") return "Undo";
    if (key == "toolbar_redo") return "Redo";
    if (key == "toolbar_redo_hint") return "Redo";
    if (key == "toolbar_add_row") return "Add Row";
    if (key == "toolbar_add_row_hint") return "Add row";
    if (key == "toolbar_add_col") return "Add Column";
    if (key == "toolbar_add_col_hint") return "Add column";
    if (key == "toolbar_del_row") return "Delete Row";
    if (key == "toolbar_del_row_hint") return "Delete row";
    if (key == "toolbar_del_col") return "Delete Column";
    if (key == "toolbar_del_col_hint") return "Delete column";
    
    if (key == "status_rows") return "Rows";
    if (key == "status_columns") return "Columns";
    if (key == "status_encoding") return "Encoding";
    if (key == "status_separator") return "Separator";
    
    return key; // Return key if not found
}

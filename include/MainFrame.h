#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/dnd.h>
#include <deque>
#include "CSVParser.h"
#include "Translations.h"

// Structure to store grid state for undo/redo
struct GridState {
    std::vector<std::vector<wxString>> data;
    std::vector<wxString> headers;
    int rows;
    int cols;
};

// File drop target class
class FileDropTarget : public wxFileDropTarget {
public:
    FileDropTarget(class MainFrame* frame) : mainFrame(frame) {}
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override;
    
private:
    MainFrame* mainFrame;
};

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);
    
    void OpenFileFromDrop(const wxString& filename);
    
    wxDECLARE_EVENT_TABLE();
    
private:
    wxGrid* grid;
    wxStatusBar* statusBar;
    wxToolBar* toolBar;
    wxChoice* fontSizeChoice;
    int currentFontSize;
    
    // File state
    wxString currentFile;
    Encoding currentEncoding;
    wxChar currentSeparator;
    bool isDirty;
    bool hasHeaderRow;
    bool isRestoringState;
    
    // Undo/redo
    std::deque<GridState> undoStack;
    std::deque<GridState> redoStack;
    const size_t MAX_UNDO_LEVELS = 50;
    
    // Language support
    Language currentLanguage;
    
    // Menu IDs
    enum {
        ID_NEW = wxID_HIGHEST + 1,
        ID_OPEN,
        ID_SAVE,
        ID_CLOSE,
        ID_UNDO,
        ID_REDO,
        ID_ADD_ROW_BELOW,
        ID_ADD_ROW_ABOVE,
        ID_ADD_COLUMN_LEFT,
        ID_ADD_COLUMN_RIGHT,
        ID_DELETE_ROW,
        ID_DELETE_COLUMN,
        ID_ENC_UTF8,
        ID_ENC_ANSI,
        ID_ENC_UTF16,
        ID_SEP_COMMA,
        ID_SEP_SEMICOLON,
        ID_SEP_TAB,
        ID_SEP_CUSTOM,
        ID_LANG_ENGLISH,
        ID_LANG_SERBIAN,
        ID_FONT_SIZE_CHOICE,
        ID_HELP_INSTRUCTIONS,
        ID_HELP_ABOUT
    };
    
    // UI Creation
    void CreateMenuBar();
    void CreateToolBar();
    void CreateStatusBar();
    
    // File operations
    void OnNew(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    
    // Edit operations
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
    void OnAddRowBelow(wxCommandEvent& event);
    void OnAddRowAbove(wxCommandEvent& event);
    void OnAddColumnLeft(wxCommandEvent& event);
    void OnAddColumnRight(wxCommandEvent& event);
    void OnDeleteRow(wxCommandEvent& event);
    void OnDeleteColumn(wxCommandEvent& event);
    
    // Settings
    void OnEncodingChange(wxCommandEvent& event);
    void OnSeparatorChange(wxCommandEvent& event);
    void OnLanguageChange(wxCommandEvent& event);
    void OnFontSizeChange(wxCommandEvent& event);
    
    // Help
    void OnInstructions(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    
    // Grid events
    void OnEditorShown(wxGridEvent& event);
    void OnCellChanged(wxGridEvent& event);
    void OnLabelDoubleClick(wxGridEvent& event);
    void OnRightClick(wxGridEvent& event);
    void OnGridRightClick(wxGridEvent& event);
    void OnColSize(wxGridSizeEvent& event);
    void OnSelectCell(wxGridEvent& event);
    
    // Helper methods
    void LoadCSVFile(const wxString& filename, Encoding encoding, 
                     wxChar separator, bool hasHeader);
    void SaveCSVFile(const wxString& filename);
    void UpdateStatusBar();
    void UpdateMenuChecks();
    void SaveState();
    void RestoreState(const GridState& state);
    GridState GetCurrentState();
    void ClearGrid();
    void SetDirty(bool dirty);
    bool PromptSaveChanges();
    void UpdateUndoRedoButtons();
    void UpdateUILanguage();
    void ApplyGridDimensions();
};

#endif // MAINFRAME_H

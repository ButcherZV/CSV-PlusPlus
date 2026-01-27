#include "Translations.h"

wxString Translate(const wxString& key, Language language) {
    if (language == LANGUAGE_SERBIAN) {
        // Menu translations
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
        if (key == "menu_sep_custom") return wxString::FromUTF8("Ostalo...");
        
        // Toolbar translations
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
        if (key == "toolbar_add_row_below") return wxString::FromUTF8("Dodaj red ispod");
        if (key == "toolbar_add_row_below_hint") return wxString::FromUTF8("Dodaj red ispod trenutnog");
        if (key == "toolbar_add_row_above") return wxString::FromUTF8("Dodaj red iznad");
        if (key == "toolbar_add_row_above_hint") return wxString::FromUTF8("Dodaj red iznad trenutnog");
        if (key == "toolbar_del_row") return wxString::FromUTF8("Obriši red");
        if (key == "toolbar_del_row_hint") return wxString::FromUTF8("Obriši red");
        if (key == "toolbar_add_col_left") return wxString::FromUTF8("Dodaj kolonu levo");
        if (key == "toolbar_add_col_left_hint") return wxString::FromUTF8("Dodaj kolonu levo od trenutne");
        if (key == "toolbar_add_col_right") return wxString::FromUTF8("Dodaj kolonu desno");
        if (key == "toolbar_add_col_right_hint") return wxString::FromUTF8("Dodaj kolonu desno od trenutne");
        if (key == "toolbar_del_col") return wxString::FromUTF8("Obriši kolonu");
        if (key == "toolbar_del_col_hint") return wxString::FromUTF8("Obriši kolonu");
        if (key == "toolbar_font_size") return wxString::FromUTF8("Veličina fonta");
        
        // Status bar translations
        if (key == "status_rows") return wxString::FromUTF8("Redova");
        if (key == "status_columns") return wxString::FromUTF8("Kolona");
        if (key == "status_encoding") return wxString::FromUTF8("Kodiranje");
        if (key == "status_separator") return wxString::FromUTF8("Separator");
        
        // Dialog translations
        if (key == "prompt_save_title") return wxString::FromUTF8("Sačuvaj izmene");
        if (key == "prompt_save_message") return wxString::FromUTF8("Da li želite da sačuvate izmene?");
        if (key == "dialog_custom_sep_title") return wxString::FromUTF8("Prilagođeni separator");
        if (key == "dialog_custom_sep_message") return wxString::FromUTF8("Unesite željeni separator:");
        if (key == "dialog_col_header_title") return wxString::FromUTF8("Naziv kolone");
        if (key == "dialog_col_header_message") return wxString::FromUTF8("Unesite naziv kolone:");
        
        if (key == "menu_help") return wxString::FromUTF8("&Pomoć");
        if (key == "menu_help_instructions") return wxString::FromUTF8("Uputstvo");
        if (key == "menu_help_instructions_desc") return wxString::FromUTF8("Prikaži uputstvo za upotrebu");
        if (key == "menu_help_about") return wxString::FromUTF8("O programu");
        if (key == "menu_help_about_desc") return wxString::FromUTF8("Informacije o aplikaciji");
        if (key == "button_close") return wxString::FromUTF8("Zatvori");
        if (key == "error_instructions_not_found") return wxString::FromUTF8("Datoteka sa uputstvom nije pronađena!");
        
        if (key == "about_title") return wxString::FromUTF8("CSV++ - Aplikacija za ure đivanje CSV datoteka");
        if (key == "about_version") return wxString::FromUTF8("Verzija 1.0");
        if (key == "about_description1") return wxString::FromUTF8("Jednostavna i brza aplikacija za rad sa CSV datotekama.");
        if (key == "about_description2") return wxString::FromUTF8("Podržava različite formate kodiranja i separatore.");
        if (key == "about_license") return wxString::FromUTF8("Licenca: Apache License 2.0");
        if (key == "about_github") return wxString::FromUTF8("GitHub repo:");
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
    if (key == "toolbar_add_row_below") return "Add Row Below";
    if (key == "toolbar_add_row_below_hint") return "Add row below current";
    if (key == "toolbar_add_row_above") return "Add Row Above";
    if (key == "toolbar_add_row_above_hint") return "Add row above current";
    if (key == "toolbar_del_row") return "Delete Row";
    if (key == "toolbar_del_row_hint") return "Delete row";
    if (key == "toolbar_add_col_left") return "Add Column Left";
    if (key == "toolbar_add_col_left_hint") return "Add column left of current";
    if (key == "toolbar_add_col_right") return "Add Column Right";
    if (key == "toolbar_add_col_right_hint") return "Add column right of current";
    if (key == "toolbar_del_col") return "Delete Column";
    if (key == "toolbar_del_col_hint") return "Delete column";
    if (key == "toolbar_font_size") return "Font Size";
    
    if (key == "status_rows") return "Rows";
    if (key == "status_columns") return "Columns";
    if (key == "status_encoding") return "Encoding";
    if (key == "status_separator") return "Separator";
    
    if (key == "prompt_save_title") return "Save Changes";
    if (key == "prompt_save_message") return "Do you want to save changes?";
    if (key == "dialog_custom_sep_title") return "Custom Separator";
    if (key == "dialog_custom_sep_message") return "Enter custom separator character:";
    if (key == "dialog_col_header_title") return "Edit Column Header";
    if (key == "dialog_col_header_message") return "Enter column header:";
    
    if (key == "menu_help") return "&Help";
    if (key == "menu_help_instructions") return "Instructions";
    if (key == "menu_help_instructions_desc") return "Show usage instructions";
    if (key == "menu_help_about") return "About";
    if (key == "menu_help_about_desc") return "About this application";
    if (key == "button_close") return "Close";
    if (key == "error_instructions_not_found") return "Instructions file not found!";
    
    if (key == "about_title") return "CSV++ - CSV File Editor";
    if (key == "about_version") return "Version 1.0";
    if (key == "about_description1") return "Simple and fast application for working with CSV files.";
    if (key == "about_description2") return "Supports different encodings and separators.";
    if (key == "about_license") return "License: Apache License 2.0";
    if (key == "about_github") return "GitHub Repository:";
    
    return key; // Return key if not found
}

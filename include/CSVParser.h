#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <wx/wx.h>
#include <vector>

enum class Encoding {
    UTF8,
    UTF8_BOM,
    ANSI,
    UTF16_LE,
    UTF16_BE
};

class CSVParser {
public:
    CSVParser();
    ~CSVParser();
    
    // Read CSV file and return 2D vector of strings
    bool ReadFile(const wxString& filename, std::vector<std::vector<wxString>>& data, 
                  wxChar& separator, Encoding& encoding, bool& hasHeader);
    
    // Write CSV file from 2D vector
    bool WriteFile(const wxString& filename, const std::vector<std::vector<wxString>>& data,
                   wxChar separator, Encoding encoding);
    
    // Auto-detect separator from content
    static wxChar DetectSeparator(const wxString& content);
    
    // Auto-detect encoding from file
    static Encoding DetectEncoding(const wxString& filename);
    
    // Parse a single CSV line respecting quotes
    static std::vector<wxString> ParseLine(const wxString& line, wxChar separator);
    
    // Format a line with proper quoting
    static wxString FormatLine(const std::vector<wxString>& fields, wxChar separator);
    
private:
    // Check if field needs quoting
    static bool NeedsQuoting(const wxString& field, wxChar separator);
    
    // Escape quotes in field
    static wxString EscapeField(const wxString& field);
    
    // Unescape quotes in field
    static wxString UnescapeField(const wxString& field);
};

#endif // CSVPARSER_H

#include "CSVParser.h"
#include <wx/textfile.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>

CSVParser::CSVParser() {
}

CSVParser::~CSVParser() {
}

Encoding CSVParser::DetectEncoding(const wxString& filename) {
    wxFileInputStream file(filename);
    if (!file.IsOk()) {
        return Encoding::UTF8;
    }
    
    // Read first few bytes to check for BOM
    unsigned char bom[4] = {0};
    file.Read(bom, 4);
    
    // Check for UTF-8 BOM (EF BB BF)
    if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
        return Encoding::UTF8_BOM;
    }
    
    // Check for UTF-16 LE BOM (FF FE)
    if (bom[0] == 0xFF && bom[1] == 0xFE) {
        return Encoding::UTF16_LE;
    }
    
    // Check for UTF-16 BE BOM (FE FF)
    if (bom[0] == 0xFE && bom[1] == 0xFF) {
        return Encoding::UTF16_BE;
    }
    
    // Default to UTF-8
    return Encoding::UTF8;
}

wxChar CSVParser::DetectSeparator(const wxString& content) {
    // Count occurrences of common separators in first 10 lines
    int commaCount = 0;
    int semicolonCount = 0;
    int tabCount = 0;
    
    wxStringTokenizer tokenizer(content, wxT("\n\r"), wxTOKEN_STRTOK);
    int lineCount = 0;
    
    while (tokenizer.HasMoreTokens() && lineCount < 10) {
        wxString line = tokenizer.GetNextToken();
        commaCount += line.Freq(',');
        semicolonCount += line.Freq(';');
        tabCount += line.Freq('\t');
        lineCount++;
    }
    
    // Return the most common separator
    if (tabCount > commaCount && tabCount > semicolonCount) {
        return '\t';
    } else if (semicolonCount > commaCount) {
        return ';';
    } else {
        return ',';
    }
}

std::vector<wxString> CSVParser::ParseLine(const wxString& line, wxChar separator) {
    std::vector<wxString> fields;
    wxString field;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        wxChar c = line[i];
        
        if (c == '"') {
            // Check for escaped quote (double quote)
            if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                field += '"';
                ++i; // Skip next quote
            } else {
                // Toggle quote state
                inQuotes = !inQuotes;
            }
        } else if (c == separator && !inQuotes) {
            // End of field
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    
    // Add last field
    fields.push_back(field);
    
    return fields;
}

wxString CSVParser::FormatLine(const std::vector<wxString>& fields, wxChar separator) {
    wxString line;
    
    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) {
            line += separator;
        }
        
        wxString field = fields[i];
        
        if (NeedsQuoting(field, separator)) {
            line += '"';
            line += EscapeField(field);
            line += '"';
        } else {
            line += field;
        }
    }
    
    return line;
}

bool CSVParser::NeedsQuoting(const wxString& field, wxChar separator) {
    // Field needs quoting if it contains separator, quotes, or newlines
    return field.Find(separator) != wxNOT_FOUND ||
           field.Find('"') != wxNOT_FOUND ||
           field.Find('\n') != wxNOT_FOUND ||
           field.Find('\r') != wxNOT_FOUND;
}

wxString CSVParser::EscapeField(const wxString& field) {
    wxString escaped;
    for (size_t i = 0; i < field.length(); ++i) {
        wxChar c = field[i];
        if (c == '"') {
            escaped += "\"\""; // Double the quote
        } else {
            escaped += c;
        }
    }
    return escaped;
}

wxString CSVParser::UnescapeField(const wxString& field) {
    return field;
}

bool CSVParser::ReadFile(const wxString& filename, std::vector<std::vector<wxString>>& data,
                        wxChar& separator, Encoding& encoding, bool& hasHeader) {
    data.clear();
    
    // Detect encoding
    encoding = DetectEncoding(filename);
    
    wxTextFile file;
    bool opened = false;
    
    // Try to open with detected encoding
    if (encoding == Encoding::UTF8 || encoding == Encoding::UTF8_BOM) {
        opened = file.Open(filename, wxConvUTF8);
    } else if (encoding == Encoding::UTF16_LE) {
        opened = file.Open(filename, wxMBConvUTF16LE());
    } else if (encoding == Encoding::UTF16_BE) {
        opened = file.Open(filename, wxMBConvUTF16BE());
    } else {
        // ANSI - use local encoding
        opened = file.Open(filename);
    }
    
    if (!opened) {
        return false;
    }
    
    // Read all content to detect separator
    wxString content;
    for (size_t i = 0; i < file.GetLineCount(); ++i) {
        content += file.GetLine(i) + wxT("\n");
    }
    
    // Detect separator
    separator = DetectSeparator(content);
    
    // Parse each line
    for (size_t i = 0; i < file.GetLineCount(); ++i) {
        wxString line = file.GetLine(i);
        if (!line.IsEmpty() || i == 0) { // Keep empty lines except skip trailing
            std::vector<wxString> fields = ParseLine(line, separator);
            data.push_back(fields);
        }
    }
    
    file.Close();
    return true;
}

bool CSVParser::WriteFile(const wxString& filename, const std::vector<std::vector<wxString>>& data,
                         wxChar separator, Encoding encoding) {
    // For ANSI encoding, use wxFile directly instead of wxTextFile
    if (encoding == Encoding::ANSI) {
        wxFile file(filename, wxFile::write);
        if (!file.IsOpened()) {
            return false;
        }
        
        wxCSConv conv(wxFONTENCODING_SYSTEM);
        
        for (const auto& row : data) {
            wxString line = FormatLine(row, separator) + "\r\n";
            // Convert to ANSI using system's default code page
            const wxCharBuffer buffer = line.mb_str(conv);
            if (buffer.length() > 0) {
                file.Write(buffer.data(), buffer.length());
            }
        }
        
        file.Close();
        return true;
    }
    
    // For other encodings, use wxTextFile
    wxTextFile file;
    
    // Create or open file
    if (wxFileExists(filename)) {
        if (!file.Open(filename)) {
            return false;
        }
        file.Clear();
    } else {
        if (!file.Create(filename)) {
            return false;
        }
    }
    
    // Write BOM if needed
    if (encoding == Encoding::UTF8_BOM) {
        wxFile bomFile(filename, wxFile::write);
        if (bomFile.IsOpened()) {
            unsigned char bom[3] = {0xEF, 0xBB, 0xBF};
            bomFile.Write(bom, 3);
            bomFile.Close();
        }
    }
    
    // Format and write each line
    for (const auto& row : data) {
        wxString line = FormatLine(row, separator);
        file.AddLine(line);
    }
    
    // Write file with appropriate encoding
    bool success = false;
    if (encoding == Encoding::UTF8 || encoding == Encoding::UTF8_BOM) {
        success = file.Write(wxTextFileType_Dos, wxConvUTF8);
    } else if (encoding == Encoding::UTF16_LE) {
        success = file.Write(wxTextFileType_Dos, wxMBConvUTF16LE());
    } else if (encoding == Encoding::UTF16_BE) {
        success = file.Write(wxTextFileType_Dos, wxMBConvUTF16BE());
    }
    
    file.Close();
    return success;
}

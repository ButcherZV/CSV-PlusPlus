# CSV++ Editor

A Windows desktop CSV editor with table view, making it easy to edit CSV files without manually dealing with delimiters and quotes.

## Features

- **Editable Table View** - Edit CSV data in an intuitive grid interface with column headers
- **Multiple Encoding Support** - UTF-8, ANSI, and UTF-16 with automatic detection
- **Flexible Separators** - Supports comma, semicolon, tab, and custom separators with auto-detection
- **Drag and Drop** - Simply drag CSV files into the window to open them
- **Undo/Redo** - 50 levels of undo/redo support
- **Excel Compatible** - Handles quoted fields with embedded separators and newlines
- **Header Editing** - Double-click column headers to rename them
- **Easy Row/Column Management** - Add and delete rows/columns via toolbar or right-click menu

## Building

### Prerequisites

1. MSYS2 with UCRT64 environment
2. wxWidgets 3.2 for MSW

Install wxWidgets:
```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-wxwidgets3.2-msw
```

### Compile

Simply run the build script:
```cmd
build.bat
```

To create a release package:
```cmd
build.bat -upakuj
```

The packaged application will be in `build\x64\` folder.

## Usage

### Opening Files

1. **File → Open** or **Ctrl+O** - Open CSV file with options dialog
2. **Drag and drop** - Drop a CSV file onto the window
3. The options dialog lets you:
   - Select encoding (auto-detected by default)
   - Choose separator (auto-detected by default)
   - Enable "First row is header" to use first row as column names

### Editing

- **Double-click** any cell to edit
- **Double-click** column headers to rename them
- **Right-click** for context menu to add/delete rows/columns
- **Toolbar buttons** for quick access to common operations

### Keyboard Shortcuts

- **Ctrl+N** - New file
- **Ctrl+O** - Open file
- **Ctrl+S** - Save file
- **Ctrl+Z** - Undo
- **Ctrl+Y** - Redo
- **Delete** - Delete selected rows/columns

### Settings Menu

- **Encoding** - Change file encoding (UTF-8, UTF-16, ANSI)
- **Separator** - Change field separator (Comma, Semicolon, Tab, Custom)

Changes to encoding and separator apply when saving the file.

### Status Bar

The status bar shows:
- Number of rows
- Number of columns  
- Current encoding
- Current separator

## CSV Format

The editor supports RFC 4180 compliant CSV files with:
- Quoted fields containing separators: `"Smith, John"`
- Quoted fields with embedded newlines
- Escaped quotes using double quotes: `"He said ""Hello"""`
- Multiple encodings (UTF-8, ANSI, UTF-16)
- Different separators (comma, semicolon, tab, custom)

## License

Copyright (C) 2026

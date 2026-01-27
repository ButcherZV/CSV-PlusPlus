#include <wx/wx.h>
#include "MainFrame.h"

class CSVApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(CSVApp);

bool CSVApp::OnInit() {
    MainFrame* frame = new MainFrame("CSV++");
    frame->Show(true);
    return true;
}

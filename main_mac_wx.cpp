#include <wx/wx.h>

#include "UIWX/MainFrame.h"
#include "Gameboy.h"

class MyApp : public wxApp
{
public:
    bool OnInit() override;

private:
    std::shared_ptr<Gameboy> gb;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    gb = std::make_shared<Gameboy>();
    MainFrame *frame = new MainFrame();
    frame->Show(true);
    return true;
}

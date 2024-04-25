#pragma once
#include <wx/wx.h>

class MainFrame : public wxFrame
{
public:
    MainFrame();

private:
    void OnFileOpen(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
};

enum
{
    ID_FileOpen = 1
};

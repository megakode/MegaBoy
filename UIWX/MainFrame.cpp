#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "MegaBoy")
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_FileOpen, "&Open...\tCtrl-O",
                     "Open ROM file");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Ready.");

    Bind(wxEVT_MENU, &MainFrame::OnFileOpen, this, ID_FileOpen);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
}

void MainFrame::OnExit(wxCommandEvent &event)
{
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox("MegaBoy GB Emulator by Peter Boné",
                 "About", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnFileOpen(wxCommandEvent &WXUNUSED(event))
{
    wxFileDialog openFileDialog(this, _("Open ROM file"), "", "", "rom files (*.gb)|*.gb", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // the user changed idea...

    // proceed loading the file chosen by the user;
    // this can be done with e.g. wxWidgets input streams:
    wxFileInputStream input_stream(openFileDialog.GetPath());
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
        return;
    }
}
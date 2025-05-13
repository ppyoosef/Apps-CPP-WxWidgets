#include <wx/wx.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/filedlg.h>
#include <wx/fontdlg.h>
#include <wx/toolbar.h>
#include <wx/artprov.h>
#include <fstream>

class WordPadFrame : public wxFrame {
public:
    WordPadFrame() : wxFrame(nullptr, wxID_ANY, "WordPad Clone", wxDefaultPosition, wxSize(800, 600)) {
        CreateMenuBar();
        CreateToolBar();
        CreateEditor();
        CreateStatusBar();
        Centre(); // Center the window on screen
    }

private:
    wxRichTextCtrl* textCtrl = nullptr;

    void CreateMenuBar() {
        wxMenu* fileMenu = new wxMenu;
        fileMenu->Append(wxID_NEW, "&New\tCtrl+N");
        fileMenu->Append(wxID_OPEN, "&Open...\tCtrl+O");
        fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S");
        fileMenu->AppendSeparator();
        fileMenu->Append(wxID_EXIT, "E&xit");

        wxMenu* editMenu = new wxMenu;
        editMenu->Append(wxID_UNDO, "&Undo\tCtrl+Z");
        editMenu->Append(wxID_REDO, "&Redo\tCtrl+Y");
        editMenu->AppendSeparator();
        editMenu->Append(wxID_CUT, "Cu&t\tCtrl+X");
        editMenu->Append(wxID_COPY, "&Copy\tCtrl+C");
        editMenu->Append(wxID_PASTE, "&Paste\tCtrl+V");

        wxMenu* formatMenu = new wxMenu;
        formatMenu->Append(wxID_ANY, "&Font...", "Choose font");

        wxMenuBar* menuBar = new wxMenuBar;
        menuBar->Append(fileMenu, "&File");
        menuBar->Append(editMenu, "&Edit");
        menuBar->Append(formatMenu, "F&ormat");

        SetMenuBar(menuBar);

        Bind(wxEVT_MENU, &WordPadFrame::OnNew, this, wxID_NEW);
        Bind(wxEVT_MENU, &WordPadFrame::OnOpen, this, wxID_OPEN);
        Bind(wxEVT_MENU, &WordPadFrame::OnSave, this, wxID_SAVE);
        Bind(wxEVT_MENU, &WordPadFrame::OnExit, this, wxID_EXIT);
        Bind(wxEVT_MENU, &WordPadFrame::OnFont, this, formatMenu->FindItem("&Font..."));
    }

    void CreateToolBar() {
        wxToolBar* toolbar = this->wxFrame::CreateToolBar();
        toolbar->AddTool(wxID_NEW, "New", wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR));
        toolbar->AddTool(wxID_OPEN, "Open", wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR));
        toolbar->AddTool(wxID_SAVE, "Save", wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR));
        toolbar->Realize();

        Bind(wxEVT_TOOL, &WordPadFrame::OnNew, this, wxID_NEW);
        Bind(wxEVT_TOOL, &WordPadFrame::OnOpen, this, wxID_OPEN);
        Bind(wxEVT_TOOL, &WordPadFrame::OnSave, this, wxID_SAVE);
    }

    void CreateEditor() {
        textCtrl = new wxRichTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
                                       wxTE_MULTILINE | wxTE_RICH2);
    }

    // ==== Menu Actions ====

    void OnNew(wxCommandEvent&) {
        textCtrl->Clear();
        SetStatusText("New document");
    }

    void OnOpen(wxCommandEvent&) {
        wxFileDialog openFileDialog(this, "Open Text file", "", "",
                                    "Text files (*.txt)|*.txt|All files (*.*)|*.*",
                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (openFileDialog.ShowModal() == wxID_CANCEL)
            return;

        textCtrl->LoadFile(openFileDialog.GetPath());
        SetStatusText("Opened: " + openFileDialog.GetFilename());
    }

    void OnSave(wxCommandEvent&) {
        wxFileDialog saveFileDialog(this, "Save Text file", "", "",
                                    "Text files (*.txt)|*.txt|All files (*.*)|*.*",
                                    wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;

        textCtrl->SaveFile(saveFileDialog.GetPath());
        SetStatusText("Saved: " + saveFileDialog.GetFilename());
    }

    void OnExit(wxCommandEvent&) {
        Close(true);
    }

    void OnFont(wxCommandEvent&) {
        wxFontData data;
        data.EnableEffects(true);
        data.SetInitialFont(textCtrl->GetFont());

        wxFontDialog dialog(this, data);
        if (dialog.ShowModal() == wxID_OK) {
            wxFont font = dialog.GetFontData().GetChosenFont();
            textCtrl->SetFont(font);
        }
    }
};

class WordPadApp : public wxApp {
public:
    bool OnInit() override {
        if (!wxApp::OnInit())
            return false;
        WordPadFrame* frame = new WordPadFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(WordPadApp);

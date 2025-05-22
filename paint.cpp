#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/filedlg.h>
#include <wx/colordlg.h>
#include <wx/image.h>
#include <wx/artprov.h>

// Tool definitions
enum ToolType { TOOL_PENCIL, TOOL_BRUSH, TOOL_ERASER };

// Forward declarations
class PaintPanel;

// -----------------------------
// MainFrame Declaration
// -----------------------------
class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);

private:
    void OnQuit(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnOpen(wxCommandEvent& event);
    void OnChooseColor(wxCommandEvent& event);
    void OnToolSelected(wxCommandEvent& event);

    PaintPanel* m_paintPanel;
    wxColour m_currentColor = *wxBLACK;
    ToolType m_currentTool = TOOL_PENCIL;

    wxDECLARE_EVENT_TABLE();
};

// -----------------------------
// PaintPanel Declaration
// -----------------------------
class PaintPanel : public wxPanel {
public:
    PaintPanel(wxWindow* parent, wxColour& colorRef, ToolType& toolRef);

    void SaveToFile(const wxString& path);
    void LoadFromFile(const wxString& path);

private:
    wxBitmap m_bitmap;
    wxPoint m_lastPos;
    wxColour& m_colorRef;
    ToolType& m_toolRef;

    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnMouseLeftUp(wxMouseEvent& event);
    void OnMotion(wxMouseEvent& event);
    void DrawLine(wxPoint from, wxPoint to);

    wxDECLARE_EVENT_TABLE();
};

// -----------------------------
// IDs
// -----------------------------
enum {
    ID_MENU_SAVE = wxID_HIGHEST + 1,
    ID_MENU_OPEN,
    ID_TOOL_PENCIL,
    ID_TOOL_BRUSH,
    ID_TOOL_ERASER,
    ID_TOOL_COLOR
};

// -----------------------------
// Event Tables
// -----------------------------
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
    EVT_MENU(ID_MENU_SAVE, MainFrame::OnSave)
    EVT_MENU(ID_MENU_OPEN, MainFrame::OnOpen)
    EVT_MENU(ID_TOOL_COLOR, MainFrame::OnChooseColor)
    EVT_TOOL(ID_TOOL_PENCIL, MainFrame::OnToolSelected)
    EVT_TOOL(ID_TOOL_BRUSH, MainFrame::OnToolSelected)
    EVT_TOOL(ID_TOOL_ERASER, MainFrame::OnToolSelected)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(PaintPanel, wxPanel)
    EVT_PAINT(PaintPanel::OnPaint)
    EVT_LEFT_DOWN(PaintPanel::OnLeftDown)
    EVT_LEFT_UP(PaintPanel::OnMouseLeftUp)
    EVT_MOTION(PaintPanel::OnMotion)
wxEND_EVENT_TABLE()

// -----------------------------
// wxWidgets App Implementation
// -----------------------------
class MyApp : public wxApp {
public:
    virtual bool OnInit() override {
        wxInitAllImageHandlers();
        MainFrame* frame = new MainFrame("C++ Paint App - wxWidgets");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);

// -----------------------------
// MainFrame Implementation
// -----------------------------
MainFrame::MainFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    // File menu
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(ID_MENU_OPEN, "&Open Image...\tCtrl+O");
    fileMenu->Append(ID_MENU_SAVE, "&Save Image...\tCtrl+S");
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, "E&xit");

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);

    // Toolbar
    wxToolBar* toolBar = CreateToolBar();
    toolBar->AddTool(ID_TOOL_PENCIL, "Pencil", wxArtProvider::GetBitmap(wxART_TIP, wxART_TOOLBAR), "Pencil");
    toolBar->AddTool(ID_TOOL_BRUSH, "Brush", wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_TOOLBAR), "Brush");
    toolBar->AddTool(ID_TOOL_ERASER, "Eraser", wxArtProvider::GetBitmap(wxART_DELETE, wxART_TOOLBAR), "Eraser");
    toolBar->AddTool(ID_TOOL_COLOR, "Color", wxArtProvider::GetBitmap(wxART_TICK_MARK, wxART_TOOLBAR), "Color Picker");
    toolBar->Realize();

    // Paint panel
    m_paintPanel = new PaintPanel(this, m_currentColor, m_currentTool);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_paintPanel, 1, wxEXPAND);
    SetSizer(sizer);
    Layout();
}

// -----------------------------
// Menu and Toolbar Events
// -----------------------------
void MainFrame::OnQuit(wxCommandEvent& event) {
    Close(true);
}

void MainFrame::OnSave(wxCommandEvent& event) {
    wxFileDialog dlg(this, "Save Image", "", "", "PNG files (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() == wxID_OK) {
        m_paintPanel->SaveToFile(dlg.GetPath());
    }
}

void MainFrame::OnOpen(wxCommandEvent& event) {
    wxFileDialog dlg(this, "Open Image", "", "", "PNG files (*.png)|*.png", wxFD_OPEN);
    if (dlg.ShowModal() == wxID_OK) {
        m_paintPanel->LoadFromFile(dlg.GetPath());
    }
}

void MainFrame::OnChooseColor(wxCommandEvent& event) {
    wxColourData data;
    data.SetColour(m_currentColor);
    wxColourDialog dlg(this, &data);
    if (dlg.ShowModal() == wxID_OK) {
        m_currentColor = dlg.GetColourData().GetColour();
    }
}

void MainFrame::OnToolSelected(wxCommandEvent& event) {
    switch (event.GetId()) {
        case ID_TOOL_PENCIL: m_currentTool = TOOL_PENCIL; break;
        case ID_TOOL_BRUSH:  m_currentTool = TOOL_BRUSH;  break;
        case ID_TOOL_ERASER: m_currentTool = TOOL_ERASER; break;
    }
}

// -----------------------------
// PaintPanel Implementation
// -----------------------------
PaintPanel::PaintPanel(wxWindow* parent, wxColour& colorRef, ToolType& toolRef)
    : wxPanel(parent), m_colorRef(colorRef), m_toolRef(toolRef)
{
    Bind(wxEVT_SIZE, &PaintPanel::OnSize, this);
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    m_bitmap.Create(800, 600);
    wxMemoryDC dc(m_bitmap);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
}

void PaintPanel::OnSize(wxSizeEvent& event) {
    wxSize newSize = event.GetSize();
    if (newSize.GetWidth() > 0 && newSize.GetHeight() > 0) {
        wxBitmap newBitmap(newSize);
        wxMemoryDC dc;
        dc.SelectObject(newBitmap);
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();
        dc.DrawBitmap(m_bitmap, 0, 0, false);
        m_bitmap = newBitmap;
    }

    Refresh();
    event.Skip();
}

void PaintPanel::OnPaint(wxPaintEvent& event) {
    wxBufferedPaintDC dc(this);
    dc.DrawBitmap(m_bitmap, 0, 0);
}

void PaintPanel::OnLeftDown(wxMouseEvent& event) {
    m_lastPos = event.GetPosition();
    if (!HasCapture()) {
        CaptureMouse();
    }
}

void PaintPanel::OnMouseLeftUp(wxMouseEvent& event) {
    if (HasCapture()) {
        ReleaseMouse();
    }
}

void PaintPanel::OnMotion(wxMouseEvent& event) {
    if (event.Dragging() && event.LeftIsDown()) {
        DrawLine(m_lastPos, event.GetPosition());
        m_lastPos = event.GetPosition();
    }
}

void PaintPanel::DrawLine(wxPoint from, wxPoint to) {
    wxMemoryDC dc(m_bitmap);
    wxPen pen;
    int m_eraserSize = 10; 

    if (m_toolRef == TOOL_ERASER) {
        dc.SetPen(wxPen(*wxWHITE, m_eraserSize));
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.DrawLine(from, to);
        Refresh();
        return;
    } else
        pen.SetColour(m_colorRef);

    pen.SetWidth((m_toolRef == TOOL_BRUSH) ? 6 : 1);
    dc.SetPen(pen);
    dc.DrawLine(from, to);

    Refresh();
}

void PaintPanel::SaveToFile(const wxString& path) {
    m_bitmap.SaveFile(path, wxBITMAP_TYPE_PNG);
}

void PaintPanel::LoadFromFile(const wxString& path) {
    wxImage img;
    if (img.LoadFile(path, wxBITMAP_TYPE_PNG)) {
        m_bitmap = wxBitmap(img);
        Refresh();
    } else {
        wxLogError("Failed to load image. Format might not be supported.");
    }
}

#include <wx/wx.h>
#include <wx/grid.h>
#include <string>
#include <sstream>
#include <stack>

class CalculatorFrame : public wxFrame {
public:
    CalculatorFrame() : wxFrame(nullptr, wxID_ANY, "Calculator", wxDefaultPosition, wxSize(300, 400)) {
        wxPanel* panel = new wxPanel(this, wxID_ANY);

        display = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(260, 50), wxTE_RIGHT | wxTE_READONLY);
        wxFont font(18, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        display->SetFont(font);

        wxGridSizer* grid = new wxGridSizer(4, 5, 5, 5);
        wxString buttons[] = {
            "7", "8", "9", "/", "C",
            "4", "5", "6", "*", "←",
            "1", "2", "3", "-", "",
            "0", ".", "=", "+", ""
        };

        for (int i = 0; i < 20; ++i) {
            if (!buttons[i].IsEmpty()) {
                wxButton* btn = new wxButton(panel, 1000 + i, buttons[i], wxDefaultPosition, wxSize(50, 50));
                btn->SetFont(font);
                btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnButtonClicked, this);
                grid->Add(btn, 0, wxEXPAND);
            } else {
                grid->AddSpacer(1);
            }
        }

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(display, 0, wxALL | wxEXPAND, 10);
        sizer->Add(grid, 1, wxALL | wxEXPAND, 10);
        panel->SetSizer(sizer);
    }

private:
    wxTextCtrl* display;
    wxString current;

    void OnButtonClicked(wxCommandEvent& event) {
        int id = event.GetId() - 1000;
        wxString label = wxDynamicCast(event.GetEventObject(), wxButton)->GetLabel();

        if (label == "C") {
            current = "";
        } else if (label == "←") {
            if (!current.IsEmpty()) current.RemoveLast();
        } else if (label == "=") {
            try {
                std::string expr(current.mb_str());
                double result = EvaluateExpression(expr);
                current = wxString::Format("%.8g", result);
            } catch (...) {
                current = "Error";
            }
        } else {
            current += label;
        }

        display->SetValue(current);
    }

    // A simple expression evaluator (no parentheses support)
    double EvaluateExpression(const std::string& expr) {
        std::istringstream in(expr);
        std::stack<double> numbers;
        std::stack<char> ops;

        auto apply = [&](char op) {
            double b = numbers.top(); numbers.pop();
            double a = numbers.top(); numbers.pop();
            switch (op) {
                case '+': numbers.push(a + b); break;
                case '-': numbers.push(a - b); break;
                case '*': numbers.push(a * b); break;
                case '/': numbers.push(a / b); break;
            }
        };

        double num;
        char op;
        in >> num;
        numbers.push(num);

        while (in >> op) {
            if (!(in >> num)) break;

            while (!ops.empty() && (
                (op == '+' || op == '-') ||
                (op == '*' || op == '/') && (ops.top() == '*' || ops.top() == '/')))
            {
                apply(ops.top());
                ops.pop();
            }

            numbers.push(num);
            ops.push(op);
        }

        while (!ops.empty()) {
            apply(ops.top());
            ops.pop();
        }

        return numbers.top();
    }
};

class CalculatorApp : public wxApp {
public:
    virtual bool OnInit() {
        CalculatorFrame* frame = new CalculatorFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(CalculatorApp);

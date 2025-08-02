#include <wx/wx.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/font.h>
#include <sqlite3.h>

class ExpenseApp : public wxApp {
public:
    virtual bool OnInit();
};

class LoginFrame : public wxFrame {
public:
    LoginFrame(const wxString& title);

private:
    wxTextCtrl* nameField;
    wxTextCtrl* emailField;
    wxTextCtrl* passwordField;

    void OnLogin(wxCommandEvent& event);
    bool SaveUserToDatabase(const wxString& name, const wxString& email, const wxString& password);
};

wxIMPLEMENT_APP(ExpenseApp);

bool ExpenseApp::OnInit() {
    LoginFrame* frame = new LoginFrame("Expense Tracker - Login");
    frame->Center();
    frame->Maximize(true);
    frame->Show(true);
    return true;
}

LoginFrame::LoginFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title) {

    SetBackgroundColour(wxColour(25, 25, 25)); // Dark theme

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetBackgroundColour(wxColour(25, 25, 25));

    wxBoxSizer* outerSizer = new wxBoxSizer(wxVERTICAL);

    outerSizer->AddStretchSpacer(1);

    wxStaticText* emoji = new wxStaticText(panel, wxID_ANY, "💰");
    emoji->SetFont(wxFont(80, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    emoji->SetForegroundColour(*wxWHITE);
    outerSizer->Add(emoji, 0, wxALIGN_CENTER | wxBOTTOM, 10);

    wxStaticText* titleLabel = new wxStaticText(panel, wxID_ANY, "Expense Tracker");
    titleLabel->SetFont(wxFont(36, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    titleLabel->SetForegroundColour(wxColour(0, 191, 165));
    outerSizer->Add(titleLabel, 0, wxALIGN_CENTER | wxBOTTOM, 40);

    wxBoxSizer* formSizer = new wxBoxSizer(wxVERTICAL);

    auto createFormRow = [&](const wxString& labelText, wxTextCtrl*& field, long style = 0) {
        wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText* label = new wxStaticText(panel, wxID_ANY, labelText);
        label->SetForegroundColour(wxColour(220, 220, 220));
        label->SetMinSize(wxSize(100, -1));
        label->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        field = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(300, 28), style);
        rowSizer->Add(label, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10);
        rowSizer->Add(field, 0);
        formSizer->Add(rowSizer, 0, wxALIGN_CENTER | wxBOTTOM, 15);
    };

    createFormRow("Full Name:", nameField);
    createFormRow("Email:", emailField);
    createFormRow("Password:", passwordField, wxTE_PASSWORD);

    wxButton* loginBtn = new wxButton(panel, wxID_ANY, "Login", wxDefaultPosition, wxSize(150, 35));
    loginBtn->SetBackgroundColour(wxColour(0, 191, 165));
    loginBtn->SetForegroundColour(*wxWHITE);
    loginBtn->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    formSizer->Add(loginBtn, 0, wxALIGN_CENTER | wxTOP, 25);

    loginBtn->Bind(wxEVT_BUTTON, &LoginFrame::OnLogin, this);

    outerSizer->Add(formSizer, 0, wxALIGN_CENTER);
    outerSizer->AddStretchSpacer(2);

    panel->SetSizer(outerSizer);
    this->SetSizerAndFit(new wxBoxSizer(wxVERTICAL));
    this->GetSizer()->Add(panel, 1, wxEXPAND);
}

void LoginFrame::OnLogin(wxCommandEvent& event) {
    wxString name = nameField->GetValue();
    wxString email = emailField->GetValue();
    wxString password = passwordField->GetValue();

    if (name.IsEmpty() || email.IsEmpty() || password.IsEmpty()) {
        wxMessageBox("Please fill in all fields: name, email, and password.", "Missing Information", wxOK | wxICON_ERROR);
        return;
    }

    if (SaveUserToDatabase(name, email, password)) {
        wxMessageBox("Welcome, " + name + "!\nEmail: " + email, "Login Successful", wxOK | wxICON_INFORMATION);
        // TODO: Launch main dashboard window
    }
}

bool LoginFrame::SaveUserToDatabase(const wxString& name, const wxString& email, const wxString& password) {
    sqlite3* db;
    if (sqlite3_open("expense_data.db", &db) != SQLITE_OK) {
        wxMessageBox("Failed to open database.", "DB Error", wxOK | wxICON_ERROR);
        return false;
    }

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            email TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL
        );
    )";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        wxMessageBox("SQL Error: " + wxString(errMsg), "DB Error", wxOK | wxICON_ERROR);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return false;
    }

    const char* insertSQL = "INSERT INTO users (name, email, password) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK) {
        wxMessageBox("Failed to prepare insert statement.", "DB Error", wxOK | wxICON_ERROR);
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.mb_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, email.mb_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, password.mb_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        wxMessageBox("Failed to save user. Email might already be registered.", "Insert Error", wxOK | wxICON_ERROR);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return true;
}

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <wx/wx.h>
#include <wx/listctrl.h>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);

private:
	void CreateControls();
	void BindEvents();
	void OnAddButtonClicked(wxCommandEvent& evt);
	void OnClearButtonClicked(wxCommandEvent& evt);
	void OnWindowClosed(wxCloseEvent& evt);
	void AddSavedExpense();
	void OnInputEnter(wxCommandEvent& evt);
	void AddExpenseFromInput();
	void OnKeyDown(wxKeyEvent& evt);
	void DeleteExpense();
	


	wxPanel* panel;
	wxStaticText* headLineText;
	wxStaticText* descText;
	wxStaticText* catText;
	wxStaticText* amountText;
	wxStaticText* dateText;

	wxTextCtrl* descInput;
	wxTextCtrl* catInput;
	wxTextCtrl* amountInput;
	wxTextCtrl* dateInput;
	wxButton* addButton;
	wxButton* clearButton;
	wxListCtrl* listCtrl;
};


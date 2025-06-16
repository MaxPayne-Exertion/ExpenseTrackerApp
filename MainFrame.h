#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>

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
	bool dateSortAscending = true;
	void OnListColClick(wxListEvent& event);
	void OnSettingsButtonClicked(wxCommandEvent& evt);

	
	
	

	
	wxPanel* panel;
	wxStaticText* headLineText;
	wxStaticText* descText;
	wxStaticText* catText;
	wxStaticText* amountText;
	wxStaticText* dateText;

	wxTextCtrl* descInput;
	wxTextCtrl* amountInput;
	wxComboBox* catInput;
	std::vector<wxString> categoryList; // To keep track of unique categories
	wxDatePickerCtrl* dateInput;
	wxButton* addButton;
	wxButton* clearButton;
	wxButton* settingsButton;
	wxListCtrl* listCtrl;
};


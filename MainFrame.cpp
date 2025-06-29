#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Expense.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <map>



// Helper to extract all expenses from the list control
std::vector<Expense> GetExpensesFromListCtrl(wxListCtrl* listCtrl) {
	std::vector<Expense> expenses;
	for (int i = 0; i < listCtrl->GetItemCount(); ++i) {
		Expense exp;
		exp.description = listCtrl->GetItemText(i, 0);
		exp.category = listCtrl->GetItemText(i, 1);
		exp.amount = listCtrl->GetItemText(i, 2);
		exp.date = listCtrl->GetItemText(i, 3);
		expenses.push_back(exp);
	}
	return expenses;
}

// Helper to reload expenses into the list control
void LoadExpensesToListCtrl(wxListCtrl* listCtrl, const std::vector<Expense>& expenses) {
	listCtrl->DeleteAllItems();
	for (const auto& exp : expenses) {
		long idx = listCtrl->InsertItem(listCtrl->GetItemCount(), exp.description);
		listCtrl->SetItem(idx, 1, exp.category);
		listCtrl->SetItem(idx, 2, exp.amount);
		listCtrl->SetItem(idx, 3, exp.date);
	}
}




MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
	CreateControls();
	BindEvents();
	AddSavedExpense();
}

void MainFrame::CreateControls()
{
	wxFont headLineFont(wxFontInfo(wxSize(0, 30)).Bold());
	wxFont bodyFont(wxFontInfo(wxSize(0, 16)));
	panel = new wxPanel(this);
	panel->SetFont(bodyFont);

	headLineText = new wxStaticText(panel, wxID_ANY, "Expense Tracker", wxPoint(0, 10), wxSize(800, -1),wxALIGN_CENTER_HORIZONTAL);
	headLineText->SetBackgroundColour(wxColour(173, 216, 230)); 
	headLineText->SetFont(headLineFont);

	descText = new wxStaticText(panel, wxID_ANY, "Description", wxPoint(20, 60), wxSize(100,-1));
	
	descInput = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(130, 60), wxSize(200, -1), wxTE_PROCESS_ENTER);

	catText = new wxStaticText(panel, wxID_ANY, "Category", wxPoint(340, 60), wxSize(100, -1));
	
	catInput = new wxComboBox(panel, wxID_ANY, "", wxPoint(450, 60), wxSize(200, -1), 0, nullptr, wxCB_DROPDOWN);

	amountText = new wxStaticText(panel, wxID_ANY, "Amount", wxPoint(20, 100), wxSize(100, -1));
	
	amountInput = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(130, 100), wxSize(200, -1), wxTE_PROCESS_ENTER);

	dateText = new wxStaticText(panel, wxID_ANY, "Date", wxPoint(340, 100), wxSize(100, -1));
	
	dateInput = new wxDatePickerCtrl(panel, wxID_ANY, wxDefaultDateTime, wxPoint(450, 100), wxSize(200, -1));

	addButton = new wxButton(panel, wxID_ANY, "Add", wxPoint(680, 75), wxSize(100, 35));

	listCtrl = new wxListCtrl(panel, wxID_ANY, wxPoint(20, 170), wxSize(760, 380),wxLC_REPORT|wxBORDER_SUNKEN);

	listCtrl->InsertColumn(0, "Description", wxLIST_FORMAT_CENTER, 250);
	listCtrl->InsertColumn(1, "Category", wxLIST_FORMAT_CENTER, 170);
	listCtrl->InsertColumn(2, "Amount", wxLIST_FORMAT_CENTER, 170);
	listCtrl->InsertColumn(3, "Date", wxLIST_FORMAT_CENTER, 170);

	clearButton = new wxButton(panel, wxID_ANY, "Clear", wxPoint(680, 550), wxSize(100, 35));

	
	
	settingsButton = new wxButton(panel, wxID_ANY, "X", wxPoint(60, 550), wxSize(40, 40));
	settingsButton->SetFont(wxFontInfo(18).Bold());
	settingsButton->SetBackgroundColour(wxColour(173, 216, 230)); 
	settingsButton->SetForegroundColour(*wxBLACK);
	settingsButton->SetToolTip("Settings");

	plotButton = new wxButton(panel, wxID_ANY, "Plot", wxPoint(560, 550), wxSize(100, 35));



}

void MainFrame::BindEvents()
{
	//Binding event handlers to controls
	addButton->Bind(wxEVT_BUTTON, &MainFrame::OnAddButtonClicked, this);
	clearButton->Bind(wxEVT_BUTTON, &MainFrame::OnClearButtonClicked, this);
	descInput->Bind(wxEVT_TEXT_ENTER, &MainFrame::OnInputEnter, this);
	catInput->Bind(wxEVT_TEXT_ENTER, &MainFrame::OnInputEnter, this);
	amountInput->Bind(wxEVT_TEXT_ENTER, &MainFrame::OnInputEnter, this);
	dateInput->Bind(wxEVT_TEXT_ENTER, &MainFrame::OnInputEnter, this);
	listCtrl->Bind(wxEVT_KEY_DOWN, & MainFrame::OnKeyDown, this);
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnWindowClosed, this);
	listCtrl->Bind(wxEVT_LIST_COL_CLICK, &MainFrame::OnListColClick, this);
	settingsButton->Bind(wxEVT_BUTTON, &MainFrame::OnSettingsButtonClicked, this);
	plotButton->Bind(wxEVT_BUTTON, &MainFrame::OnPlotButtonClicked, this);


	

	



}

void MainFrame::AddExpenseFromInput()
{
	wxString desc = descInput->GetValue();
	wxString cat = catInput->GetValue();
	// Add new category to the combo box if not already present
	if (!cat.IsEmpty() && catInput->FindString(cat) == wxNOT_FOUND) {
		catInput->Append(cat);
		categoryList.push_back(cat);
	}
	wxString amount = amountInput->GetValue();
	wxString date = dateInput->GetValue().FormatISODate();

	//Error handling for when all of the fields are not filled
	if (desc.IsEmpty() || cat.IsEmpty() || amount.IsEmpty() || date.IsEmpty()) {
		wxMessageBox("Please fill in all the fields!");
		return;
	}

	//Adding each field values to the list
	long index = listCtrl->InsertItem(listCtrl->GetItemCount(), desc);
	listCtrl->SetItem(index, 1, cat);
	listCtrl->SetItem(index, 2, amount);
	listCtrl->SetItem(index, 3, date);


	//Clearing the input field after the values of the input fields have been listed
	descInput->Clear();
	catInput->SetValue(""); // Only clear the text, not the items!
	amountInput->Clear();
	//dateInput->Clear();
}

//Deleting selected expense detail when delete key is pressed
void MainFrame::OnKeyDown(wxKeyEvent& evt)
{
	switch (evt.GetKeyCode()) {
	case WXK_DELETE:
		DeleteExpense();
	}
		
}

void MainFrame::DeleteExpense()
{
	long index = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	if (index == -1) {
		wxMessageBox("No task is selected!");
		return;
	}

	listCtrl->DeleteItem(index);
}

//Event handling when add button is pressed
void MainFrame::OnAddButtonClicked(wxCommandEvent& evt)
{
	AddExpenseFromInput();
}

//Event handling when enter key is pressed after filling the fields
void MainFrame::OnInputEnter(wxCommandEvent& evt)
{
	AddExpenseFromInput();
}

void MainFrame::OnClearButtonClicked(wxCommandEvent& evt)
{
	//Error handling for when the Clear button is pressed when there are no items in the list
	if (listCtrl->GetItemCount() == 0) {
		wxMessageBox("There are no expenses!");
		return;
	}

	//Dialog box to confirm that the user wants to clear the list
	wxMessageDialog dialog(this, "Are you sure you want to clear all expenses?", "Clear", wxYES_NO | wxCANCEL);

	//Storing enum ID of the dialog input
	int result = dialog.ShowModal();

	//If the enum ID is matching with the enum ID of the yes button, then clear the input
	if (result == wxID_YES) {
		listCtrl->DeleteAllItems();
	}
}

//Event Handling when the window is closed i.e., saving the list items in a text file
void MainFrame::OnWindowClosed(wxCloseEvent& evt)
{
	//Declaring a vector of Expense structures
	std::vector<Expense> expenses;

	//Iterating through the list
	for (int i = 0;i < listCtrl->GetItemCount();i++) {
		Expense expense;           //Declaring a Expense structure
		expense.description = listCtrl->GetItemText(i, 0); //Adding values from the list's column to the corresponding structure members
		expense.category = listCtrl->GetItemText(i, 1);
		expense.amount = listCtrl->GetItemText(i, 2);
		expense.date = listCtrl->GetItemText(i, 3);
		expenses.push_back(expense); //Adding the structure to the vector of structures
	}
	AddExpenseToFile(expenses, "expense.txt");  //Storing the vector in the text file
	evt.Skip();  //skipping event to prevent the window from not closing
}

//Adding saved expenses to the list from the text file to the list after the app has been re-opened
void MainFrame::AddSavedExpense()
{
		catInput->Clear();
		categoryList.clear();
		std::vector<Expense> expenses = LoadExpenseFromFile("expense.txt");
		
		for (const Expense& expense : expenses) {
			int index = listCtrl->InsertItem(listCtrl->GetItemCount(), expense.description);
			
			listCtrl->SetItem(index, 1, expense.category);
			listCtrl->SetItem(index, 2, expense.amount);
			listCtrl->SetItem(index, 3, expense.date);

			// Add category to the combo box if it doesn't already exist
			if (!expense.category.empty() && catInput->FindString(expense.category) == wxNOT_FOUND) {
				catInput->Append(expense.category);
				categoryList.push_back(expense.category);
			}
	}
}

void MainFrame::OnListColClick(wxListEvent& event) {
	int col = event.GetColumn();
	auto expenses = GetExpensesFromListCtrl(listCtrl);

	if (col == 1) { // Category column
		if (categorySortAscending) {
			std::sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
				return a.category < b.category;
				});
		}
		else {
			std::sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
				return a.category > b.category;
				});
		}
		categorySortAscending = !categorySortAscending;
		LoadExpensesToListCtrl(listCtrl, expenses);
	}
	else if (col == 2) { // Amount column
		if (amountSortAscending) {
			std::sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
				try {
					return std::stod(a.amount) < std::stod(b.amount);
				}
				catch (...) {
					return a.amount < b.amount; // fallback to string compare
				}
				});
		}
		else {
			std::sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
				try {
					return std::stod(a.amount) > std::stod(b.amount);
				}
				catch (...) {
					return a.amount > b.amount; // fallback to string compare
				}
				});
		}
		amountSortAscending = !amountSortAscending;
		LoadExpensesToListCtrl(listCtrl, expenses);
	}
	else if (col == 3) { // Date column
		if (dateSortAscending) {
			std::sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
				return a.date < b.date;
				});
		}
		else {
			std::sort(expenses.begin(), expenses.end(), [](const Expense& a, const Expense& b) {
				return a.date > b.date;
				});
		}
		dateSortAscending = !dateSortAscending;
		LoadExpensesToListCtrl(listCtrl, expenses);
	}
}


void MainFrame::OnSettingsButtonClicked(wxCommandEvent& evt) {
	
	isDarkMode = !isDarkMode;
	if (isDarkMode)
		EnableDarkMode();
	else
		EnableLightMode(); 
	panel->Refresh();
	

}


void MainFrame::OnPlotButtonClicked(wxCommandEvent& evt)
{
	//plotting
}




//dark mode
void MainFrame::EnableDarkMode() {
	wxColour bgColor(30, 30, 30);
	wxColour fgColor(220, 220, 220);
	wxColour accentColor(45, 45, 48);

	panel->SetBackgroundColour(bgColor);

	headLineText->SetBackgroundColour(accentColor);
	headLineText->SetForegroundColour(fgColor);

	descText->SetForegroundColour(fgColor);
	catText->SetForegroundColour(fgColor);
	amountText->SetForegroundColour(fgColor);
	dateText->SetForegroundColour(fgColor);

	descInput->SetBackgroundColour(accentColor);
	descInput->SetForegroundColour(fgColor);
	catInput->SetBackgroundColour(accentColor);
	catInput->SetForegroundColour(fgColor);
	amountInput->SetBackgroundColour(accentColor);
	amountInput->SetForegroundColour(fgColor);
	dateInput->SetBackgroundColour(accentColor);
	dateInput->SetForegroundColour(fgColor);

	addButton->SetBackgroundColour(accentColor);
	addButton->SetForegroundColour(fgColor);
	clearButton->SetBackgroundColour(accentColor);
	clearButton->SetForegroundColour(fgColor);
	plotButton->SetBackgroundColour(accentColor);
	plotButton->SetForegroundColour(fgColor);
	settingsButton->SetBackgroundColour(accentColor);
	settingsButton->SetForegroundColour(fgColor);

	listCtrl->SetBackgroundColour(bgColor);
	listCtrl->SetForegroundColour(fgColor);

	panel->Refresh();
}
void MainFrame::EnableLightMode() {
	wxColour panelBg = *wxWHITE;
	wxColour textFg = *wxBLACK;
	wxColour accentBg(173, 216, 230);

	panel->SetBackgroundColour(panelBg);

	headLineText->SetBackgroundColour(accentBg);
	headLineText->SetForegroundColour(textFg);

	descText->SetForegroundColour(textFg);
	catText->SetForegroundColour(textFg);
	amountText->SetForegroundColour(textFg);
	dateText->SetForegroundColour(textFg);

	descInput->SetBackgroundColour(*wxWHITE);
	descInput->SetForegroundColour(textFg);
	catInput->SetBackgroundColour(*wxWHITE);
	catInput->SetForegroundColour(textFg);
	amountInput->SetBackgroundColour(*wxWHITE);
	amountInput->SetForegroundColour(textFg);
	dateInput->SetBackgroundColour(*wxWHITE);
	dateInput->SetForegroundColour(textFg);

	addButton->SetBackgroundColour(*wxLIGHT_GREY);
	addButton->SetForegroundColour(textFg);
	clearButton->SetBackgroundColour(*wxLIGHT_GREY);
	clearButton->SetForegroundColour(textFg);
	plotButton->SetBackgroundColour(*wxLIGHT_GREY);
	plotButton->SetForegroundColour(textFg);
	settingsButton->SetBackgroundColour(accentBg);
	settingsButton->SetForegroundColour(textFg);

	listCtrl->SetBackgroundColour(*wxWHITE);
	listCtrl->SetForegroundColour(textFg);

	panel->Refresh();
}

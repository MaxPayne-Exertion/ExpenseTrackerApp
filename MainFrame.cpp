#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "Expense.h"


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
	headLineText->SetBackgroundColour(*wxRED);
	headLineText->SetFont(headLineFont);

	descText = new wxStaticText(panel, wxID_ANY, "Description", wxPoint(20, 60), wxSize(100,-1));
	
	descInput = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(130, 60), wxSize(200, -1), wxTE_PROCESS_ENTER);

	catText = new wxStaticText(panel, wxID_ANY, "Category", wxPoint(340, 60), wxSize(100, -1));
	
	catInput = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(450, 60), wxSize(200, -1), wxTE_PROCESS_ENTER);

	amountText = new wxStaticText(panel, wxID_ANY, "Amount", wxPoint(20, 100), wxSize(100, -1));
	
	amountInput = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(130, 100), wxSize(200, -1), wxTE_PROCESS_ENTER);

	dateText = new wxStaticText(panel, wxID_ANY, "Date", wxPoint(340, 100), wxSize(100, -1));
	
	dateInput = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(450, 100), wxSize(200, -1), wxTE_PROCESS_ENTER);

	addButton = new wxButton(panel, wxID_ANY, "Add", wxPoint(680, 75), wxSize(100, 35));

	listCtrl = new wxListCtrl(panel, wxID_ANY, wxPoint(20, 150), wxSize(760, 380),wxLC_REPORT|wxBORDER_SUNKEN);

	listCtrl->InsertColumn(0, "Description", wxLIST_FORMAT_CENTER, 250);
	listCtrl->InsertColumn(1, "Category", wxLIST_FORMAT_CENTER, 170);
	listCtrl->InsertColumn(2, "Amount", wxLIST_FORMAT_CENTER, 170);
	listCtrl->InsertColumn(3, "Date", wxLIST_FORMAT_CENTER, 170);

	clearButton = new wxButton(panel, wxID_ANY, "Clear", wxPoint(680, 540), wxSize(100, 35));

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
}

void MainFrame::AddExpenseFromInput()
{
	wxString desc = descInput->GetValue();
	wxString cat = catInput->GetValue();
	wxString amount = amountInput->GetValue();
	wxString date = dateInput->GetValue();

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
	catInput->Clear();
	amountInput->Clear();
	dateInput->Clear();
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
	
		std::vector<Expense> expenses = LoadExpenseFromFile("expense.txt");
		for (const Expense& expense : expenses) {
			int index = listCtrl->InsertItem(listCtrl->GetItemCount(), expense.description);
			
			listCtrl->SetItem(index, 1, expense.category);
			listCtrl->SetItem(index, 2, expense.amount);
			listCtrl->SetItem(index, 3, expense.date);
	}
}





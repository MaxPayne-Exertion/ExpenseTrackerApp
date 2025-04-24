#include "myApp.h"
#include "MainFrame.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(myApp);

bool myApp::OnInit() {
	MainFrame* frame = new MainFrame("Expense Tracker");
	frame->Show(true);
	frame->SetClientSize(800, 600);
	frame->Center();
	return true;
}
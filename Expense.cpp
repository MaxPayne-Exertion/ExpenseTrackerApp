#include "Expense.h"
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>

void AddExpenseToFile(const std::vector<Expense>& expenses, const std::string& fileName)
{
	std::ofstream ostream(fileName);
	ostream << expenses.size();

	for (const Expense& expense : expenses) {
		std::string description = expense.description;
		std::string category = expense.category;
		std::string amount = expense.amount;
		std::string date = expense.date;
		std::replace(description.begin(), description.end(), ' ', '_');
		std::replace(category.begin(), category.end(), ' ', '_');

		ostream << '\n' << description << ' ' << category << ' ' << amount << ' ' << date;

	}
}

std::vector<Expense> LoadExpenseFromFile(const std::string& fileName)
{
	if (!std::filesystem::exists(fileName)) {
		return std::vector<Expense>();
	}

	std::vector<Expense> expenses;
	std::ifstream istream(fileName);

	int n;
	istream >> n;

	for (int i = 0;i < n;i++) {
		std::string description, category, amount, date;

		istream >> description >> category >> amount >> date;
		std::replace(description.begin(), description.end(), '_', ' ');
		std::replace(category.begin(), category.end(), '_', ' ');
		expenses.push_back(Expense{ description, category,amount,date });

	}
	return expenses;
}

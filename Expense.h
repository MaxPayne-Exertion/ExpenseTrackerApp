#pragma once
#include <vector>

#include <string>

struct Expense
{
	std::string description;
	std::string category;
	std::string amount;
	std::string date;
};

void AddExpenseToFile(const std::vector<Expense>& expenses, const std::string& fileName);
std::vector<Expense> LoadExpenseFromFile(const std::string& fileName);

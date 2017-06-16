#pragma once

#include "StringNumberChecker.h"

class RPNParser
{
	std::vector<DTA> operands;
	StringNumberChecker monoid_checker;
public:

	DTA& parse(std::string& input) {
		operands.reserve(4);
		monoid_checker.reset();

		for (int i = 0; i < input.size(); i++) {
			if (input[i] == '*') {
				//std::cout << "Start of '*' operation\n";
				//check for correctness of stack
				operands[operands.size() - 1].Star();
				//std::cout << "";
			}
			else if (input[i] == '|') {
				//std::cout << "Start of '|' operation\n";
				//check for correctness of stack
				operands[operands.size() - 2].Union(operands[operands.size() - 1]);
				operands.pop_back();
				//std::cout << "End of '|' operation\n";
			}
			else if (input[i] == '.') {
				//std::cout << "Start of '.' operation\n";
				//check for correctness of stack
				operands[operands.size() - 2].Concatenate(operands[operands.size() - 1]);
				operands.pop_back();
				//std::cout << "End of '.' operation\n";
			}
			else {
				monoid_checker.register_symbol(input[i]);
				if (monoid_checker.success()) {
					//std::cout << "Start of monoid creation\n";
					operands.push_back(monoid_checker.factory());
					monoid_checker.reset();
					//std::cout << "End of monoid creation\n";
				}
			}
		}
		//check correctness of stack
		if (operands.size() > 1) {
			return DTA();
		}
		return operands.back();
	}
};


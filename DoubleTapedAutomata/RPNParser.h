#pragma once
#include <stack>
#include "Utility.h";
#include "StringNumberChecker.h"

//Reverse Polish Notation Parser
class RPNParser
{
	std::stack<Automata> operands;
	StringNumberChecker monoid_checker;
public:

	Automata& parse(std::string& input) {
		monoid_checker.reset();

		for (int i = 0; i < input.size(); i++) {
			if (input[i] == '*') {
				//check for correctness of stack
				auto left = operands.top();
				operands.pop();
				auto result = left.Star();
				operands.push(result);
			}
			else if (input[i] == '|') {
				//check for correctness of stack
				auto right = operands.top();
				operands.pop();
				auto left = operands.top();
				operands.pop();
				auto result = left.cUnion(right);
				operands.push(result);
			}
			else if (input[i] == '.') {
				//check for correctness of stack
				auto right = operands.top();
				operands.pop();
				auto left = operands.top();
				operands.pop();
				auto result = left.cConcatenate(right);
				operands.push(result);
			}
			else {
				monoid_checker.register_symbol(input[i]);
				if (monoid_checker.success()) {
					auto test = monoid_checker.factory();
					operands.push(test);
					monoid_checker.reset();
				}
			}
		}
		//check correctness of stack
		if (operands.size() > 1) {
			return Automata();
		}
		return operands.top();
	}
};

class RPNParser_vector_operands
{
	std::vector<Automata> operands;
	StringNumberChecker monoid_checker;
public:

	Automata& parse(std::string& input) {
		monoid_checker.reset();

		for (int i = 0; i < input.size(); i++) {
			if (input[i] == '*') {
				std::cout << "Start of '*' operation\n";
				//check for correctness of stack
				operands[operands.size() - 1].Star();
				std::cout << "";
			}
			else if (input[i] == '|') {
				std::cout << "Start of '|' operation\n";
				//check for correctness of stack
				operands[operands.size() - 2].Union(operands[operands.size() - 1]);
				operands.pop_back();
				std::cout << "End of '|' operation\n";
			}
			else if (input[i] == '.') {
				std::cout << "Start of '.' operation\n";
				//check for correctness of stack
				operands[operands.size() - 2].Concatenate(operands[operands.size() - 1]);
				operands.pop_back();
				std::cout << "End of '.' operation\n";
			}
			else {
				monoid_checker.register_symbol(input[i]);
				if (monoid_checker.success()) {
					std::cout << "Start of monoid creation\n";
					operands.push_back(monoid_checker.factory());
					monoid_checker.reset();
					std::cout << "End of monoid creation\n";
				}
			}
		}
		//check correctness of stack
		if (operands.size() > 1) {
			return Automata();
		}
		return operands.back();
	}
};


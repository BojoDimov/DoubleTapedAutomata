#pragma once
#include <stack>
#include "Utility.h";
#include "StringNumberChecker.h"

//Reverse Polish Notation Parser
class RPNParser
{
	std::stack<char> operators;
	std::stack<Automata*> operands;
	StringNumberChecker monoid_checker;
public:

	Automata& parse(std::string& input) {
		monoid_checker.reset();

		for (int i = 0; i < input.size(); i++) {
			if (input[i] == '*') {
				//check for correctness of stack
				auto left = operands.top();
				operands.pop();
				operands.push(&left->Star());
			}
			else if (input[i] == '|') {
				//check for correctness of stack
				auto right = operands.top();
				operands.pop();
				auto left = operands.top();
				operands.pop();
				operands.push(&left->Union(*right));
			}
			else if (input[i] == '.') {
				//check for correctness of stack
				auto right = operands.top();
				operands.pop();
				auto left = operands.top();
				operands.pop();
				operands.push(&left->Concatenate(*right));
			}
			else {
				monoid_checker.register_symbol(input[i]);
				if (monoid_checker.success()) {
					operands.push(&monoid_checker.factory());
					monoid_checker.reset();
				}
			}
		}
		//check correctness of stack
		if (operands.size() > 1) {
			return Automata();
		}
		return *operands.top();
	}
};


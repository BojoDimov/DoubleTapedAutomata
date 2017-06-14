#pragma once
#include <iostream>
#include <stack>
#include "Utility.h"
#include "StringNumberChecker.h"



class Parser
{
private:
	StringNumberChecker monoid_checker;
	std::stack<Automata*> operand_stack;
	std::stack<char> operator_stack;

	bool _is_operator(char symbol)const {
		return symbol == '|' || symbol == '*' || symbol == '.';
	}

	Automata& _parse(string_it& start, string_it& end, std::string& input) {
		for (start; start != end; start++) {
			monoid_checker.register_symbol(*start);

			if (monoid_checker.success()) {
				//this needs to be first because we are checking for ')' in it
				//there shouldnt be any continue because we need to enter the ')' if to return the automata
				operand_stack.push(&monoid_checker.factory());
				monoid_checker.reset();
			}

			if (_is_operator(*start)) {
				operator_stack.push(*start);
			}

			if (*start == ')') {
				start++;//important because we will unwind the stack of function calls and we will end up calling factory() again

				/*auto automata = factory();
				operand_stack.push(&automata);
				return automata;*//// wrong because we are pushing in the operand_stack the result of _parse() call
				return factory();
			}

			if (*start == '(') {
				operand_stack.push(&_parse(++start, end, input));
			}
		}
		//if we are the end and last operator was | or . that means the last operand is the last thing in the input so we need to make one last factory
		return factory();
	}

	Automata& factory() {
		auto op = operator_stack.top();
		operator_stack.pop();

		if (op == '|') {
			auto right = operand_stack.top();
			operand_stack.pop();
			auto left = operand_stack.top();
			operand_stack.pop();
			return left->Union(*right);
		}

		else if (op == '.') {
			auto right = operand_stack.top();
			operand_stack.pop();
			auto left = operand_stack.top();
			operand_stack.pop();
			return left->Concatenate(*right);
		}

		else {
			auto left = operand_stack.top();
			operand_stack.pop();
			return left->Star();
		}
	}
public:
	Parser();
	~Parser();

	Automata& parse(std::string text) {
		return _parse(text.begin(), text.end(), text);
	}
};


#include <iostream>
#include "Utility.h"


int main() {
	Automata Automata;
	return 0;
}

//class ExpressionChecker {
	//	const int HAS_OPERATOR = 5;
	//	string_it& _start;
	//	string_it& _end;
	//	std::string& _text;
	//	StringNumberElementChecker& _monoid_checker;
	//
	//	int state;
	//	MTAutomata<StringNumber> left_operand;
	//	MTAutomata<StringNumber> right_operand;
	//	int result;
	//public:
	//	ExpressionChecker(string_it& start, string_it& end, std::string& text, StringNumberElementChecker& monoid_checker)
	//		:_start(start), _end(end), _text(text), state(FINISHED), _monoid_checker(monoid_checker) { }
	//
	//	void register_symbol(char symbol) {
	//		if (symbol == '(') {
	//			//this is returning the left operand
	//			return parse(++_start, _end, _text);
	//		}
	//		
	//		if (symbol == ')' && state) {
	//			state = FINISHED;
	//			return;
	//		}
	//
	//		if (symbol == '*') {
	//			//return result*
	//		}
	//
	//		if (symbol == '.' || symbol == '|') {
	//			/*right_operand =*/ parse(++_start, _end, _text);
	//			//return (left_operand,symbol,right_operand)
	//		}
	//	}
	//
	//	bool success()const { return state == FINISHED; }
	//
	//
	//};
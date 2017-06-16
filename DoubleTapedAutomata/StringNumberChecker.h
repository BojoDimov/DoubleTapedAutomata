#pragma once
#include "typedefs.h"


class StringNumberChecker {
private:
	const int FIRST_LANE = 1;
	const int SECOND_LANE = 2;
	const int FINISHED = 3;

	int state;
	int number;
	std::string word;
public:
	StringNumberChecker()
		:state(FIRST_LANE), number(0) { }

	void reset() {
		state = FIRST_LANE;
		word = "";
		number = 0;
	}

	void register_symbol(char symbol) {
		if (state == FINISHED) {
			return;
		}

		if (symbol == '(') {
			reset();
			return;
		}

		if (symbol == ')') {
			state = FINISHED;
			return;
		}

		if (symbol == ',') {
			state = SECOND_LANE;
			return;
		}

		if (state == SECOND_LANE && symbol >= '0' && symbol <= '9') {
			number = number * 10 + (int)symbol - (int)'0';
			return;
		}

		word += symbol;
	}

	bool success()const { return state == FINISHED; }

	DTA factory() {
		return DTA(
		{ { 1,true, false },{ 2, false, true } },
		{ { 1,{ word, number }, 2 } }
		);
	}
};


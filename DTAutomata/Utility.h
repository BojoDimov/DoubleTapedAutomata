#pragma once
#include "MTAutomata.h"

struct StringNumber {
	std::string word;
	int number;
};

typedef MTAutomata<StringNumber> Automata;
typedef std::_String_iterator<std::_String_val<std::_Simple_types<char>>> string_it;

struct Node {
	Node* left;
	Node* right;
	Automata* automata;
};

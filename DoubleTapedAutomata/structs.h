#pragma once
#include <string>

template<typename Monoid>
struct ATransition {
	int source;
	Monoid m;
	int dest;
};

struct StateDescriptor {
	int state;
	bool is_starting;
	bool is_final;
};

struct StringNumber {
	std::string word;
	int number;
};

//TODO: make symbol of type int or char
struct SymbolNumber {
	std::string symbol;
	int number;
};
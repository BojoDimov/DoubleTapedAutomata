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

struct SymbolNumber {
	int symbol;
	int number;
};
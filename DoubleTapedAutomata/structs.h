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

struct IntPair {
	int a;
	int b;

	bool operator==(IntPair other) const {
		return a == other.a && b == other.b;
	}

	bool operator!=(IntPair other) const {
		return a != other.a || b != other.b;
	}

	bool operator<(IntPair other) const{
		if (a == other.a) {
			if (b == other.b) {
				return false;
			}
			else {
				return b < other.b;
			}
		}
		else {
			return a < other.a;
		}
	}
};

struct IntPairHash {
	int first = 1000003;
	int second = 1000033;

	std::size_t operator()(IntPair const& s) const
	{
		return (first % s.a) ^ (second % s.b);
	}
};

struct OSTransition {
	IntPair source;
	IntPair m;
	IntPair dest;

	bool operator<(OSTransition other) const {
		return source < other.source;
	}
};
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>

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

template<typename Tape1, typename Tape2>
struct DoubleTape {
	Tape1 m;
	Tape2 n;
};

///MTAutomata - MultiTaped Automata
//algorithms may be improved if i keep explicit copy of all start and final states
template<typename Monoid>
class MTAutomata {
public:
	std::vector<ATransition<Monoid>> trn;
	std::vector<bool> is_starting;
	std::vector<bool> is_final;
	int disposition;
	int states_size;

	MTAutomata& Concatenate(const MTAutomata&);
	MTAutomata& Star(bool positive = false);
	MTAutomata& Union(const MTAutomata&);
	MTAutomata& Power();

	MTAutomata()
		: states_size(0), disposition(1) { }

	MTAutomata(const std::vector<StateDescriptor>& states, const std::vector<ATransition<Monoid>>& transitions, int Q = 0)
		: trn(transitions), states_size(states.size()), disposition(Q + 1) {
		init(states, Q);
	}

	void remap(int Q) {
		std::unordered_map<int, int> states_map;
		int current = Q;

		for (int i = 0; i < states_size; i++) {
			states_map[i + disposition] = ++current;
		}

		std::for_each(trn.begin(), trn.end(), [&](ATransition& t) {
			t.source = states_map[t.source];
			t.dest = states_map[t.dest];
		});

		disposition = Q + 1;
	}

private:
	void init(std::vector<StateDescriptor> states, int Q = 0) {

		int current = Q;
		std::unordered_map<int, int> states_map;

		std::for_each(states.cbegin(), states.cend(), [&](const StateDescriptor& s) {
			states_map[s.state] = ++current;
			is_starting.push_back(s.is_starting);
			is_final.push_back(s.is_final);
		});

		std::for_each(trn.begin(), trn.end(), [&](ATransition<Monoid>& t) {
			t.source = states_map[t.source];
			t.dest = states_map[t.dest];
		});
	}
};


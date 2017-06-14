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

	MTAutomata()
		: states_size(0), disposition(1) {
		std::cout<<"constructor of automata called\n";
	}

	MTAutomata(const std::vector<StateDescriptor>& states, const std::vector<ATransition<Monoid>>& transitions, int Q = 0)
		: trn(transitions), states_size(states.size()), disposition(Q + 1) {
		init(states, Q);
		std::cout<<"constructor of automata called\n";
	}

	MTAutomata(const MTAutomata& o)
		: disposition(o.disposition),
		states_size(o.states_size),
		trn(o.trn),
		is_starting(o.is_starting),
		is_final(o.is_final)
	{
		std::cout << "copy-constructor of automata called\n";
	}

	void remap(int Q) {
		std::unordered_map<int, int> states_map;
		int current = Q;

		for (int i = 0; i < states_size; i++) {
			states_map[i + disposition] = ++current;
		}

		std::for_each(trn.begin(), trn.end(), [&](ATransition<Monoid>& t) {
			t.source = states_map[t.source];
			t.dest = states_map[t.dest];
		});

		disposition = Q + 1;
	}

	MTAutomata& cConcatenate(const MTAutomata& other) {
		MTAutomata A(other);
		std::vector<int> final_states;

		A.remap(states_size - 1 + disposition);

		for (int i = 0; i < states_size; i++) {
			if (is_final[i]) {
				is_final[i] = false;
				final_states.push_back(i + disposition);
			}
		}

		for (int i = 0; i < A.states_size; i++) {
			is_starting.push_back(false);
			is_final.push_back(A.is_final[i]);

			if (A.is_starting[i]) {
				std::for_each(final_states.cbegin(), final_states.cend(), [&](int state) {
					trn.push_back({ state, Monoid(), i + A.disposition });
				});
			}
		}

		states_size += A.states_size;
		trn.insert(trn.end(), A.trn.begin(), A.trn.end());
		return *this;
	}

	MTAutomata& cUnion(const MTAutomata& other) {
		MTAutomata A(other);
		A.remap(states_size - 1 + disposition);

		for (int i = 0; i < A.states_size; i++) {
			is_starting.push_back(A.is_starting[i]);
			is_final.push_back(A.is_final[i]);
		}

		states_size += A.states_size;
		trn.insert(trn.end(), A.trn.begin(), A.trn.end());
		return *this;
	}

	MTAutomata& Star(bool positive = false) {
		int new_state = states_size + disposition;

		for (int i = 0; i < states_size; i++) {
			if (is_starting[i]) {
				trn.push_back({ new_state, Monoid(), i + disposition });
				is_starting[i] = false;
			}

			if (is_final[i]) {
				trn.push_back({ i + disposition, Monoid(), new_state });
				is_final[i] = positive ? true : false;
			}
		}

		is_starting.push_back(true);
		is_final.push_back(positive ? false : true);
		states_size += 1;
		return *this;
	}

	MTAutomata& Concatenate(MTAutomata& A) {
		std::vector<int> final_states;

		A.remap(states_size - 1 + disposition);

		for (int i = 0; i < states_size; i++) {
			if (is_final[i]) {
				is_final[i] = false;
				final_states.push_back(i + disposition);
			}
		}

		for (int i = 0; i < A.states_size; i++) {
			is_starting.push_back(false);
			is_final.push_back(A.is_final[i]);

			if (A.is_starting[i]) {
				std::for_each(final_states.cbegin(), final_states.cend(), [&](int state) {
					trn.push_back({ state, Monoid(), i + A.disposition });
				});
			}
		}

		states_size += A.states_size;
		trn.insert(trn.end(), A.trn.begin(), A.trn.end());
		return *this;
	}

	MTAutomata& Union(MTAutomata& A) {
		A.remap(states_size - 1 + disposition);

		for (int i = 0; i < A.states_size; i++) {
			is_starting.push_back(A.is_starting[i]);
			is_final.push_back(A.is_final[i]);
		}

		states_size += A.states_size;
		trn.insert(trn.end(), A.trn.begin(), A.trn.end());
		return *this;
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


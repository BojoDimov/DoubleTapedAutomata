#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include "structs.h"


///MonoidAutomata - Multi-taped Automata
//algorithms may be improved if i keep explicit copy of all start and final states
template<typename Monoid>
class MonoidAutomata
{
protected:
	MonoidAutomata(
		int d,
		int s,
		std::vector<bool>& st,
		std::vector<bool>& fn,
		std::vector<ATransition<Monoid>>& t
	)
		: disposition(d),
		states_size(s),
		is_starting(st),
		is_final(fn),
		trn(t)
	{ }

public:
	std::vector<ATransition<Monoid>> trn;
	std::vector<bool> is_starting;
	std::vector<bool> is_final;
	int disposition;
	int states_size;

	MonoidAutomata()
		: states_size(0), disposition(1) {
		std::cout << "constructor of automata called\n";
	}

	MonoidAutomata(
		const std::vector<StateDescriptor>& states, 
		const std::vector<ATransition<Monoid>>& transitions, 
		int Q = 0
	)
		: trn(transitions), states_size(states.size()), disposition(Q + 1) {
		init(states, Q);
		std::cout << "constructor of automata called\n";
	}

	MonoidAutomata(const MonoidAutomata& o)
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

	MonoidAutomata& Star(bool positive = false) {
		int new_state = states_size + disposition;

		for (int i = 0; i < states_size; i++) {
			if (is_starting[i]) {
				trn.push_back({ new_state, { }, i + disposition });
				is_starting[i] = false;
			}

			if (is_final[i]) {
				trn.push_back({ i + disposition, { }, new_state });
				is_final[i] = positive ? true : false;
			}
		}

		is_starting.push_back(true);
		is_final.push_back(positive ? false : true);
		states_size += 1;
		return *this;
	}

	MonoidAutomata& Concatenate(MonoidAutomata& A) {
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
					trn.push_back({ state, { }, i + A.disposition });
				});
			}
		}

		states_size += A.states_size;
		trn.insert(trn.end(), A.trn.begin(), A.trn.end());
		return *this;
	}

	MonoidAutomata& Union(MonoidAutomata& A) {
		A.remap(states_size - 1 + disposition);

		for (int i = 0; i < A.states_size; i++) {
			is_starting.push_back(A.is_starting[i]);
			is_final.push_back(A.is_final[i]);
		}

		states_size += A.states_size;
		trn.insert(trn.end(), A.trn.begin(), A.trn.end());
		return *this;
	}

	///makes the transitions on the first lane by one symbol or epsillon
	MonoidAutomata& Sequalize() {
		std::cout << "Not specialized function\n";
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

template <>
MonoidAutomata<StringNumber>& MonoidAutomata<StringNumber>::Sequalize() {
	std::vector<ATransition<StringNumber>> new_transitions;
	new_transitions.reserve(trn.capacity());

	for (auto it = trn.begin(); it != trn.end(); it++) {
		if (it->m.word.length() > 1) {
			int prev_state = it->source;
			int next_state = states_size + disposition;

			for (int i = 0; i < it->m.word.length() - 1; i++) {
				new_transitions.push_back({ prev_state, { it->m.word.substr(i,1), i == 0 ? it->m.number : 0}, next_state });
				is_starting.push_back(false);
				is_final.push_back(false);

				prev_state = next_state;
				next_state = (i == it->m.word.length() - 1 ? it->dest : next_state + 1);
			}
			new_transitions.push_back({ prev_state, { it->m.word.substr(it->m.word.length() - 1 ,1), 0 }, it->dest });
		}
		else {
			new_transitions.push_back(*it);
		}
	}

	trn = new_transitions;
	return *this;
}



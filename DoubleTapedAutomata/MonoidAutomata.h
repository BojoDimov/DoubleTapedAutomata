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
public:
	std::vector<ATransition<Monoid>> trn;
	std::vector<bool> is_starting;
	std::vector<bool> is_final;
	int disposition;
	int states_size;

	MonoidAutomata()
		: states_size(0), disposition(1) {
		//std::cout << "constructor of automata called\n";
	}

	MonoidAutomata(
		const std::vector<StateDescriptor>& states, 
		const std::vector<ATransition<Monoid>>& transitions, 
		int Q = 0
	)
		: trn(transitions), states_size(states.size()), disposition(Q + 1) {
		init(states, Q);
		//std::cout << "constructor of automata called\n";
	}

	MonoidAutomata(const MonoidAutomata& o)
		: disposition(o.disposition),
		states_size(o.states_size),
		trn(o.trn),
		is_starting(o.is_starting),
		is_final(o.is_final)
	{
		//std::cout << "copy-constructor of automata called\n";
	}

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

	//creates e-transitions
	MonoidAutomata& Concatenate_old(MonoidAutomata& A) {
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

	//optimized concatenation(does not create e-transitions)
	MonoidAutomata& Concatenate(MonoidAutomata& A) {
		std::vector<int> final_states;
		std::vector<int> starting_states;

		A.remap(states_size - 1 + disposition);

		for (int i = 0; i < states_size; i++) {
			if (is_final[i]) {
				is_final[i] = false;
				final_states.push_back(i + disposition);
			}
		}

		std::vector<ATransition<Monoid>> additional_trn1;
		std::vector<ATransition<Monoid>> additional_trn2;
		additional_trn2.reserve(A.trn.size());

		for (int i = 0; i < A.trn.size(); i++) {
			if (A.is_starting[A.trn[i].source - A.disposition]) {
				for (int j = 0; j < final_states.size(); j++) {
					additional_trn1.push_back({ final_states[j], A.trn[i].m, A.trn[i].dest });
				}
			}
			else {
				additional_trn2.push_back(A.trn[i]);
			}
		}

		int current = states_size - 1 + disposition;
		std::unordered_map<int, int> rmp;
		std::vector<bool> new_starting;
		std::vector<bool> new_final;
		for (int i = 0; i < A.states_size; i++) {
			if (!A.is_starting[i]) {
				new_starting.push_back(false);
				if (A.is_final[i]) {
					new_final.push_back(true);
				}
				else {
					new_final.push_back(false);
				}
				states_size++;
				rmp[i + A.disposition] = ++current;
			}
		}

		std::for_each(additional_trn2.begin(), additional_trn2.end(), [&](ATransition<Monoid>& t) {
			t.source = rmp[t.source];
			t.dest = rmp[t.dest];
		});

		std::for_each(additional_trn1.begin(), additional_trn1.end(), [&](ATransition<Monoid>& t) {
			t.dest = rmp[t.dest];
		});

		trn.insert(trn.end(), additional_trn2.begin(), additional_trn2.end());
		trn.insert(trn.end(), additional_trn1.begin(), additional_trn1.end());
		is_starting.insert(is_starting.end(), new_starting.begin(), new_starting.end());
		is_final.insert(is_final.end(), new_final.begin(), new_final.end());

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
		std::cout << "Not specialized function 'Sequalize'\n";
		return *this;
	}

	MonoidAutomata<SymbolNumber> MakeRTT() {
		std::cout << "Not specialized function 'MakeRTT'\n";
		return MonoidAutomata<SymbolNumber>();
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
				states_size++;
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



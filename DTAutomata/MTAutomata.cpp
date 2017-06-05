#include "MTAutomata.h"


template<typename Monoid>
MTAutomata<Monoid>& MTAutomata<Monoid>::Concatenate(const MTAutomata& other) {
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

template<typename Monoid>
MTAutomata<Monoid>& MTAutomata<Monoid>::Union(const MTAutomata& other) {
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

template<typename Monoid>
MTAutomata<Monoid>& MTAutomata<Monoid>::Star(bool positive = false) {
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

template<typename Monoid = DoubleTape>
MTAutomata<Monoid>& MTAutomata<Monoid>::Power() {
	return *this;
}
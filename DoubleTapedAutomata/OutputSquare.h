#pragma once
#include <set>
#include <map>
#include "typedefs.h"

class OutputSquare {
public:
	std::vector<IntPair> states;
	std::vector<OSTransition> trn;
	std::vector<bool> is_starting;
	std::vector<bool> is_final;

	OutputSquare(RTT rtt)
	{
		std::sort(rtt.trn.begin(), rtt.trn.end(), [](const RTTTransition& a, const RTTTransition& b) {
			return a.m.symbol < b.m.symbol;
		});

		std::vector<IntPair> symbol_index;
		std::string current_symbol = rtt.trn[0].m.symbol;
		int start = 0;
		for (int i = 1; i < rtt.trn.size(); i++) {
			if (rtt.trn[i].m.symbol != current_symbol) {
				symbol_index.push_back({ start, i });
				start = i;
				current_symbol = rtt.trn[i].m.symbol;
			}

			if (i == rtt.trn.size() - 1) {
				symbol_index.push_back({ start, i + 1 });
			}
		}

		//create delta
		std::set<IntPair> st;
		for (int i = 0; i < symbol_index.size(); i++) {
			for (int q = symbol_index[i].a; q < symbol_index[i].b; q++) {
				for (int p = symbol_index[i].a; p < symbol_index[i].b; p++) {
					trn.push_back({
						{ rtt.trn[q].source, rtt.trn[p].source },
						{ rtt.trn[q].m.number, rtt.trn[p].m.number },
						{ rtt.trn[q].dest, rtt.trn[p].dest }
					});

					//st.insert({ rtt.trn[q].source, rtt.trn[p].source });
					//st.insert({ rtt.trn[q].dest, rtt.trn[p].dest });
				}
			}
		}

		//create states from the information taken from delta
		for (auto it = st.begin(); it != st.end(); it++) {
			states.push_back(*it);
			if (rtt.is_starting[it->a - rtt.disposition] && rtt.is_starting[it->b - rtt.disposition]) {
				is_starting.push_back(true);
			}
			else {
				is_starting.push_back(false);
			}

			if (rtt.is_final[it->a - rtt.disposition] && rtt.is_final[it->b - rtt.disposition]) {
				is_final.push_back(true);
			}
			else {
				is_final.push_back(false);
			}
		}
	}

	bool is_functional() {
		std::map<IntPair, IntPair> adm;
		std::map<IntPair, IntPair> _step;
		bool state_test = false;
		bool finished = false;

		for (int i = 0; i < states.size(); i++) {
			if (is_starting[i]) {
				adm[states[i]] = { 0, 0 };
			}
		}

		while (!finished) {
			_step = step(adm, state_test);
			if (state_test) {
				return false;
			}

			if (_step.size() == 0) {
				finished = true;
			}
			else {
				adm.insert(_step.begin(), _step.end());
			}
		}

		for (int i = 0; i < states.size(); i++) {
			if (is_final[i]) {
				auto w = adm.find(states[i])->second;
				if (w.a != 0 || w.b != 0) {
					return false;
				}
			}
		}

		return true;
	}

	std::map<IntPair, IntPair> step(const std::map<IntPair, IntPair>& adm, bool& state_test) {
		std::map<IntPair, IntPair> result;
		int x, y, a, b, c, wa, wb;

		for (auto it = adm.begin(); it != adm.end(); it++) {
			auto transitions = get_transitions(it->first);
			for (int i = 0; i < transitions.size(); i++) {
				x = it->second.a;
				y = it->second.b;
				a = transitions[i].m.a;
				b = transitions[i].m.b;
				c = std::min(x + a, y + b);
				wa = x + a - c;
				wb = y + b - c;
				auto pair = adm.find(transitions[i].dest);
				if (pair != adm.end()) {
					if (pair->second.a != wa || pair->second.b != wb) {
						//that mean |Adm(transitions[i].dest)|>1
						state_test = true;
						return result;
					}
				}
				else {
					//check for balanced
					if (wa != 0 && wb != 0) {
						state_test = true;
						return result;
					}

					result[transitions[i].dest] = { wa,wb };
				}
			}
		}

		return result;
	}

	std::vector<OSTransition> get_transitions(IntPair source) {
		std::vector<OSTransition> result;

		for (int i = 0; i < trn.size(); i++) {
			if (trn[i].source == source) {
				result.push_back(trn[i]);
			}
		}

		return result;
	}
};
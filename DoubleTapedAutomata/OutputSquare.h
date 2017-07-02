#pragma once
#include <set>
#include <unordered_set>
#include <map>
#include "typedefs.h"

class OutputSquare {
public:
	std::vector<IntPair> states;
	//std::set<IntPair> states;
	std::vector<OSTransition> trn;
	std::vector<bool> is_starting;
	std::vector<bool> is_final;
	std::map<IntPair, IntPair> source_index;

	OutputSquare(RTT& rtt)
	{
		if (rtt.trn.size() == 0) {
			return;
		}

		discover_states(rtt);

		init_states(rtt);
	}

	//IntPair - a = number; b = dest
	typedef std::unordered_map<int, std::map<std::string, std::vector<IntPair>>> rtt_index;

	rtt_index create_rtt_index(RTT& rtt) {
		rtt_index index;
		for (const auto& t : rtt.trn) {
			index[t.source][t.m.symbol].push_back({ t.m.number, t.dest });
		}

		return index;
	}

	void discover_states(RTT& rtt) {
		/*std::unordered_set<IntPair, IntPairHash> states_marker;*/
		auto index = create_rtt_index(rtt);
		std::set<IntPair> states_marker;

		//init stating states
		for (int i = 0; i < rtt.states_size; i++) {
			if (rtt.is_starting[i]) {
				for (int j = 0; j < rtt.states_size; j++) {
					if (rtt.is_starting[j]) {
						states.push_back({ i + rtt.disposition, j + rtt.disposition });
						states_marker.insert({ i + rtt.disposition, j + rtt.disposition });
					}
				}
			}
		}

		int n = 0, iterations = 0;
		while (n < states.size()) {
			auto cs = states[n];
			auto trn1 = index[cs.a];
			auto trn2 = index[cs.b];
			for (auto i = trn1.begin(), j = trn2.begin(); i != trn1.end() && j != trn2.end(); i++, j++) {
				//std::map is sorted so we can do this
				while (i != trn1.end() && j != trn2.end() && i->first < j->first) {
					i++;
				}

				while (j != trn2.end() && i != trn1.end() && j->first < i->first) {
					j++;
				}

				//if the are not trn.end(), they are equal
				if (i != trn1.end() && j != trn2.end()) {
					for (const auto& m1 : i->second) {
						for (const auto& m2 : j->second) {
							IntPair dest = { m1.b, m2.b };
							trn.push_back({ cs, { m1.a, m2.a}, dest });
							if (states_marker.find(dest) == states_marker.end()) {
								states_marker.insert(dest);
								states.push_back(dest);
							}
						}
					}
				}

				//this is needed because we cant ++ on end() iterator
				if (i == trn1.end() || j == trn2.end()) {
					break;
				}
			}
			n++;
		}
	}

	void init_states(const RTT& rtt) {
		for (const auto& st: states){
			if (rtt.is_starting[st.a - rtt.disposition] && rtt.is_starting[st.b - rtt.disposition]) {
				is_starting.push_back(true);
			}
			else {
				is_starting.push_back(false);
			}

			if (rtt.is_final[st.a - rtt.disposition] && rtt.is_final[st.b - rtt.disposition]) {
				is_final.push_back(true);
			}
			else {
				is_final.push_back(false);
			}
		}
	}

	void create_index() {
		std::sort(trn.begin(), trn.end(), [](const OSTransition& a, const OSTransition& b) {
			return a.source < b.source;
		});

		auto current = trn[0].source;
		int start = 0;
		for (int i = 1; i < trn.size(); i++) {
			if (current != trn[i].source) {
				source_index[current] = { start, i };
				start = i;
				current = trn[i].source;
			}

			if (i == trn.size() - 1) {
				source_index[current] = { start, i + 1 };
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
				auto w = adm.find(states[i]);
				if (w == adm.end()) {
					continue;
				}
				if (w->second.a != 0 || w->second.b != 0) {
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

		auto finding = source_index[source];

		for (int i =finding.a; i < finding.b; i++) {
			if (trn[i].source == source) {
				result.push_back(trn[i]);
			}
		}

		return result;
	}
};
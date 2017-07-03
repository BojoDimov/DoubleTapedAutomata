#pragma once
#include <set>
#include <unordered_set>
#include <map>
#include "typedefs.h"

typedef std::unordered_map<int, std::map<std::string, std::vector<IntPair>>> rtt_index;
//map<q, map<p, vector<pair<m, dest>>>
typedef std::unordered_map<int, std::unordered_map<int, std::vector<std::pair<IntPair, IntPair>>>> os_index;

class OutputSquare {
public:
	std::vector<IntPair> states;
	//std::set<IntPair> states;
	std::vector<OSTransition> trn;
	std::vector<bool> is_starting;
	std::vector<bool> is_final;
	std::map<IntPair, IntPair> source_index;
	os_index index;

	OutputSquare(RTT& rtt)
	{
		if (rtt.trn.size() == 0) {
			return;
		}

		std::vector<RTTTransition> test;
		for (int i = 0; i < rtt.trn.size(); i++) {
			if (rtt.trn[i].source == 65) {
				test.push_back(rtt.trn[i]);
			}
		}

		discover_states(rtt);

		init_states(rtt);
	}

	//IntPair - a = number; b = dest

	rtt_index create_rtt_index(RTT& rtt) {
		rtt_index index;
		for (const auto& t : rtt.trn) {
			index[t.source][t.m.symbol].push_back({ t.m.number, t.dest });
		}

		return index;
	}

	void discover_states(RTT& rtt) {
		auto index = create_rtt_index(rtt);
		std::set<IntPair> states_marker;
		std::set<IntPair> symmetric_marked;

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

		int n = 0;
		while (n < states.size()) {
			auto cs = states[n];
			if (symmetric_marked.find(cs) != symmetric_marked.end()) {
				n++;
				continue;
			}
			else if (cs.a != cs.b) {
				symmetric_marked.insert({ cs.b, cs.a });
			}

			auto trn1 = index[cs.a];
			auto trn2 = index[cs.b];
			auto i = trn1.begin();
			auto j = trn2.begin();
			while (i != trn1.end() && j != trn2.end())
			{
				if (i->first < j->first) {
					i++;
					continue;
				}

				if (j->first < i->first) {
					j++;
					continue;
				}

				if(i->first == j->first) {
					for (const auto& m1 : i->second) {
						for (const auto& m2 : j->second) {
							IntPair dest = { m1.b, m2.b };
							trn.push_back({ cs, { m1.a, m2.a}, dest });
							if (cs.a != cs.b) {
								//push the symmetric transitions <b,a>
								trn.push_back({ {cs.b, cs.a } , { m2.a, m1.a }, { m2.b, m1.b } });
							}
							if (states_marker.find(dest) == states_marker.end()) {
								states_marker.insert(dest);
								states.push_back(dest);
								if (cs.a != cs.b) {
									states_marker.insert({ dest.b, dest.a });
									states.push_back({ dest.b, dest.a });
								}
							}
						}
					}
					i++;
					j++;
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

	void create_index2() {
		index.clear();
		for (int i = 0; i < trn.size(); i++) {
			auto source = trn[i].source;
			index[source.a][source.b].push_back(std::make_pair(trn[i].m, trn[i].dest));
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
#pragma once
#include <set>
#include "typedefs.h"
#include "structs.h"
#include "MonoidAutomata.h"

struct Path {
	int start;
	int label;
	int end;

	bool operator<(Path o) const {
		if (start == o.start) {
			if (label == o.label) {
				if (end == o.end) {
					return false;
				}
				else {
					return end < o.end;
				}
			}
			else {
				return label < o.label;
			}
		}
		else {
			return start < o.start;
		}
	}
};

//TODO:
//functionalization of delta only for e-transitions
//rework this using 'range' for the return type
std::vector<DTATransition> F(int state, std::vector<DTATransition> trn) {
	std::vector<DTATransition> result;
	result.reserve(trn.size());

	for (const auto& t : trn) {
		if (t.source == state && t.m.word.length() == 0) {
			result.push_back(t);
		}
	}

	result.shrink_to_fit();
	return result;
}

std::vector<Path> dc(int size, int d, std::vector<DTATransition>& trn) {
	std::vector<Path> result;
	result.reserve(size * 3);
	std::set<Path> paths;

	//step 0
	for (int i = 0; i < size; i++) {
		result.push_back({ i + d, 0, i + d });
		paths.insert({ i + d, 0, i + d });
	}

	int n = 0;
	while (n != result.size()) {
		for (const auto& t : F(result[n].end, trn)) {
			//invariant t.source = result[n].end
			Path path = { result[n].start, result[n].label + t.m.number, t.dest };
			//проверка за безкрайна многозначност
			if (path.start == path.end && path.label != 0) {
				std::cout << "Automata is Infinitely multifaceted - cannont produce real-time transducer\n";
				return result;
			}

			if (paths.find(path) == paths.end()) {
				result.push_back(path);
				paths.insert(path);
			}
		}
		n++;
	}

	return result;
}

//TODO:rework all functionalizations using the range based pattern to avoid copying new items and save memory
//functionalize delta_closure 2~>1
std::vector<Path> f_2_1(int state, std::vector<Path>& paths) {
	std::vector<Path> result;
	for (auto path : paths) {
		if (path.end == state) {
			result.push_back(path);
		}
	}
	return result;
}

//functionalize delta_closure 1~>2
std::vector<Path> f_1_2(int state, std::vector<Path>& paths) {
	std::vector<Path> result;
	for (auto path : paths) {
		if (path.start == state) {
			result.push_back(path);
		}
	}
	return result;
}

template <>
MonoidAutomata<SymbolNumber> MonoidAutomata<StringNumber>::MakeRTT() {
	//auto delta_closure = dc(get_states(trn), trn);//this is extremelly wrong as every states is in closure with itself
	auto delta_closure = dc(states_size, disposition, trn);
	std::vector<RTTTransition> rtt_trn;

	for (auto it = trn.begin(); it != trn.end(); it++) {
		if (it->m.word.length() != 0) {
			auto bc = f_2_1(it->source, delta_closure);
			auto fc = f_1_2(it->dest, delta_closure);

			for (const auto& b : bc) {
				for (const auto& f : fc) {
					rtt_trn.push_back({ b.start, {it->m.word, b.label + it->m.number + f.label}, f.end });
				}
			}
		}
	}

	std::cout << "Specialised 'MakeRTT'\n";
	return MonoidAutomata<SymbolNumber>(disposition, states_size, is_starting, is_final, rtt_trn);
}
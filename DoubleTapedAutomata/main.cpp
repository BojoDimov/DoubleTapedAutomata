#include "RPNParser.h"
#include "RTT.h"

//Automata(state_descriptor(state, is_starting, is_final) transitions(source, m, dest)
int main() {
	DTA A1(
		{
			{ 1, true, false },
			{ 2, false, true }
		}, 
		{
			{ 1, { "a", 1 }, 2 }
		});

	auto copy_temp = A1;
	A1.Concatenate(copy_temp).Union(copy_temp).Star();
	A1.Sequalize();

	DTA A2(
	{
		{ 1, true, false },
		{ 2, false, true }
	},
	{
		{ 1,{ "baba", 4 }, 2 }
	});

	A2.Sequalize().MakeRTT();
	//auto res = dc(A2.states_size, A2.disposition, A2.trn);

	std::string automata1_input = "(a,1)(a,1).(a,1)|*";
	RPNParser parser;
	auto result = parser.parse(automata1_input);
	return 0;
}
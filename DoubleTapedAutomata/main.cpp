#include "Utility.h"
#include "RPNParser.h"

///Automata(state_descriptor(state, is_starting, is_final) transitions(source, m, dest)
int main() {
	Automata A1(
		{
			{ 1, true, false },
			{ 2, false, true }
		}, 
		{
			{ 1, { "a", 1 }, 2 }
		});
	auto copy_temp = A1;
	A1.Concatenate(copy_temp).Union(copy_temp).Star();

	std::string automata1_input = "(a,1)(a,1).(a,1)|*";
	RPNParser_vector_operands parser;
	//logic is OK
	//too many calls of copy constructor
	auto result = parser.parse(automata1_input);
	return 0;
}
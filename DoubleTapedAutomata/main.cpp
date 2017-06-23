#include <ctime>
#include <fstream>
#include <ctime>
#include "RPNParser.h"
#include "RTT.h"
#include "OutputSquare.h"

//Automata(state_descriptor(state, is_starting, is_final) transitions(source, m, dest)
int main(int argc, char ** argv) {
	/*DTA A1(
		{
			{ 1, true, false },
			{ 2, false, true },
			{ 3, false, true },
			{ 4, false, true }
		}, 
		{
			{ 1, { "a", 1 }, 2 }
		});

	DTA A2(
	{
		{ 1, true, false },
		{ 2, false, true },
		{ 3, true, false },
		{ 4, false, true },
		{ 5, true, false },
		{ 6, false, true }
	},
	{
		{ 1,{ "b", 1 }, 2 },
		{ 3,{ "c", 1 }, 4 },
		{ 5,{ "b", 1 }, 6 }
	});

	A1.Concatenate(A2);*/

	RPNParser parser;
	std::string regex;
	std::string line;
	std::ifstream myfile(argv[1]);
	auto timer = clock();
	if(myfile.is_open()){
		while (std::getline(myfile, line))
		{
			regex += line;
		}
		myfile.close();
	}
	else {
		std::cout << "File not found!\nTerminating\n";
	}

	/// (double)CLOCKS_PER_SEC

	std::cout << "Reading file completed. Time elapsed: " << (clock() - timer)  << "ms" << std::endl;
	timer = clock();
	auto A = parser.parse(regex);
	std::cout << "Parsing regex completed. Time elapsed: " << (clock() - timer) << "ms" << std::endl;
	timer = clock();
	A.Sequalize();
	std::cout << "Sequalizing completed. Time elapsed: " << (clock() - timer) << "ms" << std::endl;
	timer = clock();
	auto B = A.MakeRTT();
	std::cout << "Real-time transducer created. Time elapsed: " << (clock() - timer) << "ms" << std::endl;
	timer = clock();
	OutputSquare C(B);
	std::cout << "Output square created. Time elapsed: " << (clock() - timer) << "ms" << std::endl;
	timer = clock();
	C.create_index();
	std::cout << "Index created. Time elapsed: " << (clock() - timer) << "ms" << std::endl;
	timer = clock();
	auto is_function = C.is_functional();
	std::cout << "Function check completed. Time elapsed: " << (clock() - timer) << "ms" << "\nResult:" << is_function << std::endl;
	return 0;
}
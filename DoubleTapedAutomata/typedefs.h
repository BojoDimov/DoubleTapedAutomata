#pragma once
#include "structs.h"
#include "MonoidAutomata.h"

typedef MonoidAutomata<StringNumber> DTA;
typedef ATransition<StringNumber> DTATransition;

typedef MonoidAutomata<SymbolNumber> RTT;
typedef ATransition<SymbolNumber> RTTTransition;

typedef std::_String_iterator<std::_String_val<std::_Simple_types<char>>> string_it;



#ifndef PARSER_HPP
#define PARSER_HPP
#include "lexer.hpp"
#include "token.hpp"
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;
typedef std::vector<Token> tokenvect;
typedef std::vector<vector<Token>> all_lines;

struct package {

	all_lines instructions;
	vector<string> labels;
	vector<size_t> label_lines;
	vector<string> varType;
	vector<string> varValue;
	vector<string> varName;
	size_t parse_line = 0;
	int text_start = 0;
};

//we need a parser for this, mainly a function that takes in TokenLists and writes out to a seperate file
class parser {

public:
	//inputs the tokenlist from the lexer and gives a boolean output to determine if it is correct 
bool ParseMips(const TokenList& list);
bool ParseData(tokenvect tokens);
bool ParseText(tokenvect tokens);
int RegTable(std::string registers);
package getpackage();

	//a few functions to check size of the storage variables
bool CheckWord(double possibleword, bool sign);
bool CheckHalf(double possiblehalf, bool sign);
bool CheckByte(double possiblebyte, bool sign);
bool CheckSpace(double possiblespace, bool sign);
bool CanStoi(string in);

size_t line = 0;
tokenvect linevect; //stores the line token with string and line

//output these to the VirtualMachine
vector<string> labels;
size_t text_line = 0;
vector<size_t> label_line; //not that helpful but maybe we can use this to help find the PC value equiv
vector<string> varType;
vector<string> varValue;
vector<string> varName; 
all_lines text_section; 
int text_start = 0;
};
#endif

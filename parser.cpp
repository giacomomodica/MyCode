#include "parser.hpp"
#include <vector>
#include <cmath>
#include <iostream>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

bool parser::ParseMips(const TokenList& list)
{
	TokenList listcopy = list;
	string CurrReg;
	tokenvect tokens;
	tokenvect temp;
	string error_beg = "Error:";
	string error_message;
	string print_error;
	char temporary_line;
	bool ret = true;
	bool ret1 = true;
	bool inside = false;
	size_t currentIndex = 0;//tracks the current location of the vector

	while (!listcopy.empty())
	{ //put the whole list into a vector
		tokens.push_back(listcopy.front());
		listcopy.pop_front();
	}

	while (currentIndex < tokens.size())
	{//main loop for parsing

		if ((tokens.at(currentIndex).value() == ".data") && (tokens.at(currentIndex + 1).type() == EOL))
		{//we should parse the data state now
			//copy the vector from the current index
			inside = true;
			for (size_t i = currentIndex; tokens.at(i).value() != ".text"; i++)
			{//push until we have the text part of the code
				if (tokens.at(i).value() == "#")
				{
					while (tokens.at(i - 1).type() != EOL)
					{
						if (i + 1 < tokens.size())
						{
							if (tokens.at(i + 1).value().find(".text") != string::npos)
							{
								break;
							}
						}
						if (tokens.at(i).type() == EOL)
						{
							temp.push_back(tokens.at(i));
						}
						i++;
					}
					currentIndex = i;
					if (currentIndex >= tokens.size())
					{
						break;
					}
				}
				if (tokens.at(i).value().find("#") == string::npos){
					temp.push_back(tokens.at(currentIndex));
				}
				else {
					if (tokens.at(i - 1).type() == EOL && tokens.at(i + 1).type() == EOL)//check if its EOL # EOL
					{
						currentIndex = i + 1;
					}
				}

				currentIndex++;
			}
			ret = ParseData(temp);
			temp.clear();
		}

		if ((currentIndex < tokens.size()) && (tokens.at(currentIndex).value() == ".text") && (tokens.at(currentIndex + 1).type() == EOL))
		{//we should parse the text state now
			text_start = tokens.at(currentIndex).line();
			inside = true;
			for (unsigned int i = (unsigned int)currentIndex; i < tokens.size(); i++)
			{//push until we have the text part of the code
				
				if (tokens.at(i).type() == ERROR)
				{
					return false;
				}
				if (tokens.at(i).type() == OPEN_PAREN)
				{
					temp.push_back(tokens.at(i));
					temp.push_back(tokens.at(i + 1));
					temp.push_back(tokens.at(i + 2));
					i += 3;
				}
				if (tokens.at(i).value() == "#")
				{
					while (tokens.at(i - 1).type() != EOL || tokens.at(i).value() == "#")
					{
						if (i == tokens.size() - 1)
						{
							break;
						}
						i++;
						if (tokens.at(i).type() == EOL)
						{
							temp.push_back(tokens.at(i));
							line++;
						}
					}
					currentIndex = i;
				}
				currentIndex = i;
				temp.push_back(tokens.at(currentIndex));
				currentIndex++;
			}
			ret1 = ParseText(temp);
			temp.clear();
		
		}
		currentIndex++;
	}

	if (!inside)
	{
		temporary_line = (char)line + '0';
		error_message = ": no .data or .text section detected\n";
		print_error = error_beg + temporary_line + error_message;
		//cerr << print_error;
		ret = false;
	}
	listcopy.clear();
	tokens.clear();
	return (ret && ret1);
}
bool parser::ParseData(tokenvect tokens)
{
	string error_beg = "Error:";
	string error_message;
	bool var_sign = false;
	char temporary_line;
	string print_error;
	enum varState { word, half, byte, space, ascii };
	varState MyState = word;
	size_t index = 2;
	double temp = 0;
	bool ret = true;
	bool tempcorrect = true;
	//char temporary_line;
	string teststring;
	size_t lineindex = 1;
	int delimcounter = 0;
	tokenvect mytokens = tokens; //create a copy to change

	while (index < tokens.size())
	{//we need to make sure this is empty by the end

		while (mytokens.at(index).type() != EOL)
		{
			if ((mytokens.at(index).type() == STRING_DELIM) && (delimcounter == 0))
			{
				delimcounter++;
				index++;
				//deleted a temp line store, shouldn't affect anything but just in case
			}
			else if (delimcounter > 0)
			{
				delimcounter = 0;
				break;
			}
			linevect.push_back(mytokens.at(index));
			index++;
		}
		if (linevect.size() < 3)
		{
			if (linevect.size() == 2 && linevect.at(0).type() == STRING && CanStoi(linevect.at(1).value()))
			{
				if (linevect.at(0).value() == ".space") {
					varName.push_back("XX");
					varType.push_back(linevect.at(0).value());
					varValue.push_back(linevect.at(1).value());
				}
				lineindex = 1;
				linevect.clear();
			}
			else
			{
				temporary_line = (char)line + '0';
				error_message = ": size of data declaration too small\n";
				print_error = error_beg + temporary_line + error_message;
				//cerr << print_error;
				ret = false;
				break;
			}
			
		}
		while (lineindex < linevect.size())
		{//parse the line

			if ((lineindex == 1) && (linevect.at(lineindex).type() == STRING))
			{//beginning of the linevector, save the value of the string
				if (linevect.at(lineindex).value().find(".word") != string::npos) {
					MyState = word;
				}
				else if (linevect.at(lineindex).value().find(".half") != string::npos) {
					MyState = half;
				}
				else if (linevect.at(lineindex).value().find(".byte") != string::npos) {
					MyState = byte;
				}
				else if (linevect.at(lineindex).value().find(".space") != string::npos) {
					MyState = space;
				}
				else if (linevect.at(lineindex).value().find(".ascii") != string::npos) {
					MyState = ascii;
				}
				lineindex++; //go to the next token
			}

			if (linevect.at(lineindex).type() == EQUAL && linevect.at(lineindex).value() != ".space")
			{
				varName.push_back(linevect.at(lineindex - 1).value());
				varValue.push_back(linevect.at(lineindex + 1).value());
				varType.push_back("XX");
				break;
			}

			if (linevect.at(lineindex - 2).value().find(":") != string::npos)
			{//must be a name
				varName.push_back(linevect.at(lineindex - 2).value());
				if (linevect.at(lineindex - 1).value().find(".") != string::npos)
				{//must be a name
					varType.push_back(linevect.at(lineindex - 1).value());
					varValue.push_back(linevect.at(lineindex).value());
				}
			}

			if ((linevect.at(lineindex - 1).value().find(".ascii") == string::npos) && (linevect.at(lineindex).type() == STRING))
			{
				if ((linevect.at(lineindex).value().find("+") != string::npos) || ((linevect.at(lineindex).value().find("-") != string::npos)))
				{//set the sign var
					var_sign = true;
				}

				teststring = linevect.at(lineindex).value();

				if (!CanStoi(teststring))
				{
					for (size_t i = 0; i < varName.size(); i++)
					{
						if (varName.at(i) == teststring)
						{
							teststring = varValue.at(i);
						}
					}
				}
				else
				{
					temp = (int)stol(teststring);
				}
			}

			switch (MyState)
			{
			case word:
				tempcorrect = CheckWord(temp, var_sign);
				if (tempcorrect)
					break;
			case half:
				tempcorrect = CheckHalf(temp, var_sign);
				if (tempcorrect)
					break;
			case byte:
				tempcorrect = CheckByte(temp, var_sign);
				if (tempcorrect)
					break;
			case space:
				tempcorrect = CheckSpace(temp, var_sign);
				if (tempcorrect)
					break;
			case ascii:
				//not sure yet
				break;
			default:
				//um idk
				break;
			}

			if (!tempcorrect)
			{
				temporary_line = (char)line + '0';
				error_message = ": variable overload\n";
				print_error = error_beg + temporary_line + error_message;
				//cerr << print_error;
				ret = false;
				tempcorrect = true;
				break;
			}

			var_sign = false;
			while (lineindex + 1 < linevect.size())
			{
				if (linevect.at(lineindex + 1).type() == SEP)
				{
					if (lineindex + 2 == linevect.size())
					{
						temporary_line = (char)line + '0';
						error_message = ": cannot end a line in a comma\n";
						print_error = error_beg + temporary_line + error_message;
						//cerr << print_error;
						ret = false;
						break;
					}
					if (linevect.at(lineindex + 2).value().find(":") != string::npos)
					{
						temporary_line = (char)line + '0';
						error_message = ": cannot end a line in a comma\n";
						print_error = error_beg + temporary_line + error_message;
						//cerr << print_error;
						ret = false;
						break;
					}
					lineindex += 2;
				}
			}
			if (lineindex + 1 >= linevect.size())
			{
				break;
			}
			teststring.clear();
			break;

		}
		lineindex = 1;
		index++;
		linevect.clear();
	}
	return ret;
}
bool parser::ParseText(tokenvect tokens)
{

	string error_beg = "Error:";
	bool big_break = false;
	char temporary_line;
	string error_message;
	bool ret = true;
	string print_error;
	size_t index = 2;
	bool bad_reg = true;
	linevect.clear();
	bool data_movement = false;
	bool int_arith = false;
	bool control_instr = false;
	string teststring;
	varName.push_back("main:");
	bool found_label = false;
	bool two_reg = false;
	string mynop = "nop";
	size_t lineindex = 0;
	
	int delimcounter = 0;
	tokenvect mytokens = tokens; //create a copy to change

	while (index < tokens.size())
	{//we need to make sure this is empty by the end
		line = tokens.at(index).line();
		while (mytokens.at(index).type() != EOL)
		{
			if (mytokens.at(index).value() == "main:")
			{
				labels.push_back("main:");
				label_line.push_back(0);
				if (mytokens.at(index + 1).type() == EOL) {
					index+=2;
				}
				else {
					index++;
				}

			}
			if ((mytokens.at(index).type() == STRING_DELIM) && (delimcounter == 0))
			{
				delimcounter++;
				index+=2;
			}
			if (mytokens.at(index).value() == "nop")
			{
				linevect.emplace_back(STRING, line, mynop);
				text_section.push_back(linevect);
				linevect.clear();
				index+=2;
			}
			else if (delimcounter > 0)
			{
				delimcounter = 0;
				break;
			}
			linevect.push_back(mytokens.at(index));
			if (mytokens.at(index).type() == CLOSE_PAREN)
			{
				break;
			}
			index++;
		}
		text_section.push_back(linevect); //push back the whole line
		text_line++;
		data_movement = false; //update every line
		int_arith = false;
		control_instr = false;
		found_label = false;
		two_reg = false;
		while (lineindex < linevect.size())
		{
			if (linevect.at(0).value().find(":") != string::npos)
			{
				labels.push_back(linevect.at(0).value());//we need to keep track of the index that it is putting it in for the label line vector
				label_line.push_back(linevect.at(0).line());
				break;
			}
			if (lineindex == 0)
			{//update this once to classify the current line

					if (linevect.at(lineindex).value() == "move" || linevect.at(lineindex).value() == "lw" || linevect.at(lineindex).value() == "lh" || linevect.at(lineindex).value() == "lb" || linevect.at(lineindex).value() == "la" || linevect.at(lineindex).value() == "sw" 
						|| linevect.at(lineindex).value() == "lh" || linevect.at(lineindex).value() == "lb" || linevect.at(lineindex).value() == "li" || linevect.at(lineindex).value() == "lw" || 
						linevect.at(lineindex).value() == "sw" || linevect.at(lineindex).value() == "sh" || linevect.at(lineindex).value() == "sb" || linevect.at(lineindex).value() == "mflo" || linevect.at(lineindex).value() == "mfhi") {
						data_movement = true;
					}
					else if (linevect.at(lineindex).value() == "add" || linevect.at(lineindex).value() == "addu" || linevect.at(lineindex).value() == "sub" || linevect.at(lineindex).value() == "subu" ||
						linevect.at(lineindex).value() == "mul" || linevect.at(lineindex).value() == "mulo" || linevect.at(lineindex).value() == "mulou" || linevect.at(lineindex).value() == "rem" || linevect.at(lineindex).value() == "remu" ||
						linevect.at(lineindex).value() == "and" || linevect.at(lineindex).value() == "nor" || linevect.at(lineindex).value() == "or" || linevect.at(lineindex).value() == "xor") {
						int_arith = true;
					}
					else if (linevect.at(lineindex).value() == "beq" || linevect.at(lineindex).value() == "bne" || linevect.at(lineindex).value() == "blt" || linevect.at(lineindex).value() == "ble" || linevect.at(lineindex).value() == "bgt" || linevect.at(lineindex).value() == "bge") {
						control_instr = true;
					}
					else if (linevect.at(lineindex).value() == "mult" || linevect.at(lineindex).value() == "multu" || linevect.at(lineindex).value() == "div" || linevect.at(lineindex).value() == "divu" ||
						linevect.at(lineindex).value() == "abs" || linevect.at(lineindex).value() == "neg" || linevect.at(lineindex).value() == "negu" || linevect.at(lineindex).value() == "not"){
						two_reg = true;
					}
					else if (linevect.at(lineindex).value() != "j") {
						temporary_line = (char)linevect.at(0).line() + '0';
						error_message = ": instruction not found\n";
						print_error = error_beg + temporary_line + error_message;
						//cerr << print_error;
						ret = false;
					}
				
				lineindex++;
			}
			if (lineindex == 1)
			{//check the first index for the register, unless it is a jump
				if (linevect.at(0).value() != "j")
				{
					if (RegTable(linevect.at(1).value()) == -1)
					{
						bad_reg = false;
					}
				}
				else
				{//is a jump
					
					for (size_t i = 0; i < labels.size(); i++)
					{
						if (labels.at(i).find(linevect.at(lineindex).value()) != string::npos)
						{//lineindex should just be 1 so this is @2
							found_label = true;
						}
					}
				}
				lineindex++;
			}
			if (two_reg)
			{
				if (linevect.at(lineindex).type() == SEP)
				{
					lineindex++;
					if (linevect.size() > 4 && (linevect.at(0).value() == "div" || linevect.at(0).value() == "divu"))
					{
						break;
					}
					if (RegTable(linevect.at(lineindex).value()) == -1)
					{//check the register
						for (size_t i = 0; i < varName.size(); i++)
						{
							if (varName.at(i).find(linevect.at(lineindex).value()) != string::npos)
							{//lineindex should just be 1 so this is @2
								found_label = true;
							}
						}
						if (!found_label) {
							//bad_reg = false;
							break;
						}
						
					}
				}
				else
				{
					temporary_line = (char)line + '0';
					error_message = ": syntax incorrect for this line\n";
					print_error = error_beg + temporary_line + error_message;
					//cerr << print_error;
					ret = false;
					break;
				}
			}
			if (data_movement)
			{
				if (linevect.at(0).value() == "mflo" || linevect.at(0).value() == "mfhi")
				{
					if (linevect.size() < 4)
					{
						break;
					}
				}
				if (linevect.at(lineindex).type() == SEP)
				{
					lineindex++;
				}
				else
				{
					temporary_line = (char)line + '0';
					error_message = ": syntax incorrect for this line\n";
					print_error = error_beg + temporary_line + error_message;
					//cerr << print_error;
					ret = false;
					break;
				}
				if (linevect.at(0).value() == "li") {
					if (CanStoi(linevect.at(lineindex).value())) {//this one is correct
						break;
					}
					found_label = false;
					for (size_t i = 0; i < varName.size(); i++)
					{
						if (varName.at(i).find(linevect.at(lineindex).value()) != string::npos)
						{//lineindex should just be 1 so this is @2
							found_label = true;
						}
					}
					if(!found_label) {
						temporary_line = (char)line + '0';
						error_message = ": syntax incorrect for this line\n";
						print_error = error_beg + temporary_line + error_message;
						//cerr << print_error;
						ret = false;
						break;
					}
				} 
				else if (RegTable(linevect.at(lineindex).value()) == -1) {
					//parse if label
					for (size_t i = 0; i < varName.size(); i++)
					{
						if (linevect.at(lineindex).type() == STRING)
						{
							if (varName.at(i).find(linevect.at(lineindex).value()) != string::npos)
							{
								found_label = true;
								//correct_index = i; not initialized, but for errors this could be helpful
							}
						}
					}
					if (!found_label && linevect.size() > 4)
					{//is not a label
						if (linevect.at(lineindex).type() == OPEN_PAREN) { //just a paren and reg
							if (RegTable(linevect.at(lineindex + 1).value()) != -1) {
								break;
							}
							else {
								temporary_line = (char)line + '0';
								error_message = ": syntax incorrect for this line\n";
								print_error = error_beg + temporary_line + error_message;
								//cerr << print_error;
								ret = false;
								break;
							}
						}
						else if (CanStoi(linevect.at(lineindex).value()) && (linevect.at(lineindex + 1).type() == OPEN_PAREN))
						{//offset and paren and reg
							if (RegTable(linevect.at(lineindex + 2).value()) != -1) {
								break;
							}
							found_label = false;
							for (size_t i = 0; i < varName.size(); i++)
								{
									if (varName.at(i).find(linevect.at(lineindex + 2).value()) != string::npos)
									{//lineindex should just be 1 so this is @2
										found_label = true;
									}
								}
							if(!found_label)
							{
								temporary_line = (char)line + '0';
								error_message = ": syntax incorrect for this line\n";
								print_error = error_beg + temporary_line + error_message;
								//cerr << print_error;
								ret = false;
								break;
							}
							else
							{
								break;
							}
						}
					}
				}
				
					
			}
			else if (int_arith) {

				while (lineindex < linevect.size())
				{
					ret = false;
					if (linevect.at(lineindex).type() == SEP) { //should be the case to start with
						lineindex++;

						if (RegTable(linevect.at(lineindex).value()) != -1 && linevect.size() > 4) {
							lineindex++;

							if (linevect.at(lineindex).type() == SEP) {
								lineindex++;

								if (RegTable(linevect.at(lineindex).value()) == -1) { //must be immediate

									if (linevect.at(lineindex).value().find("$") != string::npos) {
										temporary_line = (char)line + '0';
										error_message = ": syntax incorrect for this line\n";
										print_error = error_beg + temporary_line + error_message;
										//cerr << print_error;
										big_break = true;
										break;
									}
									ret = true;
									break;
								}
								else
								{
									ret = true;
									break;
								}
							}
						}
					}
					else {
						big_break = true;
						break;
					}

				}
			} 
			else if (control_instr) {
				if (linevect.at(lineindex).type() == SEP)
				{
					lineindex++;
				}
				else
				{
					ret = false;
					break;
				}
				if (linevect.at(lineindex - 2).value() != "j")
				{//should be SOURCE then LABEL
					if (!CanStoi(linevect.at(lineindex).value()))
					{
						if (RegTable(linevect.at(lineindex).value()) == -1)
						{
							ret = false;
							break;
						}
						lineindex++; //now we are on labels
					}
					if (linevect.at(lineindex).type() == SEP){
						lineindex++;
					}
					else{
						ret = false;
						break;
					}
					for (size_t i = 0; i < labels.size(); i++)
					{
						if (labels.at(i).find(linevect.at(lineindex).value()) != string::npos)
						{//lineindex should just be 1 so this is @2
							found_label = true;
						}
					}
					if (!found_label)
					{//cannot find label, must have a predefined label
						//ret = false;
					}
				}
			}
			lineindex++; //temp until I finish the rest of it FINISH MEMREF
			if(!bad_reg)
			{
				ret = false;
				break;
			}
		}
		if (big_break)
		{
			big_break = false;
			break;
		}
		index++;
		linevect.clear();
		lineindex = 0;
	}

	return ret;
}
int parser::RegTable(std::string registers)
{
	//takes the string for the operation and returns the register number
	//IF THE RETURN VALUE IS -1, THEN THE REGISTER IS NOT A VALID REGISTER
	int i = 1;
	int ret = -1;
	if (registers.size() < 2)
	{
		return -1;
	}
	if (registers[i] == 'z')
	{
		ret = 0;
	}
	else if (registers[i] == '0')
	{
		ret = 0;
	}
	else if ((registers[i] == 'a') && (registers[i + 1] == 't'))
	{
		ret = 1;
	}
	else if (registers[i] == 'v')
	{
		if (registers[i + 1] == '0')
		{
			ret = 2;
		}
		else if(registers[i + 1] == '1')
		{
			ret = 3;
		}
	}
	else if (registers[i] == 'a')
	{
		if (registers[i + 1] == '0')
		{
			ret = 4;
		}
		else if (registers[i + 1] == '1')
		{
			ret = 5;
		}
		else if (registers[i + 1] == '2')
		{
			ret = 6;
		}
		else if (registers[i + 1] == '3')
		{
			ret = 7;
		}
	}
	else if (registers[i] == 't')
	{
		if (registers[i + 1] == '0')
		{
			ret = 8;
		}
		else if (registers[i + 1] == '1')
		{
			ret = 9;
		}
		else if (registers[i + 1] == '2')
		{
			ret = 10;
		}
		else if (registers[i + 1] == '3')
		{
			ret = 11;
		}
		else if (registers[i + 1] == '4')
		{
			ret = 12;
		}
		else if (registers[i + 1] == '5')
		{
			ret = 13;
		}
		else if (registers[i + 1] == '6')
		{
			ret = 14;
		}
		else if (registers[i + 1] == '7')
		{
			ret = 15;
		}
		else if (registers[i + 1] == '8')
		{
			ret = 24;
		}
		else if(registers[i + 1] == '9')
		{
			ret = 25;
		}
	}
	else if ((registers[i] == 's') && (registers[i + 1] != 'p'))
	{
		if (registers[i + 1] == '0')
		{
			ret = 16;
		}
		else if (registers[i + 1] == '1')
		{
			ret = 17;
		}
		else if (registers[i + 1] == '2')
		{
			ret = 18;
		}
		else if (registers[i + 1] == '3')
		{
			ret = 19;
		}
		else if (registers[i + 1] == '4')
		{
			ret = 20;
		}
		else if (registers[i + 1] == '5')
		{
			ret = 21;
		}
		else if (registers[i + 1] == '6')
		{
			ret = 22;
		}
		else if (registers[i + 1] == '7')
		{
			ret = 23;
		}
	}
	else if (registers[i] == 'k')
	{
		if (registers[i + 1] == '0')
		{
			ret = 26;
		}
		else if(registers[i + 1] == '1')
		{
			ret = 27;
		}
	}
	else if (registers[i] == 'g')
	{
		if (registers[i + 1] == 'p')
		{
			ret = 28;
		}
	}
	else if ((registers[i] == 's') && (registers[i + 1] == 'p'))
	{
		ret = 29;
	}
	else if ((registers[i] == 'f') && (registers[i + 1] == 'p'))
	{
		ret = 30;
	}
	else if ((registers[i] == 'r') && (registers[i + 1] == 'a'))
	{
		ret = 31;
	}
	string teststring;
	if (ret == -1)
	{//we need to check if the number in the input string is less than 32
		teststring.push_back(registers[i]);
		teststring.push_back(registers[i + 1]);
		if (CanStoi(teststring))
		{
			int temp = stoi(teststring);
			if (temp < 32 && temp >= 0)
			{
				ret = temp;
			}
		}
	}
	return ret;
}
bool parser::CheckWord(double possibleword, bool sign)
{
	double check = 4294967295;
	if (sign)
	{
		check = 2147483647;
	}
	double temp = possibleword;
	return abs(temp) <= check;
}
bool parser::CheckHalf(double possiblehalf, bool sign)
{
	double check = 65535;
	if (sign)
	{
		check = 32767;
	}
	double temp = possiblehalf;
	return abs(temp) <= check;
}
bool parser::CheckByte(double possiblebyte, bool sign)
{
	double check = 255;
	if (sign)
	{
		check = 127;
	}
	double temp = possiblebyte;
	return abs(temp) <= check;
}
bool parser::CheckSpace(double possiblespace, bool sign)
{
	double check = 4294967295;
	if (sign)
	{
		check = 2147483647;
	}
	double temp = possiblespace;
	return abs(temp) <= check;
}
bool parser::CanStoi(string in)
{
	try {
		stol(in);
	}
	catch (invalid_argument) {
		return false;
	}
	return true;
}
package parser::getpackage()
{
	package mypack;
	all_lines temp;
	label_line.clear();
	label_line.push_back(0);
	int subcounter = -1;
	for (size_t i = 0; i < text_section.size(); i++) {
		if (text_section[i].size() == 0) {
			subcounter++;
		}
		if (text_section[i].size() > 1) {
			temp.push_back(text_section[i]);
		}
		else if (text_section[i].size() == 1) {
			subcounter++;
			if (text_section[i][0].value().find(":") != string::npos && i != 0) {
				label_line.push_back(i - subcounter);
			}
			if (text_section[i][0].value().find("nop") != string::npos) {
				temp.push_back(text_section[i]);
				subcounter--;
			}
		}	
	}
	mypack.instructions = temp;
	mypack.labels = labels;
	mypack.label_lines = label_line;
	mypack.varType = varType;
	mypack.varValue = varValue;
	mypack.varName = varName;
	mypack.parse_line = text_line;
	mypack.text_start = text_start;

	return mypack;
}
#include "lexer.hpp"
#include <stdio.h>
using namespace std;
TokenList tokenize(istream & ins)
{
	TokenList myTokens;
	char c;
	string tempstr;
	string debuggingstring;
	bool inparen = false;
	int openParenCounter = 0;
	int parenLine = 0;
	bool inquote = false;
	int quoteLine;
	bool text = false;
	char message[50];
	tempstr.clear();
	int currentLine = 1;
	while (ins.get(c))
	{
		if (c == '\t')
		{
			c = ' ';
		}
		switch (c) {
		case '\n':
			if (tempstr == ".text") {
				text = true;
			}
			if (!tempstr.empty())
			{//we should push a string even if the tempstring is empty
				myTokens.emplace_back(STRING, currentLine, tempstr);
				myTokens.emplace_back(EOL, currentLine);
			}
			else if (myTokens.size() > 0 && text){
				myTokens.emplace_back(EOL, currentLine);
			}

			currentLine++;
			tempstr.clear();
			if (inparen)
			{
				tempstr.clear();
				sprintf(message, "Unmatched paren on line %2d", parenLine);
				for (int i = 0; i < 30; i++)
				{
					tempstr.push_back(message[i]);
				}
				myTokens.emplace_back(ERROR, currentLine, tempstr);
				break;
			}
			break;
		case '\t':
			break;
		case '\r':
			break;
		case ',':
			//this one isn't too complicated
			if (!tempstr.empty())
			{ //tempstr is not empty, store the string
				myTokens.emplace_back(STRING, currentLine, tempstr);
				myTokens.emplace_back(SEP, currentLine);
				tempstr.clear();
			}
			else
			{
				myTokens.emplace_back(SEP, currentLine);
			}
			break;
		case '(':
			//a little bit of weird coding here
			if (inquote)
			{
				tempstr.push_back(c);
				debuggingstring.push_back(c);
				break;
			}

			if (tempstr.empty())
			{
				myTokens.emplace_back(OPEN_PAREN, currentLine);
			}
			else
			{
				myTokens.emplace_back(STRING, currentLine, tempstr);
				myTokens.emplace_back(OPEN_PAREN, currentLine);
				tempstr.clear();
			}
			openParenCounter++;
			inparen = true;
			parenLine = currentLine;
			break;
		case '"':

			if (inquote)
			{
				myTokens.emplace_back(STRING, currentLine, tempstr);

				inquote = false;
				tempstr.clear();
			}
			else
			{
				inquote = true;
			}
			myTokens.emplace_back(STRING_DELIM, currentLine);
			quoteLine = currentLine;
			break;

		case ')':
			openParenCounter--;
			if (inquote)
			{
				tempstr.push_back(c);
				debuggingstring.push_back(c);
				break;
			}
			if (tempstr.empty())
			{
				myTokens.emplace_back(CLOSE_PAREN, currentLine);
			}
			else
			{
				myTokens.emplace_back(STRING, currentLine, tempstr);
				myTokens.emplace_back(CLOSE_PAREN, currentLine);
				tempstr.clear();
			}
			if (inparen)
			{
				inparen = false;
			}
			else
			{
				inparen = true;
			}


			break;
		case '=':
			myTokens.emplace_back(EQUAL, currentLine);
			break;
		case ' ':
			if (!inquote)
			{
				if (!tempstr.empty())
				{
					myTokens.emplace_back(STRING, currentLine, tempstr);
					tempstr.clear();
				}
			}
			else
			{
				tempstr.push_back(c);
				debuggingstring.push_back(c);
			}
			break;
		default:
			tempstr.push_back(c);
			debuggingstring.push_back(c);
			break;
		}
	}
	if (!myTokens.empty())
	{
		if (!tempstr.empty())
		{
			myTokens.emplace_back(STRING, currentLine, tempstr);
			if (myTokens.back().type() != EOL)
			{
				myTokens.emplace_back(EOL, currentLine);
			}
		}
	}
	//a few final changes if we need them to the end of the list
	if (inquote)
	{
		if (quoteLine != currentLine)
		{
			tempstr.clear();
			sprintf(message, "Error: Unmatched quote on line %2d", quoteLine);
			for (int i = 0; i < 30; i++)
			{
				tempstr.push_back(message[i]);
			}
			myTokens.emplace_back(ERROR, currentLine, tempstr);
		}
	}
	if (inparen || (openParenCounter != 0))
	{
		if (parenLine != currentLine)
		{
			tempstr.clear();
			sprintf(message, "Error: Unmatched paren on line %2d", parenLine);
			for (int i = 0; i < 30; i++)
			{
				tempstr.push_back(message[i]);
			}
			myTokens.emplace_back(ERROR, currentLine, tempstr);
		}
	}
	return myTokens;
}

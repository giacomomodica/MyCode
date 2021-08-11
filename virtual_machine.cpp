#include "virtual_machine.hpp"
#include "parser.hpp"
#include <vector>
#include <future>
#include <thread>
#include <chrono>
#include <cmath>
#include <iterator>
#include <iostream>
#include <sstream>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;

virtual_machine::virtual_machine() {
	
	queue_checker q1(&vm_mq, this);
	Worker w1(this);

	queue_checker_thread = std::thread(q1);
	worker_thread = std::thread(w1);
	
	queue_checker_thread.detach();
	worker_thread.detach();
}
virtual_machine::~virtual_machine() {
	EXIT_CODE = true;
	vm_mq.push("exit");
	std::chrono::duration<double, std::milli> x = std::chrono::milliseconds(50);
	std::this_thread::sleep_for(x);

}
void virtual_machine::initialize(package mypack) {

	pc = 0;
	hi = 0;
	lo = 0;
	
	labels = mypack.labels;
	varType = mypack.varType;
	varValue = mypack.varValue;
	varName = mypack.varName;
	label_lines = mypack.label_lines;
	parse_line = mypack.parse_line;
	text_start = mypack.text_start;
	tokenvect blank_token;
	for (size_t i = 0; i < 32; i++)	{
		all_reg.push_back(0);
	}
	for (size_t i = 0; i < 512; i++)	{
		mem.push_back(0);
	}
	instr.push_back(blank_token);
	for (size_t i = 0; i < mypack.instructions.size(); i++)
	{
		instr.push_back(mypack.instructions[i]);
	}
	initialize_mem();
}
void virtual_machine::initialize_mem() {

	size_t typecounter = 0, namecounter = 0, valcounter = 0;
	int normtemp = 0;
	int_32 temp32 = 0;
	int_8 temp8 = 0;
	for (size_t type = 0; type < varType.size(); type++) {
		typecounter++;
		

		if (varType[type] != ".space" && varType[type] != ".ascii" && varType[type] != ".asciiz") { //space will offset things a little bit
			namecounter++;
			memory_indicies.push_back(mem_index);
			if (CanStoi(varValue.at(valcounter))) {
				temp32 = stol(varValue.at(valcounter));
			}//load the number into the temp int_32 variable
			
			//mask the back 8 bits for the temp8 var
			for (size_t i = 0; i < 8; i++) {
				temp8[i] = temp32[i];
			}
			mem.at(mem_index) = temp8;
			mem_index++;

			if (varType[type] == ".word") {//allocate 32 bits of space, mem_index++
				for (size_t i = 8; i < 16; i++) {
					temp8[normtemp] = temp32[i];
					normtemp++;
				}
				normtemp = 0;
				mem.at(mem_index) = temp8;
				mem_index++;

				for (size_t i = 16; i < 24; i++) {
					temp8[normtemp] = temp32[i];
					normtemp++;
				}
				normtemp = 0;
				mem.at(mem_index) = temp8;
				mem_index++;

				for (size_t i = 24; i < 32; i++) {
					temp8[normtemp] = temp32[i];
					normtemp++;
				}
				normtemp = 0;
				mem.at(mem_index) = temp8;
				mem_index++;
			}
			else if (varType[type] == ".half") {
				for (size_t i = 8; i < 16; i++) {
					temp8[normtemp] = temp32[i];
					normtemp++;
				}
				normtemp = 0;
				mem.at(mem_index) = temp8;
				mem_index++;
			}
			
		}
		else if (varType[type] == ".space") {
			//DON'T PUSH BACK IF IT ISN'T A MEM TYPE
				space_names.push_back(varName[valcounter]);
				space_values.push_back(mem_index);
				memory_indicies.push_back(-1);
				if (CanStoi(varValue[valcounter])) {
					mem_index += stol(varValue[valcounter]);
				}
				namecounter++;
		}
		else if(varType[type] == ".ascii" || varType[type] == ".asciiz") {//THIS WILL OFFSET IT FOR NOW BUT DOES NOT FILL IT
			for (size_t i = 0; i < varValue[valcounter].size(); i++) {
				mem_index++;
			}
			if (varType[type] == ".asciiz") {
				mem_index++;
			}
		}
		valcounter++;
	}

}
void virtual_machine::step() {
	 if (pc < instr.size() - 1) {
		pc++;
	}
	virtual_machine::simulate();

}//call simulate at the end of step
void virtual_machine::simulate() {

	bool data_move = false, integer_arith = false, control_instruct = false, logical = false;

	if	(instr.at(pc).at(0).value() == "move" || instr.at(pc).at(0).value() == "lw" || instr.at(pc).at(0).value() == "lh" || instr.at(pc).at(0).value() == "la" || instr.at(pc).at(0).value() == "sw" ||
		instr.at(pc).at(0).value() == "mflo" || instr.at(pc).at(0).value() == "mfhi" || instr.at(pc).at(0).value() == "li") {
		data_move = true;
	}
	else if (instr.at(pc).at(0).value() == "add" || instr.at(pc).at(0).value() == "addu" || instr.at(pc).at(0).value() == "sub" || instr.at(pc).at(0).value() == "subu" ||
		instr.at(pc).at(0).value() == "mult" || instr.at(pc).at(0).value() == "multu" || instr.at(pc).at(0).value() == "div" || instr.at(pc).at(0).value() == "divu") {
		integer_arith = true;
	}
	else if (instr.at(pc).at(0).value() == "beq" || instr.at(pc).at(0).value() == "bne" || instr.at(pc).at(0).value() == "blt" || 
		instr.at(pc).at(0).value() == "ble" || instr.at(pc).at(0).value() == "bgt" || instr.at(pc).at(0).value() == "bge" || instr.at(pc).at(0).value() == "j") {
		control_instruct = true;
	}
	else if (instr.at(pc).at(0).value() == "and" || instr.at(pc).at(0).value() == "nor" || instr.at(pc).at(0).value() == "not" ||
		instr.at(pc).at(0).value() == "or" || instr.at(pc).at(0).value() == "xor") {
		logical = true;
	}

	if (data_move) 
	{
		data_movement(instr.at(pc));
	}
	else if (integer_arith) 
	{
		int_arith(instr.at(pc));
	}
	else if (control_instruct) 
	{
		control_instruction(instr.at(pc));
	}
	else if (logical) 
	{
		logic(instr.at(pc));
	}
	if (parse_line == 1) {
		pc--;
	}
}
void virtual_machine::data_movement(tokenvect line) {

	enum datastate { lw, li, la, sw, mov, mfhi, mflo };
	datastate data_state = li;
	string arg, label_arg;
	int_32 mem_val = 0, int32_temp = 0;
	unsigned int memtemp = 0;
	int found_index = 0;
	long arg2 = 0;
	long immediate_temp = 0;
	string caseStr = line.at(0).value(); //for the case statement

	const int dest = RegTable(line.at(1).value()); //this will never change for these instructions

	if (caseStr == "lw") {
		data_state = lw;
	}
	else if (caseStr == "li") {
		data_state = li;
	}
	else if (caseStr == "la") {
		data_state = la;
	}
	else if (caseStr == "sw") {
		data_state = sw;
	}
	else if (caseStr == "move") {
		data_state = mov;
	}
	else if (caseStr == "mfhi") {
		data_state = mfhi;
	}
	else if (caseStr == "mflo") {
		data_state = mflo;
	}
	if (data_state != mfhi && data_state != mflo) {
		arg = line.at(3).value();
	}
	switch (data_state) {
	case li:
		if (CanStoi(line.at(3).value()))
		{
			all_reg[dest] = (int_32)stoi(line.at(3).value());
		}
		else {
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
			all_reg[dest] = arg2;
		}
		break;
	case lw:

		arg = line.at(3).value();
		if (RegTable(arg) != -1) {//is a register
			all_reg[dest] = eightto32bit(mem.at(all_reg[RegTable(arg)].to_ulong()));
		}
		else {//is not a register, so must be immediate/tag/offset
			if (CanStoi(arg)) {//is an immediate
				immediate_temp = stol(line.at(3).value());
			}
			else {//must be a label
				for (size_t i = 0; i < varName.size(); i++) {
					if (arg + ':' == varName[i]) {
						if (i < memory_indicies.size()) {
							memtemp = memory_indicies[i];
							found_index = i;
						}
						//else {
						//	i = memory_indicies.size() - 1;
						//	break;
						//}
					}
				}

				arg = mem.at(memtemp).to_string();
				int32_temp = eightto32bit(mem.at(memtemp));
				if (stol(varValue[found_index]) > 256 || stol(varValue[found_index]) < -256) {
					int32_temp = load_address(memtemp);
				}
				//int32_temp = load_address(memtemp);
				if (arg[0] == '1') {//is negative, fill the dest temp with 1s
					for (size_t i = 0; i < 24; i++) {
						int32_temp[31 - i] = 1;
					}
					all_reg[dest] = int32_temp;
				}
				else {
					all_reg[dest] = int32_temp;
				}
			}
		}
	
	if (line.size() > 4) {
		if (line.at(4).type() == OPEN_PAREN) {
			if (RegTable(line.at(5).value()) != -1) {
				arg2 = RegTable(line.at(5).value());
				int32_temp = immediate_temp + all_reg[arg2].to_ulong();
				arg = mem[int32_temp.to_ulong()].to_string();
				int32_temp = eightto32bit(mem[int32_temp.to_ulong()]);
			}
			else {
				if (CanStoi(line.at(5).value())) {
					arg2 = stol(line.at(5).value());
				}
				else {
					arg = line.at(5).value();
					for (size_t i = 0; i < varName.size(); i++) {
						if (arg + ':' == varName[i]) {
							memtemp = memory_indicies[i];
						}
					}
				}
				arg2 = memtemp + immediate_temp;
				arg = mem.at(arg2).to_string();
				int32_temp = eightto32bit(mem.at(arg2));
			}
			if (arg[0] == '1') {//is negative, fill the dest temp with 1s
				for (size_t i = 0; i < 24; i++) {
					int32_temp[31 - i] = 1;
				}
				all_reg[dest] = int32_temp;
			}
			else {//is positive
				all_reg[dest] = mem[int32_temp.to_ulong()].to_ulong();
			}
		}
	}
	
	break;

	case la:
		
		if (RegTable(arg) == -1) {
			for (size_t i = 0; i < varName.size(); i++) {
				if (varName[i] != "XX") {
					if (arg + ':' == varName[i]) {
						memtemp = memory_indicies[i];
					}
				}
			}
			for (size_t i = 0; i < space_names.size(); i++) {
				if (arg + ':' == space_names[i]) {
					memtemp = space_values[i];
				}
			}
		}
		
		mem_val = memtemp;
		all_reg[dest] = memtemp;
		break;
	case sw:

		arg = line.at(3).value();
		if (RegTable(arg) != -1) {//is a register
			store_mem(all_reg[dest], all_reg[RegTable(arg)].to_ulong());
			//transfer the register to the memory location
		}
		else {//is not a register, so must be immediate/tag/offset
			if (CanStoi(arg)) {//is an immediate
				immediate_temp = stol(line.at(3).value());
				arg2 = immediate_temp;
				if (!(line.size() > 4)) {
					store_mem(all_reg[dest], immediate_temp);
				}
			}
			else {//must be a label
				for (size_t i = 0; i < varName.size(); i++) {
					if (arg + ':' == varName[i]) {
						memtemp = memory_indicies[i];
					}
				}
				for (size_t i = 0; i < space_names.size(); i++) {
					if (arg + ':' == space_names[i]) {
						memtemp = space_values[i];
					}
				}
				if (arg[0] == '1') {//is negative, fill the dest temp with 1s
					for (size_t i = 0; i < 24; i++) {
						int32_temp[31 - i] = 1;
					}
				} 
				store_mem(all_reg[dest], memtemp);
			}
		}

		if (line.size() > 4) {
			if (line.at(4).type() == OPEN_PAREN) {
				if (RegTable(line.at(5).value()) != -1) {//register in parenthesis
					int32_temp = all_reg[RegTable(line.at(5).value())];
					
				}
				else { //immediate or variable in parenthesis
					if (CanStoi(line.at(5).value())) {
						arg2 = stol(line.at(5).value());
					}
					else {
						arg = line.at(5).value();
						for (size_t i = 0; i < varName.size(); i++) {
							if (arg + ':' == varName[i]) {
								memtemp = memory_indicies[i];
							}
						}
						for (size_t i = 0; i < space_names.size(); i++) {
							if (arg + ':' == space_names[i]) {
								memtemp = space_values[i];
							}
						}
					}
					arg2 = memtemp + immediate_temp;
				}
				if (arg[0] == '1') {//is negative, fill the dest temp with 1s
					for (size_t i = 0; i < 24; i++) {
						int32_temp[31 - i] = 1;
					}
					store_mem(all_reg[dest], arg2);
				}
				else {//is positive
					store_mem(all_reg[dest], int32_temp.to_ulong() + arg2);
				}
			}
		}

		break;
	case mov:
		all_reg[dest] = all_reg[RegTable(line.at(3).value())];
		break;
	case mfhi:
		all_reg[dest] = hi;
		break;
	case mflo:
		all_reg[dest] = lo;
		break;
	default:
		break;
	}
}
void virtual_machine::int_arith(tokenvect line) {

	enum state { add, addu, sub, subu, mult, multu, div2, div2u };
	state int_arith_state;
	string caseStr = line.at(0).value(); //for the case statement
	unsigned long long_temp = 0;
	bitset<64> bit_temp = 0;
	string arg;
	int_32 op = 0, temp32a, temp32b;
	int dest = RegTable(line.at(1).value()); //this will never change for these instructions
	long arg1 = 0, a, b;
	long arg2 = 0;
	
	if (line.size() > 4) {
		arg2 = RegTable(line.at(5).value());
		arg1 = RegTable(line.at(3).value());
		arg = line.at(5).value();
		if (RegTable(line.at(5).value()) == -1) {
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
		}
	}
	if (caseStr == "add") {
		int_arith_state = add;
	}
	else if (caseStr == "addu") {
		int_arith_state = addu;
	}
	else if (caseStr == "sub") {
		int_arith_state = sub;
	}
	else if (caseStr == "subu") {
		int_arith_state = subu;
	}
	else if (caseStr == "mult") {
		int_arith_state = mult;
	}
	else if (caseStr == "multu") {
		int_arith_state = multu;
	}
	else if (caseStr == "div") {
		int_arith_state = div2;
	}
	else if (caseStr == "divu") {
		int_arith_state = div2u;
	}

	switch (int_arith_state) {
	case add:
		//if (!WillOverflow(all_reg[arg1], all_reg[arg2], "add"))                WILL FIX THIS JUST NOT RN
		//{
			if (RegTable(line.at(5).value()) != -1) {
				long_temp = all_reg[arg1].to_ulong() + all_reg[arg2].to_ulong();
				all_reg[dest] = (int)long_temp;
			}
			else {//must be a label
				if (!CanStoi(line.at(5).value())) {
					for (size_t i = 0; i < varName.size(); i++) {
						if (arg + ':' == varName[i] || arg == varName[i]) {
							arg2 = stol(varValue[i]);
						}
					}
					all_reg[dest] = arg2 + all_reg[arg1].to_ulong();
				}
				else {
					all_reg[dest] = all_reg[arg1].to_ulong() + stol(line.at(5).value());
				}
			}
			
		//}
		//else {
		//	ERROR_FLAG = true;
		//	ERROR_MESSAGE = "Error: overflow occurred on add instruction";
		//}
		
		break;
	case addu:
		if (RegTable(line.at(5).value()) != -1) {
			long_temp = all_reg[arg1].to_ulong() + all_reg[arg2].to_ulong();
			all_reg[dest] = (int)long_temp;
		}
		else {//must be a label
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
			all_reg[dest] = arg2 + all_reg[arg1].to_ulong();
		}
		break;
	case sub:
		//if (!WillOverflow(all_reg[arg1], all_reg[arg2], "sub"))                WILL FIX THIS JUST NOT RN
		//{
		if (RegTable(line.at(5).value()) != -1) {
			long_temp = all_reg[arg1].to_ulong() - all_reg[arg2].to_ulong();
			all_reg[dest] = (int)long_temp;
		}
		else {//must be a label
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
			all_reg[dest] = all_reg[arg1].to_ulong() - arg2;
		}

		//}
		//else {
		//	ERROR_FLAG = true;
		//	ERROR_MESSAGE = "Error: overflow occurred on add instruction";
		//}

		break;
	case subu:
		if (RegTable(line.at(5).value()) != -1) {
			long_temp = all_reg[arg1].to_ulong() - all_reg[arg2].to_ulong();
			all_reg[dest] = (int)long_temp;
		}
		else {//must be a label
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
			all_reg[dest] = all_reg[arg1].to_ulong() - arg2;
		}
		break;
	case mult:
		arg1 = RegTable(line.at(1).value());
		arg2 = RegTable(line.at(3).value());

		long_temp = all_reg[arg1].to_ulong() * all_reg[arg2].to_ulong();
		bit_temp = long_temp;
		if (all_reg[arg1][31] == 1 || all_reg[arg2][31] == 1) {
			for (size_t i = 0; i < 32; i++) {
				bit_temp[63 - i] = 1;
			}
		}
		for (int i = 0; i < 32; i++)
		{
			op[i] = bit_temp[i];
		}
		lo = op;
		op = 0;
		for (int i = 32; i < 64; i++)
		{
			op[i - 32] = bit_temp[i];
		}
		hi = op;
		break;
	case multu:
		arg1 = RegTable(line.at(1).value());
		arg2 = RegTable(line.at(3).value());

		long_temp = all_reg[arg1].to_ulong() * all_reg[arg2].to_ulong();
		bit_temp = long_temp;
		for (int i = 0; i < 16; i++)
		{
			op[i] = bit_temp[i];
		}
		lo = op;
		op = 0;
		for (int i = 15; i < 32; i++)
		{
			op[i] = bit_temp[i];
		}
		hi = op;
		break;
	case div2:
		arg1 = RegTable(line.at(1).value());
		arg2 = RegTable(line.at(3).value());
		temp32a = all_reg[arg1];
		temp32b = all_reg[arg2];
		a = temp32a.to_ulong();
		b = temp32b.to_ulong();
		
		if (all_reg[arg1][31] == 1) {
			temp32a.flip();
			a = temp32a.to_ulong();
			a++;
			a = a * -1;
		}
		if (all_reg[arg2][31] == 1) {
			temp32b.flip();
			b = temp32b.to_ulong();
			b++;
			b = b * -1;
		}
		if (all_reg[arg2] != 0)
		{
			lo = a / b;
			hi = a % b;
		}
		break;
	case div2u:
		arg1 = RegTable(line.at(1).value());
		arg2 = RegTable(line.at(3).value());
		temp32a = all_reg[arg1];
		temp32b = all_reg[arg2];
		a = temp32a.to_ulong();
		b = temp32b.to_ulong();

		if (all_reg[arg1][31] == 1) {
			temp32a.flip();
			a = temp32a.to_ulong();
			a++;
			a = a * -1;
		}
		if (all_reg[arg2][31] == 1) {
			temp32b.flip();
			b = temp32b.to_ulong();
			b++;
			b = b * -1;
		}
		if (all_reg[arg2] != 0)
		{
			lo = a / b;
			hi = a % b;
		}
		break;
	default:
		break;
	}

} 
void virtual_machine::logic(tokenvect line) {

	enum datastate { log_and, log_nor, log_not, log_or, log_xor};
	datastate logic_state = log_and; 
	string arg;
	string caseStr = line.at(0).value(); //for the case statement

	int dest = RegTable(line.at(1).value()); //this will never change for these instructions
	long arg1 = 0;
	long arg2 = 0;
	if (line.size() > 4) {
		arg2 = RegTable(line.at(5).value());
		arg1 = RegTable(line.at(3).value());
		arg = line.at(5).value();
		if (RegTable(line.at(5).value()) == -1) {
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
		}
	}
	else {
		if (line.at(3).value().find("$") != string::npos) {
			arg1 = RegTable(line.at(3).value());
		}
	}

	if (caseStr == "and") {
		logic_state = log_and;
	}
	else if (caseStr == "nor") {
		logic_state = log_nor;
	}
	else if (caseStr == "not") {
		logic_state = log_not;
	}
	else if (caseStr == "or") {
		logic_state = log_or;
	}
	else if (caseStr == "xor") {
		logic_state = log_xor;
	}
	

	switch (logic_state) {
	case log_and:
		if (RegTable(line.at(5).value()) != -1) {
			all_reg[dest]  = all_reg[arg1] & all_reg[arg2];
		}
		else {//must be a label
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
			all_reg[dest] = all_reg[arg1].to_ulong() & arg2;
		}
		break;
	case log_nor:
		if (RegTable(line.at(5).value()) != -1) {
			all_reg[dest] = ~(all_reg[arg1] | all_reg[arg2]);
		}
		else {//must be a label
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
			all_reg[dest] = ~(all_reg[arg1].to_ulong() | arg2);
		}
		break;
	case log_not:
	
		if (RegTable(line.at(3).value()) != -1) {
			all_reg[dest] = ~(all_reg[arg1]);
		}
		else {//must be a label
			arg = line.at(3).value();
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
			all_reg[dest] = ~(arg2);
		}
		break;
	case log_or:
		if (RegTable(line.at(5).value()) != -1) {
			all_reg[dest] = (all_reg[arg1] | all_reg[arg2]);
		}
		else {//must be a label
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
			all_reg[dest] = (all_reg[arg1].to_ulong() | arg2);
		}
		break;
	case log_xor:
		if (RegTable(line.at(5).value()) != -1) {
			all_reg[dest] = all_reg[arg1] ^ all_reg[arg2];
		}
		else {//must be a label
			for (size_t i = 0; i < varName.size(); i++) {
				if (arg + ':' == varName[i] || arg == varName[i]) {
					arg2 = stol(varValue[i]);
				}
			}
			all_reg[dest] = (all_reg[arg1].to_ulong() ^ arg2);
		}
		break;
	default:
		break;
	}

}
void virtual_machine::control_instruction(tokenvect line) {
	enum control_state { jump, branch_eq, branch_ne, branch_lt, branch_le, branch_gt, branch_ge };
	control_state contr_state = jump;

	string caseStr = line.at(0).value(); //for the case statement
	
	string myLabel;
	int arg1 = 0;
	int arg2 = 0;

	if (line[0].value() != "j") {
		arg1 = RegTable(line[1].value());
		arg2 = RegTable(line[3].value());
		myLabel = line[5].value();
	} else {
		myLabel = line[1].value();
	}
	long a = all_reg[arg1].to_ulong(), b = all_reg[arg2].to_ulong();
	if (caseStr == "j") {
		contr_state = jump;
	}
	else if (caseStr == "beq") {
		contr_state = branch_eq;
	}
	else if (caseStr == "bne") {
		contr_state = branch_ne;
	}
	else if (caseStr == "blt") {
		contr_state = branch_lt;
	}
	else if (caseStr == "ble") {
		contr_state = branch_le;
	}
	else if (caseStr == "bgt") {
		contr_state = branch_gt;
	}
	else if (caseStr == "bge") {
		contr_state = branch_ge;
	}

	switch (contr_state) {
	case jump:

		for (size_t i = 0; i < labels.size(); i++) {
			if (myLabel + ':' == labels[i]) {
				if (pc > 1) {
					pc = label_lines[i];
				}
				else {
					pc = label_lines[i] + 1;
				}
			}
		}

		break;
	case branch_eq:

		if (all_reg[arg1] == all_reg[arg2]) {
			for (size_t i = 0; i < labels.size(); i++) {
				if (myLabel + ':' == labels[i]) {
					if (pc > 1) {
						pc = label_lines[i];
					}
					else {
						pc = label_lines[i] + 1;
					}
				}
			}
		}
		break;
	case branch_ne:
		if (all_reg[arg1] != all_reg[arg2]) {
			for (size_t i = 0; i < labels.size(); i++) {
				if (myLabel + ':' == labels[i]) {
					if (pc > 1) {
						pc = label_lines[i];
					}
					else {
						pc = label_lines[i] + 1;
					}
				}
			}
		}
		break;
	case branch_lt:
		if (a < b) {
			for (size_t i = 0; i < labels.size(); i++) {
				if (myLabel + ':' == labels[i]) {
					if (pc > 1) {
						pc = label_lines[i];
					}
					else {
						pc = label_lines[i] + 1;
					}
				}
			}
		}
		break;
	case branch_le:
		if (a <= b) {
			for (size_t i = 0; i < labels.size(); i++) {
				if (myLabel + ':' == labels[i]) {
					if (pc > 1) {
						pc = label_lines[i];
					}
					else {
						pc = label_lines[i] + 1;
					}
				}
			}
		}
		break;
	case branch_gt:
		if (a > b) {
			for (size_t i = 0; i < labels.size(); i++) {
				if (myLabel + ':' == labels[i]) {
					if (pc > 1) {
						pc = label_lines[i];
					}
					else {
						pc = label_lines[i] + 1;
					}
				}
			}
		}
		break;
	case branch_ge:
		if (a >= b) {
			for (size_t i = 0; i < labels.size(); i++) {
				if (myLabel + ':' == labels[i]) {
					if (pc > 1) {
						pc = label_lines[i];
					}
					else {
						pc = label_lines[i] + 1;
					}
				}
			}
		}
		break;
	default: 
		break;
	}
}
int_32 virtual_machine::get_reg(string reg) {

	int_32 ret = 0;
	string temp;

	for (size_t i = 6; i < reg.size(); i++) {
		temp.push_back(reg[i]);
	}
	if (reg.find("hi") != string::npos) {
		ret = hi;
	}
	else if (reg.find("lo") != string::npos) {
		ret = lo;
	}
	else if (reg.find("pc") != string::npos){
		ret = pc;
	}
	else {
		if (RegTable(temp) != -1)
		{
			ret = all_reg.at(RegTable(temp));
		}
		else {
			ret = -102534;
		}
	}
	
	return ret;
}
int_8 virtual_machine::get_mem(string mem_in) {
	//print &0x0000000a
	long result = 0;
	string newStr;
	std::stringstream ss;
	for (size_t i = 7; i < mem_in.size(); i++) {
		newStr.push_back(mem_in[i]);
	}
	if (CanStoi(newStr)) {
		long hexValue = stol(newStr,nullptr,16);
		ss << std::hex << hexValue;
		ss >> result;
	}
	

	int_8 ret = 0;
	string temp;
	ret = mem.at(result);

	return ret;
}
int virtual_machine::RegTable(std::string registers)
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
		else if (registers[i + 1] == '1')
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
		else if (registers[i + 1] == '9')
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
		else if (registers[i + 1] == '1')
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
		if (registers[0] != '$') {
			teststring.push_back(registers[i - 1]);
		}
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
bool virtual_machine::CanStoi(string in)
{
	try {
		stoi(in);
	}
	catch (invalid_argument) {
		return false;
	}
	return true;
}
int_32 virtual_machine::signed_unsigned(int_32 potential_sign)
{
	if (potential_sign[31] == 1)
	{//is negative, lets turn it to positive
		potential_sign.flip();
		potential_sign = potential_sign.to_ulong() + 1;
	}
	else
	{
		potential_sign = 0;
	}
	return potential_sign;
}
bool virtual_machine::WillOverflow(int_32 &arg1, int_32 &arg2, string operation)
{//4294967295 is max number for 32 bits
	unsigned long temp = arg1.to_ulong();
	long res = 0;
	bool ret = true;
	if (operation == "add") {
		res = temp + arg2.to_ulong();
		if (res >= 4294967295 || res < -4294967295)
		{
			ret = false;
		}
	}
	else if (operation == "sub") {
		res = (long)temp - (long)arg2.to_ulong();
		if (res >= 4294967295 || res < -4294967295)
		{
			ret = false;
		}
	}
	return ret; 
}
int_32 virtual_machine::load_address(size_t curr_mem_index) {
	int_8 bit_temp = 0;
	int_32 ret = 0; //we need all 32 bits, which is 4 temps added together
	string temp1, temp2, temp3, temp4, tempfinal;

	bit_temp = mem.at(curr_mem_index);
	temp1 = bit_temp.to_string();
	
	bit_temp = mem.at(curr_mem_index + 1);
	temp2 = bit_temp.to_string();

	bit_temp = mem.at(curr_mem_index + 2);
	temp3 = bit_temp.to_string();

	bit_temp = mem.at(curr_mem_index + 3);
	temp4 = bit_temp.to_string();
	tempfinal = temp4 + temp3 + temp2 + temp1;

	ret = stol(tempfinal, nullptr, 2);

	return ret;
}
int_32 virtual_machine::eightto32bit(int_8 in) {
	int_32 ret = 0;
	for (size_t i = 0; i < 8; i++) {
		ret[i] = in[i];
	}
	return ret;
}
void virtual_machine::store_mem(int_32 var, size_t memory_index) {

	int_32 temp32 = var;
	int_32 RegValue = memory_index;
	unsigned long store_var = RegValue.to_ulong();
	int_8 temp8 = 0;
	int normtemp = 0;

	for (size_t i = 0; i < 8; i++) {
		temp8[normtemp] = temp32[i];
		normtemp++;
	}
	normtemp = 0;
	mem.at(store_var) = temp8;
	store_var++;
	for (size_t i = 8; i < 16; i++) {
		temp8[normtemp] = temp32[i];
		normtemp++;
	}

	normtemp = 0;
	mem.at(store_var) = temp8;
	store_var++;

	for (size_t i = 16; i < 24; i++) {
		temp8[normtemp] = temp32[i];
		normtemp++;
	}
	normtemp = 0;
	mem.at(store_var) = temp8;
	store_var++;

	for (size_t i = 24; i < 32; i++) {
		temp8[normtemp] = temp32[i];
		normtemp++;
	}
	normtemp = 0;
	mem.at(store_var) = temp8;
	mem_index++;
}


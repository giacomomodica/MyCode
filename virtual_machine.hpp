#ifndef VIRTUAL_MACHINE_HPP
#define VIRTUAL_MACHINE_HPP

#include "lexer.hpp"
#include <iostream>
#include "token.hpp"
#include "parser.hpp"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <bitset>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

typedef bitset<8> int_8;
typedef vector<int_8> memory;
typedef bitset<32> int_32;
typedef vector<int_32> registers;

class MessageQueue {
public:

	bool empty() const {
		lock_guard<mutex> lock(the_mutex);
		return the_queue.empty();
	}

	void push(const string& value) {
		unique_lock<mutex> lock(the_mutex);
		the_queue.push(value);
		lock.unlock();
		the_condition_variable.notify_one();
	}

	bool try_pop(string &popped_value) {
		lock_guard<mutex> lock(the_mutex);
		if (the_queue.empty()) {
			return false;
		}
		popped_value = the_queue.front();
		the_queue.pop();
		return true;
	}

	void wait_and_pop(string &popped_value) {
		unique_lock<mutex> lock(the_mutex);
		while (the_queue.empty()) {
			the_condition_variable.wait(lock);
		}
		popped_value = the_queue.front();
		the_queue.pop();
	}

private:
	queue<string> the_queue;
	mutable mutex the_mutex;
	condition_variable the_condition_variable;
};

class virtual_machine {

public:

size_t pc;
MessageQueue vm_mq;
bool EXIT_CODE = false;
string ERROR_MESSAGE;
std::thread worker_thread;
std::thread queue_checker_thread;
bool ERROR_FLAG = false;
int_32 hi;
int_32 lo;
mutex glob_mutex;
bool run_now = false;
bool exit_code = false;
registers all_reg;
memory mem;
size_t mem_index = 0;
all_lines instr;
size_t parse_line = 0;
vector<string> labels;
vector<size_t> label_lines;
vector<string> varType;
vector<string> varValue;
vector<string> varName;
vector<size_t> memory_indicies;
vector<string> space_names;
vector<size_t> space_values;

//helper functions
bool CanStoi(string in);
int_32 get_reg(string reg); //passes the register specified back to the shell
int_8 get_mem(string mem_in); //passes the mem_addr specified back to the shell
int RegTable(std::string registers);

/*
	will do pc = pc + 1 and then call simulate. It will also do any functions it needs to
*/
void step(); 


virtual_machine();
~virtual_machine();
void initialize(package mypack);
void initialize_mem();
/*
	will read in the current instructions.at(pc) and preform the given action
	will call helper functions as needed, as stated below simulate 
*/

bool WillOverflow(int_32 &arg1, int_32 &arg2, string operation);
int_32 signed_unsigned(int_32 potential_sign);
void simulate();
void data_movement(tokenvect line);
void int_arith(tokenvect line);
void logic(tokenvect line);
void control_instruction(tokenvect line);
int_32 load_address(size_t curr_mem_index);
int_32 eightto32bit(int_8 in);
void store_mem(int_32 var, size_t memory_index);
int text_start = 0;

};


class Worker {

public:
	Worker(virtual_machine *the_vm) {
		vm = the_vm;
	}

	void operator()() const {
		while (1) {
			while (vm->run_now) {
				vm->step();
			}
			if (vm->EXIT_CODE) {
				break;
			}
		}
		
	}

private:
	virtual_machine * vm;
};

class queue_checker {

public:
	queue_checker(MessageQueue * mqptr, virtual_machine *this_vm) {
		the_mq = mqptr;
		vm = this_vm;
	};

	void operator()() const {		
		while (1) {

			string message;
			the_mq->wait_and_pop(message);
			lock_guard<mutex> guard(vm->glob_mutex);
			if (message == "run") {
				vm->run_now = true;
			}
			else if (message == "stop") {
				vm->run_now = false;
			}
			else if (message == "exit") {
				vm->exit_code = true;
				break;			
			}
		}
	}


private:
	MessageQueue * the_mq;
	virtual_machine *vm;
};


#endif

#include "virtual_machine_gui.hpp"
#include "virtual_machine.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <sstream>
#include "test_config.hpp"

using namespace std;

int QTHandler(int argc, char ** argv);

int main(int argc, char ** argv) {

	(void)argc;
	filebuf in;
	istream myIn(&in);
	bool exit_loop = false;
	TokenList myList;
	virtual_machine vm;
	int_32 pass = 0;
	string filename;
	parser p;
	bitset<8> mem_val;
	bool parsed = true;
	string argument;
	bool isRunning = false;
	bool reg = false;
	if (argc > 2) { //gui time, run this part using the gui
		QTHandler(argc, argv);
		return EXIT_SUCCESS;
	}
	else {
		in.open(argv[1], std::ios::in);
	}
	enum REPL { print_start, accept_input};
	REPL myState = print_start;

	if (!in.is_open())
	{
		cout << "Error: there is no such file";
		return EXIT_FAILURE;
	}
	myList = tokenize(myIn); //should be fine

	parsed = p.ParseMips(myList);
	package mypack = p.getpackage();
	vm.initialize(mypack);
	if (!parsed)
	{
		cout << "Error: file not parsed correctly";
		return EXIT_FAILURE;
	}
	while (1)
	{
		reg = false;
		argument.clear();
		switch (myState) {
		case print_start:
			cout << "simmips> ";
			myState = accept_input;

			break;
		case accept_input:
			getline(cin, argument);

			if (isRunning) {
				if(argument.find("break") != string::npos) {
					isRunning = false;
					vm.vm_mq.push("stop");
				}
				else {
					cout << "Error: Simulation running. Type break to halt.\n";
				}
				myState = print_start;
			}
			else if (argument .find("run") != string::npos) {
				vm.vm_mq.push("run");
				isRunning = true;
				myState = print_start;
			}
			else if (argument.find("print") != string::npos)
			{
				if (argument.find("&") != string::npos) {
					mem_val = vm.get_mem(argument);
				}
				else if (argument.find("$") != string::npos) {
					pass = vm.get_reg(argument);
					if (pass == -102534) {
						cout << "Error: register not defined\n";
						myState = print_start;
						break;
					}
					reg = true;
				}

				std::stringstream ss;
				if (reg) {
					ss << std::hex << pass.to_ulong(); // int decimal_value
				}
				else {
					ss << std::hex << mem_val.to_ulong(); // int decimal_value
				}
				std::string res(ss.str());
				cout << "0x";
				if (reg) {
					for (size_t i = res.size(); i < 8; i++) {
						cout << "0";
					}
				}
				else {
					if (mem_val.to_ulong() < 16) {
						cout << "0";
					}
				}
				cout << res << endl;

				myState = print_start;
			}
			else if (argument.find("step") != string::npos)
			{
				if (!vm.ERROR_FLAG) {
					vm.step();
				}
				std::stringstream ss;
				pass = vm.get_reg("print $pc");
				ss << std::hex << pass.to_ulong(); // int decimal_value
				std::string res2(ss.str());
				cout << "0x";
				for (size_t i = res2.size(); i < 8; i++) {
					cout << "0";
				}
				cout << res2 << endl;
				myState = print_start;
			}
			else if (argument == "quit")
			{
				exit_loop = true;
			}
			else if (argument == "reset")
			{
				vm.initialize(mypack);
				myState = print_start;
			}
			else if (argument == "status") {
				if (vm.ERROR_FLAG) {
					cout << vm.ERROR_MESSAGE << endl;
					myState = print_start;
				}
				else {
					myState = print_start;
				}
			}
			else {
				cout << "Error: unknown command\n";
				myState = print_start;
			}
			break;
		}
		if (exit_loop) {
			break;
			exit_loop = false;
		}
	}
	return EXIT_SUCCESS;
}


int QTHandler(int argc, char ** argv) {

	QApplication app(argc, argv);
	VirtualMachineGUI gui;
	QString path = QString::fromStdString(argv[2]);
	gui.load(path);

	return app.exec();

}

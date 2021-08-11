#include "virtual_machine_gui.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

VirtualMachineGUI::VirtualMachineGUI() {
	
	in_file->setObjectName("text");
	step->setObjectName("step");
	status->setObjectName("status");
	registers->setObjectName("registers");
	memory->setObjectName("memory");
	break_but->setObjectName("break");
	run->setObjectName("run");

	mainWindow = new QMainWindow(this);
	mainWindow->setCentralWidget(CenteralWidget);
	CenteralWidget->setLayout(mainLayout);
	mainLayout->addLayout(secondaryLayout);

	secondaryLayout->addWidget(in_file);
	secondaryLayout->addWidget(registers);
	secondaryLayout->addWidget(memory);

	LineLabel->addWidget(label_status);
	LineLabel->addWidget(status);

	buttons->addWidget(step);
	buttons->addWidget(run);
	buttons->addWidget(break_but);

	step->setText("Step");
	run->setText("Run");
	break_but->setText("Break");
	break_but->setEnabled(false);
	label_status->setText("Status: ");

	mainLayout->addLayout(LineLabel);
	mainLayout->addLayout(buttons);

	status->setText("Ok");

	connect(step, SIGNAL(released()), this, SLOT(handleStep()));
	connect(run, SIGNAL(released()), this, SLOT(handleRun()));
	connect(break_but, SIGNAL(released()), this, SLOT(handleBreak()));
	
	status->setReadOnly(true);
	
	mainWindow->showMaximized();
}

void VirtualMachineGUI::load(QString filename) {

	filebuf in;
	istream myIn(&in);
	TokenList myList;
	parser p;
	bitset<8> mem_val;
	string argument;
	string str_file = filename.toLocal8Bit().constData();

	vm = new virtual_machine;

	file = str_file;

	in.open(str_file, std::ios::in);
	myList = tokenize(myIn); //should be fine
	p.ParseMips(myList);

	package mypack = p.getpackage();
	vm->initialize(mypack); //should be ready to get registers and such

	filepath_glob = filename;
	gui_pc = vm->text_start + 1;
	print_file(filename);
	print_reg();
	print_mem();
	if (from_struct) {
		*glob_struct.instr_size = vm->instr.size() - 1;
	}
	
}

void VirtualMachineGUI::print_file(QString input) {
	in_file->clear();
	bool setting_blank_lines = true;
	QString startTag = "<span style=\"background-color: yellow; white-space: pre\">";
	QString endTag = " </span>";
	QString QFileDir = input;
	QString *temp = new QString;
	QString *temp2 = new QString;
	QFile file(QFileDir);
	file.open(QIODevice::ReadOnly);
	QTextStream stream(&file);
	size_t pc = vm->pc;

	while (temp->toStdString().find("main:") == std::string::npos) {
		temp->clear();
		stream.readLineInto(temp, 100);
		in_file->appendPlainText(*temp);
	}

	size_t i = 0;

	while (i < pc) {
		temp->clear();
		temp2->clear();
		stream.readLineInto(temp, 100);
		if (temp->toStdString().find(":") != string::npos) {
			stream.readLineInto(temp2, 100);
			temp->append(temp2);
		}
		else if (temp->isEmpty()) {
			stream.readLineInto(temp, 100);
		}

		in_file->appendPlainText(*temp);
		i++;
	}
	if (i == pc) {
		
		temp->clear();
		temp2->clear();
		stream.readLineInto(temp2, 100);
		if (temp2->toStdString().find(":") != string::npos) {
			stream.readLineInto(temp, 100);
			temp2->append(temp);
			temp->clear();
		}
		else if (temp2->isEmpty()) {
			stream.readLineInto(temp2, 100);
		}
		temp->push_back(startTag);
		temp->push_back(*temp2);
		temp->push_back(endTag);
		in_file->appendHtml(*temp);
		temp->clear();
	}
	while (!stream.atEnd()) {
		temp->clear();
		temp2->clear();
		stream.readLineInto(temp, 100);
		if (temp->toStdString().find(":") != string::npos) {
			stream.readLineInto(temp2, 100);
			temp->append(temp2);
		}
		else if (temp->isEmpty()) {
			stream.readLineInto(temp, 100);
		}

		in_file->appendPlainText(*temp);
		i++;
	}
	if (setting_blank_lines) {
		file_len = i;
	}

	setting_blank_lines = false;
	file.close();
	QFont font = in_file->font();

	font.setPointSize(13);
	in_file->setFont(font);
	in_file->setFixedHeight(5.2 * h / 6);

	in_file->setReadOnly(true);
}
void VirtualMachineGUI::print_reg() {

	int_32 pass = 0;
	registers->setRowCount(35);
	registers->setColumnCount(3);
	size_t col = 0, row = 3;
	QStringList horizHeaders;
	horizHeaders << "Number" << "Alias" << "Value(Hex)";
	registers->setHorizontalHeaderLabels(horizHeaders);
	registers->setFixedHeight(5.2 * h / 6);
	registers->verticalHeader()->setVisible(false);
	string tempstr = "$", newtemp;
	char x [4];
	size_t i = 0;

	while (i < 35) { //load registers with values
		std::stringstream ss;
		string passStr = "print ";
		if (i <= 31) {
			sprintf(x, "%d", (int)i);
			tempstr.push_back(x[0]);
			tempstr.push_back(x[1]);
		}
		else {
			if (i == 32) {
				tempstr = "$pc";
				col = 1;
				row = 0;
				load_register_names();
			}
			else if (i == 33) {
				tempstr = "$hi";
				row = 1;
			}
			else if (i == 34) {
				tempstr = "$lo";
				row = 2;
			}
		}
		
		registers->setItem(row, col, new QTableWidgetItem(tempstr.c_str()));
		passStr = passStr + tempstr;

		pass = vm->get_reg(passStr);

		ss << std::hex << pass.to_ulong();
		std::string res(ss.str());
		newtemp = res;
		res.clear();

		for (size_t i = newtemp.size(); i < 8; i++) {
			res.push_back('0');
		}
		res = "0x" + res + newtemp;
		if (i <= 31) {
			registers->setItem(row, col + 2, new QTableWidgetItem(res.c_str()));
		}
		else {
			registers->setItem(row, col + 1, new QTableWidgetItem(res.c_str()));
		}

		passStr.clear();
		tempstr.clear();
		tempstr.push_back('$');
		i++;
		row++;
	}

}
void VirtualMachineGUI::print_mem() {

	string newtemp;
	int_8 pass = 0;
	memory->setRowCount(512);
	memory->setColumnCount(2);
	QStringList vertHeaders;
	vertHeaders << "Address(Hex)" << "Value(Hex)";
	memory->setHorizontalHeaderLabels(vertHeaders);
	memory->verticalHeader()->setVisible(false);
	memory->setFixedHeight(5.2 * h / 6);

	size_t i = 0, row = 0, col = 0;
	while (i < 512) {
		string passStr = "print &";
		std::stringstream ss;
		ss << std::hex << i;
		std::string res(ss.str());
		newtemp = res;
		res.clear();
		for (size_t i = newtemp.size(); i < 8; i++) {
			res.push_back('0');
		}
		res = "0x" + res + newtemp;
		memory->setItem(row, col, new QTableWidgetItem(res.c_str()));

		passStr = passStr + res;

		pass = vm->get_mem(passStr);

		std::stringstream ss2;
		ss2 << std::hex << pass.to_ulong();
		string temp3 = ss2.str();

		res.clear();
		if (pass.to_ulong() < 16) {
			res.push_back('0');
		}
		res = "0x" + res + temp3;
		memory->setItem(row, col + 1, new QTableWidgetItem(res.c_str()));
		i++;
		row++;
		newtemp.clear();
		pass = 0;
		passStr.clear();
	}

}
void VirtualMachineGUI::load_register_names(){
	string reg = "$zero";
	registers->setItem(3, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$at";
	registers->setItem(4, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$v0";
	registers->setItem(5, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$v1";
	registers->setItem(6, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$a0";
	registers->setItem(7, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$a1";
	registers->setItem(8, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$a2";
	registers->setItem(9, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$a3";
	registers->setItem(10, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t0";
	registers->setItem(11, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t1";
	registers->setItem(12, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t2";
	registers->setItem(13, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t3";
	registers->setItem(14, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t4";
	registers->setItem(15, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t5";
	registers->setItem(16, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t6";
	registers->setItem(17, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t7";
	registers->setItem(18, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$s0";
	registers->setItem(19, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$s1";
	registers->setItem(20, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$s2";
	registers->setItem(21, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$s3";
	registers->setItem(22, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$s4";
	registers->setItem(23, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$s5";
	registers->setItem(24, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$s6";
	registers->setItem(25, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$s7";
	registers->setItem(26, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t8";
	registers->setItem(27, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$t9";
	registers->setItem(28, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$k0";
	registers->setItem(29, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$k1";
	registers->setItem(30, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$gp";
	registers->setItem(31, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$sp";
	registers->setItem(32, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$fp";
	registers->setItem(33, 1, new QTableWidgetItem(reg.c_str()));
	reg.clear();
	reg = "$ra";
	registers->setItem(34, 1, new QTableWidgetItem(reg.c_str()));

}
void VirtualMachineGUI::handleStep() {
	if (vm->pc <= file_len - 1) {
		vm->step();
		print_reg();
		print_mem();
		status->setText("Ok");

		if (vm->pc != file_len - 1) {
			print_file(filepath_glob);
		}
	}
	else {
		vm->ERROR_FLAG = true;
		vm->ERROR_MESSAGE = "Error: file has ended.";
		status->setText("Error: file has ended.");
	}
}
void VirtualMachineGUI::handleRun() {

	step->setEnabled(false);
	break_but->setEnabled(true);
	run->setEnabled(false);

	vm->vm_mq.push("run");
	

}

void VirtualMachineGUI::handleBreak() {

	vm->vm_mq.push("stop");

	step->setEnabled(true);
	break_but->setEnabled(false);
	run->setEnabled(true);
	print_reg();
	print_mem();
	vm->pc--;
	print_file(filepath_glob);
	vm->pc++;

}

#ifndef VIRTUAL_MACHINE_GUI_HPP
#define VIRTUAL_MACHINE_GUI_HPP

#include "virtual_machine.hpp"
#include <iostream>
#include <parser.hpp>
#include <QString>
#include <QWidget>
#include <QPlainTextEdit>
#include <QLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableView>
#include <QMainWindow>
#include <QTableWidget>
#include <QApplication>
#include <QLineEdit>
#include <QLabel>
#include <QSize>
#include <QTextStream>
#include <QApplication>
#include <QDesktopWidget>

using namespace std;
// TODO define the GUI class

struct pass_to_gui {
	virtual_machine *the_vm;
	size_t *instr_size;
};

class VirtualMachineGUI : public QWidget {
Q_OBJECT
		
public:

	bool run_now;
	bool end_code = false;
	size_t step_counter = 0;
	pass_to_gui glob_struct;
	vector<int> ignore_lines;
	size_t file_len = 0;
	bool from_struct = false;
	virtual_machine *vm;
	bool IGNORE_LINE = false;
	QString filepath_glob;
	size_t gui_pc = 0;
	string file;
	int offset_counter = 0;
	QMainWindow *mainWindow;
	QVBoxLayout *mainLayout = new QVBoxLayout;
	QHBoxLayout *secondaryLayout = new QHBoxLayout;
	QHBoxLayout *LineLabel = new QHBoxLayout;
	QHBoxLayout *buttons = new QHBoxLayout;
	QWidget *CenteralWidget = new QWidget;
	QPlainTextEdit *in_file = new QPlainTextEdit;
	QTableWidget *registers = new QTableWidget;
	QTableWidget *memory = new QTableWidget;
	QLineEdit *status = new QLineEdit;
	QLabel *label_status = new QLabel;
	QPushButton *step = new QPushButton;
	QPushButton *run = new QPushButton;
	QPushButton *break_but = new QPushButton;

	void load(QString filename);
	VirtualMachineGUI();
	QRect r = QApplication::desktop()->screenGeometry();
	int h = r.height();
	int w = r.width();

	void print_file(QString input);
	void load_register_names();
	void print_reg();
	void print_mem();

private slots:
	void handleStep();
	void handleRun();
	void handleBreak();
};


#endif

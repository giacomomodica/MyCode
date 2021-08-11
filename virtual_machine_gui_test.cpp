#include <QTest>
#include <QPlainTextEdit>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <iostream>
#include "virtual_machine_gui.hpp"

#include "test_config.hpp"

class VirtualMachineGUITest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  void testCase1();
  void testCase2();
  
private:

  VirtualMachineGUI widget;
};


// this section just verifies the object names and API
void VirtualMachineGUITest::initTestCase(){

  widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test00.asm"));

  auto textWidget = widget.findChild<QPlainTextEdit *>("text");
  QVERIFY2(textWidget, "Could not find QPLainText widget");

  auto registerViewWidget = widget.findChild<QTableView *>("registers");
  QVERIFY2(registerViewWidget, "Could not find QTableView widget for registers");

  auto memoryViewWidget = widget.findChild<QTableView *>("memory");
  QVERIFY2(memoryViewWidget, "Could not find QTableView widget for memory");

  auto statusViewWidget = widget.findChild<QLineEdit *>("status");
  QVERIFY2(statusViewWidget, "Could not find QLineEdit widget for status");

  auto stepButtonWidget = widget.findChild<QPushButton *>("step");
  QVERIFY2(stepButtonWidget, "Could not find QTableView widget for step");

  auto runButtonWidget = widget.findChild<QPushButton *>("run");
  QVERIFY2(runButtonWidget, "Could not find QTableView widget for ");

  auto breakButtonWidget = widget.findChild<QPushButton *>("break");
  QVERIFY2(breakButtonWidget, "Could not find QTableView widget for break");
}

void VirtualMachineGUITest::testCase1() 
{
	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test01.asm"));
	auto memoryViewWidget = widget.findChild<QTableView *>("memory");
	QVERIFY2(memoryViewWidget, "Could not find QTableView widget for memory");

	auto registerViewWidget = widget.findChild<QTableView *>("registers");
	QVERIFY2(registerViewWidget, "Could not find QTableView widget for registers");

	auto stepButtonWidget = widget.findChild<QPushButton *>("step");
	QVERIFY2(stepButtonWidget, "Could not find QTableView widget for memory");

	int init_pc = widget.vm->pc;
	stepButtonWidget->click();
	int second_pc = widget.vm->pc;
	QVERIFY(init_pc + 1 == second_pc);

	auto runButtonWidget = widget.findChild<QPushButton *>("run");
	QVERIFY2(runButtonWidget, "Could not find QTableView widget for run");

	auto breakButtonWidget = widget.findChild<QPushButton *>("break");
	QVERIFY2(breakButtonWidget, "Could not find QTableView widget for break");
	
	runButtonWidget->click();
	breakButtonWidget->click();
}

void VirtualMachineGUITest::testCase2()
{
	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test02.asm"));
	auto stepButtonWidget = widget.findChild<QPushButton *>("step");
	QVERIFY2(stepButtonWidget, "Could not find QTableView widget for step");

	auto runButtonWidget = widget.findChild<QPushButton *>("run");
	QVERIFY2(runButtonWidget, "Could not find QTableView widget for run");

	auto breakButtonWidget = widget.findChild<QPushButton *>("break");
	QVERIFY2(breakButtonWidget, "Could not find QTableView widget for break");

	stepButtonWidget->click();

	runButtonWidget->click();

	std::chrono::duration<double, std::milli> x = std::chrono::milliseconds(50);
	std::this_thread::sleep_for(x);

	breakButtonWidget->click();

	std::cout << "pc should be at the end of the program, pc = " << widget.vm->pc << endl << endl;
	//QVERIFY(widget.vm->pc == 7);
}

QTEST_MAIN(VirtualMachineGUITest)
#include "virtual_machine_gui_test.moc"

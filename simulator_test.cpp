#include "catch.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "virtual_machine.hpp"
using namespace std;
TEST_CASE("testing helpers", "[helpers]")
{
	{
		virtual_machine vm;
		string teststr0 = "$0";
		string teststr01 = "$1";
		string teststr1 = "$zero";
		string teststr2 = "$at";
		string teststr3 = "$v0";
		string teststr4 = "$v1";
		string teststr5 = "$a0";
		string teststr6 = "$a1";
		string teststr7 = "$a2";
		string teststr8 = "$a3";
		string teststr9 = "$t0";
		string teststr10 = "$t1";
		string teststr11 = "$t2";
		string teststr12 = "$t3";
		string teststr13 = "$t4";
		string teststr14 = "$t5";
		string teststr15 = "$t6";
		string teststr16 = "$t7";
		string teststr17 = "$s0";
		string teststr18 = "$s1";
		string teststr19 = "$s2";
		string teststr20 = "$s3";
		string teststr21 = "$s4";
		string teststr22 = "$s5";
		string teststr23 = "$s6";
		string teststr24 = "$s7";
		string teststr25 = "$t8";
		string teststr26 = "$t9";
		string teststr27 = "$k0";
		string teststr28 = "$k1";
		string teststr29 = "$gp";
		string teststr30 = "$sp";
		string teststr31 = "$fp";
		string teststr32 = "$ra";

		REQUIRE(vm.RegTable(teststr0) == 0);
		REQUIRE(vm.RegTable(teststr1) == 0);
		REQUIRE(vm.RegTable(teststr01) == 1);
		REQUIRE(vm.RegTable(teststr2) == 1);
		REQUIRE(vm.RegTable(teststr3) == 2);
		REQUIRE(vm.RegTable(teststr4) == 3);
		REQUIRE(vm.RegTable(teststr5) == 4);
		REQUIRE(vm.RegTable(teststr6) == 5);
		REQUIRE(vm.RegTable(teststr7) == 6);
		REQUIRE(vm.RegTable(teststr8) == 7);
		REQUIRE(vm.RegTable(teststr9) == 8);
		REQUIRE(vm.RegTable(teststr10) == 9);
		REQUIRE(vm.RegTable(teststr11) == 10);
		REQUIRE(vm.RegTable(teststr12) == 11);
		REQUIRE(vm.RegTable(teststr13) == 12);
		REQUIRE(vm.RegTable(teststr14) == 13);
		REQUIRE(vm.RegTable(teststr15) == 14);
		REQUIRE(vm.RegTable(teststr16) == 15);
		REQUIRE(vm.RegTable(teststr17) == 16);
		REQUIRE(vm.RegTable(teststr18) == 17);
		REQUIRE(vm.RegTable(teststr19) == 18);
		REQUIRE(vm.RegTable(teststr20) == 19);
		REQUIRE(vm.RegTable(teststr21) == 20);
		REQUIRE(vm.RegTable(teststr22) == 21);
		REQUIRE(vm.RegTable(teststr23) == 22);
		REQUIRE(vm.RegTable(teststr24) == 23);
		REQUIRE(vm.RegTable(teststr25) == 24);
		REQUIRE(vm.RegTable(teststr26) == 25);
		REQUIRE(vm.RegTable(teststr27) == 26);
		REQUIRE(vm.RegTable(teststr28) == 27);
		REQUIRE(vm.RegTable(teststr29) == 28);
		REQUIRE(vm.RegTable(teststr30) == 29);
		REQUIRE(vm.RegTable(teststr31) == 30);
		REQUIRE(vm.RegTable(teststr32) == 31);

	}
	{
		virtual_machine vm;
		//need to make this test work
		string teststr3 = "$s9";

		REQUIRE(vm.RegTable(teststr3) == -1);
	}
	{
		int test = -2;
		int test2 = 2;
		int_32 test32 = test;
		int_32 test232 = test2;
		test32.flip();
		long a = test32.to_ulong() + 1;
		a = a * -1;
		long b = test232.to_ulong();

		REQUIRE(b/a == -1);
	}
}
TEST_CASE("testing initial cases", "[initialize]")
{
	{
		parser p;
		virtual_machine vm;
		string teststring = R"(# simple infinite loop
	.data
	.text
main:	j main
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		REQUIRE(p.ParseMips(list));//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		REQUIRE(vm.pc == 0);
		vm.step();
		REQUIRE(vm.pc == 0);

		REQUIRE(vm.get_reg("print $at") == 0);
		int_32 x = vm.get_reg("print $1");
		REQUIRE(vm.get_reg("print $1") == x);
		REQUIRE(vm.get_reg("print $v0") == 0);
	}
	{
		parser p;
		virtual_machine vm;
		string teststring = R"(.data
avar: .word 0, 1, 2, 4
bvar: .half 1, 1, 4, 76
cvar: .byte 3, 6, 23, 3, 62
var6: .ascii "hello"
var7: .word 1

	.text
main:	
	li $t0, 45
	lw $t1, avar
	lh $t2, bvar
	lb $t2, cvar
	sw $t1, avar
	sh $t2, bvar
	sb $t2, cvar

	move $t0, $0)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		REQUIRE(p.ParseMips(list));//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);
		REQUIRE(vm.pc == 0);
		REQUIRE(vm.hi == 0);
		REQUIRE(vm.lo == 0);
		registers reg;
		int temp = 0;
		for (size_t i = 0; i < 32; i++) {
			if (vm.all_reg.at(i) != 0)
			{
				temp = 1;
			}
		}

		REQUIRE(temp == 0);
	}
	
	{
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(.data
	.text
main:	
	li $t0, 45
	move $t2, $t0
	addu $t1, $t0, $t2
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		REQUIRE(vm.all_reg[8] == 45);
		int_32 t0_val = vm.get_reg("print $t0");
		REQUIRE(t0_val == 45);
		vm.step(); //now we should be on the move command
		vm.step(); //now on add

		REQUIRE(vm.all_reg[9].to_ulong() == 90);

	}
	{//test01.asm
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
        .space 8
var1:   .word 1
var2:   .word -2

        .text
main:
     	la $t0, var1

	lw $t1, 0
	lw $t2, $t0
	lw $t3, 4($t0)
	lw $t4, 4(var1)
	lw $t5, var2
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		REQUIRE(vm.get_reg("print $t0") == 0);
		REQUIRE(vm.get_mem("print &0x00000008") == 1);
		REQUIRE(vm.get_mem("print &0x00000009") == 0);
		REQUIRE(vm.get_mem("print &0x0000000a") == 0);
		REQUIRE(vm.get_mem("print &0x0000000b") == 0);
		REQUIRE(vm.get_mem("print &0x0000000c") == 0xfe);
		REQUIRE(vm.get_mem("print &0x0000000d") == 0xff);
		REQUIRE(vm.get_mem("print &0x0000000e") == 0xff);
		REQUIRE(vm.get_mem("print &0x0000000f") == 0xff);

		vm.step();
		REQUIRE(vm.get_reg("print $t0").to_ulong() == 8);
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $t2").to_ulong() == 1);
		vm.step();
		REQUIRE(vm.get_reg("print $t3").to_ulong() == 0xfffffffe);
		vm.step();
		REQUIRE(vm.get_reg("print $t4").to_ulong() == 0xfffffffe);
		vm.step();
		REQUIRE(vm.get_reg("print $t5").to_ulong() == 0xfffffffe);
	}
	{
		//test02.asm
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(       .data
r1:     .space 4
r2:     .space 12
r3:     .space 4
var:    .word 7

        .text
main:
        la $t0, r2
     	lw $t1, var

	sw $t1, 0
	sw $t1, $t0
	sw $t1, 4($t0)
	sw $t1, 8(r2)
	sw $t1, r3
	
	)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		REQUIRE(vm.get_reg("print $t0").to_ulong() == 4);
		vm.step();
		REQUIRE(vm.get_reg("print $t1").to_ulong() == 7);
		vm.step();
		REQUIRE(vm.get_mem("print &0x00000000").to_ulong() == 7);
		REQUIRE(vm.get_mem("print &0x00000001").to_ulong() == 0);
		vm.step();
		REQUIRE(vm.get_mem("print &0x00000004").to_ulong() == 7);
		REQUIRE(vm.get_mem("print &0x00000007").to_ulong() == 0);
		vm.step();
		REQUIRE(vm.get_mem("print &0x00000008").to_ulong() == 7);
		vm.step();
		REQUIRE(vm.get_mem("print &0x0000000c").to_ulong() == 7);
		vm.step();
		REQUIRE(vm.get_mem("print &0x00000010").to_ulong() == 7);

	}
	{
		//test03.asm
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
A = 0
B = 1
C = 2
D = 4
        .text
main:
        li $t0, 100
        li $t1, A
        li $t2, B
        li $t3, C
        li $t4, D
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step(); 
		REQUIRE(vm.get_reg("print $t0").to_ulong() == 100);
		vm.step();
		REQUIRE(vm.get_reg("print $t1").to_ulong() == 0);
		vm.step();
		REQUIRE(vm.get_reg("print $t2").to_ulong() == 1);
	}
	{
		//test05.asm
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
VALUE = -1
var:    .word 1
        .text
main:
        lw $t0, var
        add $t1, $t0, VALUE
        add $t2, $t1, $t0)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		vm.step();
		vm.step();

		REQUIRE(vm.get_reg("print $t0").to_ulong() == 1);
		REQUIRE(vm.get_reg("print $t1").to_ulong() == 0);
		REQUIRE(vm.get_reg("print $t2").to_ulong() == 1);
	}
	{
		//test03.asm
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
VALUE = 12
var:    .word 31
        .text
main:
        lw $t0, var
        addu $t1, $t0, VALUE # 31+12=43
        addu $t2, $t1, $t0 # 43+31=74
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		vm.step();
		vm.step();

		REQUIRE(vm.get_reg("print $t0").to_ulong() == 0x1f);
		REQUIRE(vm.get_reg("print $t1").to_ulong() == 0x2b);
		REQUIRE(vm.get_reg("print $t2").to_ulong() == 0x4a);
	}
	{
		//test07.asm
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
VALUE = 2
var1:   .word 1
var2:   .word 12
var3:   .word -1
        .text
main:
        lw $t0, var1
        lw $t1, var2
        lw $t2, var3
        sub $t3, $t0, VALUE # 1-2 = -1
        sub $t4, $t1, $t0 # 12-1 = 11
        sub $t5, $t2, VALUE # -1-2 = -3
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $t0").to_ulong() == 1);
		REQUIRE(vm.get_reg("print $t1").to_ulong() == 0xc);
		REQUIRE(vm.get_reg("print $t2").to_ulong() == 0xffffffff);
		vm.step();
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $t3").to_ulong() == 0xffffffff);
		REQUIRE(vm.get_reg("print $t4").to_ulong() == 0xb);
		REQUIRE(vm.get_reg("print $t5").to_ulong() == 0xfffffffd);
	}
	{
		//test13.asm
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        and $t2, $t0, $t1   
        and $t2, $t0, VALUE
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $10").to_ulong() == 8);
		vm.step();
		REQUIRE(vm.get_reg("print $10").to_ulong() == 0);
	}
	{//9
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
VALUE = 4
VALUE2 = -4
var1:   .word 2
var2:   .word -2
var3:   .word 1073741824 # = 2^30
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        mult $t0, $t1 # 2*4 = 8
        mflo $t8
        mfhi $t9

        lw $t0, var2
        lw $t1, var1
        mult $t0, $t1 # -2*2 = -4 
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE
        mult $t0, $t1 # 1073741824*4 = 4294967296 (overflow)
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE2
        mult $t0, $t1 # 1073741824*-4 = -4294967296 (overflow)
        mflo $t8
        mfhi $t9
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);
		vm.step();
		vm.step();
		vm.step();
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $lo").to_ulong() == 8);
		REQUIRE(vm.get_reg("print $hi").to_ulong() == 0);


		vm.step();
		vm.step();
		vm.step();
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $t0").to_ulong() == 0xfffffffe);
		REQUIRE(vm.get_reg("print $lo").to_ulong() == 0xfffffffc);
		REQUIRE(vm.get_reg("print $hi").to_ulong() == 0xffffffff);
		vm.step();
		vm.step();
		vm.step();
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $t0").to_ulong() == 0x40000000);
		REQUIRE(vm.get_reg("print $t1").to_ulong() == 0x00000004);
		REQUIRE(vm.get_reg("print $lo").to_ulong() == 0x0);

	}
	{//17
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        not $t2, $t0
        not $t2, $t1
        not $t2, VALUE
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);
		vm.step();
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $10").to_ulong() == 0xfffffff3);
		vm.step();
		REQUIRE(vm.get_reg("print $10").to_ulong() == 0xfffffff5);
		vm.step();
		REQUIRE(vm.get_reg("print $10").to_ulong() == 0xfffffffc);
	}
	{
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
VALUE = 4
VALUE2 = -4
var1:   .word 2
var2:   .word -2
var3:   .word 1073741824 # = 2^30
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        div $t0, $t1 # 2/4 = 0 rem 2
        mflo $t8
        mfhi $t9

        lw $t0, var2
        lw $t1, var1
        div $t0, $t1 # -2/2 = -1 rem 0 
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE
        div $t0, $t1 # 1073741824/4 = 268435456 rem 0
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE2
        div $t0, $t1 # 1073741824/-4 = -268435456 rem 0 
        mflo $t8
        mfhi $t9

        # divide by 0
        div $t0, $0
	)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		vm.step();
		vm.step();
		vm.step();
		vm.step();

		REQUIRE(vm.get_reg("print $t0").to_ulong() == 2);
		REQUIRE(vm.get_reg("print $t1").to_ulong() == 4);
		REQUIRE(vm.get_reg("print $hi").to_ulong() == 2);

		vm.step();
		vm.step();
		vm.step();
		vm.step();
		vm.step();

		REQUIRE(vm.get_reg("print $lo").to_ulong() == 0xffffffff);
		vm.step();
		vm.step();
		vm.step();
		vm.step();
		vm.step();

		vm.step();
		vm.step();
		vm.step();
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $t0").to_ulong() == 0x40000000);
		REQUIRE(vm.get_reg("print $t1").to_ulong() == 0xfffffffc);
		REQUIRE(vm.get_reg("print $24").to_ulong() == 0xf0000000);

	}
}

TEST_CASE("line management tests", "[error]") {

	{//can't get this to work :(
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(.data
	.text
main:	
	li $s1, 4
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list
		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		vm.step();
		vm.step(); //pc should stay at 0
		REQUIRE(vm.pc == 0);

		//also we can try one more thing
		vm.mem.at(0) = -5;
		REQUIRE(vm.get_mem("print &0x00000000").to_string() == "11111011");
	}

}
TEST_CASE("Overflow Tests", "[error]") {
	
	{//can't get this to work :(
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(.data
	.text
main:	
	li $t0, 2147483
	li $t2, 4294967
	add $t3, $t0, $t2
	li $s1, 4
	li $s1, 4
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list
		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		vm.step();
		vm.step(); //we should be past the add now


	}

}
TEST_CASE("Control Instructions", "[test scripts]") {
	{//test18.asm
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
        .text
main:
        nop
        j next
        nop
next:
        nop
        j main
	
)";
		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list
		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0x00000001);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 3);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 4);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0);
	}
	{
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(  .data
var0:   .word 0
var1:   .word 1
var2:   .word 2
var3:   .word 3
        .text
main:
        lw $t0, var0
        lw $t1, var1
        lw $t2, var2
        lw $t3, var3

        beq $t0, $t1, check1
        beq $t0, $t0, check1
        nop
check1:
        bne $t0, $t0, check2
        bne $t0, $t1, check2
        nop
check2:
        bgt $t0, $t0, check3
        bgt $t3, $t1, check3
        nop
check3:
        bge $t0, $t1, check4
        bge $t3, $t2, check4
        nop
check4:
        blt $t3, $t1, check5
        blt $t1, $t3, check5
        nop
check5:
        ble $t3, $t1, check6
        ble $t3, $t3, check6
        nop
check6:
        nop
        j check6
	
)";
		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list
		package mypack = p.getpackage();
		vm.initialize(mypack);

		vm.step();
		vm.step();
		vm.step();
		vm.step();


		REQUIRE(vm.get_reg("print $pc").to_ulong() == 4);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 5);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 7);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 8);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0xa);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0xb);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0xd);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0xe);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0x10);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0x11);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0x13);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0x14);
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0x16);
		vm.step();
		vm.step();
		REQUIRE(vm.get_reg("print $pc").to_ulong() == 0x16);
		
	}
	{
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(	# Example program to compute the sum of squares from Jorgensen [2016]

	#---------------------------------------------------------------
	# data declarations
	
	.data
n:		.word 10
sumOfSquares:	.word 0

	#---------------------------------------------------------------
	# the program
	.text
main:
	lw $t0,n
	li $t1,1
	li $t2,0

sumLoop:
	mult $t1, $t1
        mflo $t3
	add $t2, $t2, $t3
	add $t1, $t1, 1
	ble $t1, $t0, sumLoop
	sw  $t2, sumOfSquares

end:
	j end
	
)";
		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list
		package mypack = p.getpackage();
		vm.initialize(mypack);

		for (int i = 0; i < 54; i++) {
			vm.step();
		}
		REQUIRE(vm.get_mem("print &0x00000004").to_ulong() == 0x81);
		REQUIRE(vm.get_mem("print &0x00000005").to_ulong() == 1);
		REQUIRE(vm.get_mem("print &0x00000006").to_ulong() == 0);
		REQUIRE(vm.get_mem("print &0x00000007").to_ulong() == 0);
	}
	{
		//test07.asm
		parser p;
		virtual_machine vm;
		bool temp = false;
		string teststring = R"(        .data
VALUE = 2
var1:   .word 1
var2:   .word 12
var3:   .word -1
        .text
main:
        multu $t0, $t0
        or $t1, $t1, $t1
        nor $t1, $t1, $t1
        xor $t3, $t0, $t1 # 1-2 = -1
        divu $t4, $t1 # 12-1 = 11
        subu $t5, $t2, VALUE # -1-2 = -3
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		temp = p.ParseMips(list);
		REQUIRE(temp);//test if the parser inputted the list

		package mypack = p.getpackage();
		vm.initialize(mypack);
		vm.step();
		vm.step();
		vm.step();
		vm.step();
		vm.step();

	}
}
TEST_CASE("testing message queue", "[mq]") {
	{
		MessageQueue mq;
		mq.push("Hello");
		mq.push("Hello2");
		string message1;
		mq.wait_and_pop(message1);
		REQUIRE(message1 == "Hello");
		mq.wait_and_pop(message1);
		REQUIRE(message1 == "Hello2");
		
		bool isEmpty = mq.empty();
		REQUIRE(isEmpty);
	}
}
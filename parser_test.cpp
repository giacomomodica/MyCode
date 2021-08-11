#include "catch.hpp"
#include "lexer.hpp"
#include "parser.hpp"
using namespace std;

TEST_CASE("Checking word and space test", "[helpers]") {
	{
		parser myparser;
		double test = 4294967295;
		REQUIRE(myparser.CheckWord(test, false));
		REQUIRE(myparser.CheckSpace(test, false));
	}
	{
		parser myparser;
		double test2 = 4294967299;
		REQUIRE(!myparser.CheckWord(test2, false));
		REQUIRE(!myparser.CheckSpace(test2, false));
	}
}
TEST_CASE("Checking half test", "[helpers]") {
	{
		parser myparser;
		double test = 65535;
		REQUIRE(myparser.CheckHalf(test, false));
	}
	{
		parser myparser;
		double test2 = 65539;
		REQUIRE(!myparser.CheckHalf(test2, false));
	}
}
TEST_CASE("Checking byte test", "[helpers]") {
	{
		parser p;
		double test = 255;
		REQUIRE(p.CheckByte(test, false));
	}
	{
		parser p;
		double test2 = 280;
		REQUIRE(!p.CheckByte(test2, false));
	}
}
TEST_CASE("testing canStoi function", "[helpers]")
{
	{
		parser p;
		string teststring = "1234";
		REQUIRE(p.CanStoi(teststring));
	}
	{
		parser p;
		string teststring = "not a number";
		REQUIRE(!p.CanStoi(teststring));
	}
}
TEST_CASE("testing lookup table", "[helpers]")
{
	{
		parser p;
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

		REQUIRE(p.RegTable(teststr1) == 0);
		REQUIRE(p.RegTable(teststr2) == 1);
		REQUIRE(p.RegTable(teststr3) == 2);
		REQUIRE(p.RegTable(teststr4) == 3);
		REQUIRE(p.RegTable(teststr5) == 4);
		REQUIRE(p.RegTable(teststr6) == 5);
		REQUIRE(p.RegTable(teststr7) == 6);
		REQUIRE(p.RegTable(teststr8) == 7);
		REQUIRE(p.RegTable(teststr9) == 8);
		REQUIRE(p.RegTable(teststr10) == 9);
		REQUIRE(p.RegTable(teststr11) == 10);
		REQUIRE(p.RegTable(teststr12) == 11);
		REQUIRE(p.RegTable(teststr13) == 12);
		REQUIRE(p.RegTable(teststr14) == 13);
		REQUIRE(p.RegTable(teststr15) == 14);
		REQUIRE(p.RegTable(teststr16) == 15);
		REQUIRE(p.RegTable(teststr17) == 16);
		REQUIRE(p.RegTable(teststr18) == 17);
		REQUIRE(p.RegTable(teststr19) == 18);
		REQUIRE(p.RegTable(teststr20) == 19);
		REQUIRE(p.RegTable(teststr21) == 20);
		REQUIRE(p.RegTable(teststr22) == 21);
		REQUIRE(p.RegTable(teststr23) == 22);
		REQUIRE(p.RegTable(teststr24) == 23);
		REQUIRE(p.RegTable(teststr25) == 24);
		REQUIRE(p.RegTable(teststr26) == 25);
		REQUIRE(p.RegTable(teststr27) == 26);
		REQUIRE(p.RegTable(teststr28) == 27);
		REQUIRE(p.RegTable(teststr29) == 28);
		REQUIRE(p.RegTable(teststr30) == 29);
		REQUIRE(p.RegTable(teststr31) == 30);
		REQUIRE(p.RegTable(teststr32) == 31);

	}
	{
		parser p;
		//need to make this test work
		string teststr3 = "$s9";

		REQUIRE(p.RegTable(teststr3) == -1);
	}
}
TEST_CASE("testing a few correct cases", "[data]")
{
	
	{
		parser p;
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
	}
	
	{
		parser p;
		string teststring = R"(	# A test file of data declarations only
	.data
var1: .word 1032             # int var1 = 1024

var2: .half 45               # short var2 = 12
	
var3: .byte 0                # char var3 = 0

var4: .byte 1, 2, 3, 2, 5, 2, 7, 8  # var4 = {1,2,3,4,5,6,7,8}

var5: .space 512             # reserve 512 bytes
 
var6: .ascii "hello"
 
var7: .asciiz "goodbye"

	.text)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		REQUIRE(p.ParseMips(list));//test if the parser inputted the list
	}
	//testing with a signed variable
	{
		parser p;
		string teststring = R"(.data
avar: .byte +120
bvar: .half 1
cvar: .word 24
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{
		parser p;
		string teststring = R"(        .data
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}

}

TEST_CASE("testing a few error cases", "[data]")
{
	//with no .data or .text section
	{
		parser p, q;
		string teststring = R"(
avar: .word 0
bvar: .half 1
cvar: .byte 3

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
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);

		REQUIRE(!parsed);
	}
	//ending a line in a comma
	{
		parser p, q;
		string teststring = R"( .data
avar: .word 0,
bvar: .half 1
cvar: .byte 3
xvar: .word 2,
dvar: .word 1
	
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
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);

		REQUIRE(!parsed);
	}
	//incorrect size for a .data var
	{
		parser p, q;
		string teststring = R"( .data
avar: .word 0
bvar: .half 1
cvar: .byte 300
xvar: .word 2
dvar: .word 1
	
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
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);
		REQUIRE(parsed);
	}
}
TEST_CASE("testing most OF the test scripts", "[text]")
{
	{
		parser p;
		string teststring = R"(
	.data
avar:	.word 	0
bvar:	.half	1
cvar:	.byte	3

	.text
main:	
	li $t0, 45
	lw $t1, avar
	lh $t2, bvar
	lb $t2, cvar
	sw $t1, avar
	sh $t2, bvar
	sb $t2, cvar

	move $t0, $0
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{
		parser p;
		string teststring = R"(# a test of basic arithmetic operations
	.data
x:	.word	1
y:	.word	1
z:	.space 	4

	.text
main:
	lw 	$t0, x
	lw 	$t1, y
	add 	$t2, $t0, $t1
	add 	$t2, $t0, 2
	addu 	$t2, $t0, $t1
	addu 	$t2, $t0, 2
	sub 	$t2, $t0, $t1
	sub 	$t2, $t0, 2
	subu 	$t2, $t0, $t1
	subu 	$t2, $t0, 2
	mul 	$t2, $t0, $t1
	mul 	$t2, $t0, 2
	mulo 	$t2, $t0, $t1
	mulo 	$t2, $t0, 2
	mulou 	$t2, $t0, $t1
	mulou 	$t2, $t0, 2
	mult	$t0, $t1
	multu	$t0, $t1
	div 	$t2, $t0, $t1
	div 	$t2, $t0, 2
	divu 	$t2, $t0, $t1
	divu 	$t2, $t0, 2
	div	$t0, $t1
	divu	$t0, $t1
	rem 	$t2, $t0, $t1
	rem 	$t2, $t0, 2
	remu 	$t2, $t0, $t1
	remu 	$t2, $t0, 2
	abs	$t0, $t1
	neg	$t0, $t1
	negu	$t0, $t1
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{
		parser p;
		string teststring = R"(	# a test for constants
	.data
	LENGTH = 1024
arr:	.space LENGTH
	
	.text
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{
		parser p;
		string teststring = R"(# a test of all register names
	.data

	.text
main:
	sw $1, $0
	sw $at, $zero
	sw $2, $0
	sw $v0, $zero
	sw $3, $0
	sw $v1, $zero
	sw $4, $0
	sw $a0, $zero
	sw $5, $0
	sw $a1, $zero
	sw $6, $0
	sw $a2, $zero
	sw $7, $0
	sw $a3, $zero
	sw $8, $0
	sw $t0, $zero
	sw $9, $0
	sw $t1, $zero
	sw $10, $0
	sw $t2, $zero
	sw $11, $0
	sw $t3, $zero
	sw $12, $0
	sw $t4, $zero
	sw $13, $0
	sw $t5, $zero
	sw $14, $0
	sw $t6, $zero
	sw $15, $0
	sw $t7, $zero
	sw $16, $0
	sw $s0, $zero
	sw $17, $0
	sw $s1, $zero
	sw $18, $0
	sw $s2, $zero
	sw $19, $0
	sw $s3, $zero
	sw $20, $0
	sw $s4, $zero
	sw $21, $0
	sw $s5, $zero
	sw $22, $0
	sw $s6, $zero
	sw $23, $0
	sw $s7, $zero
	sw $24, $0
	sw $t8, $zero
	sw $25, $0
	sw $t9, $zero
	sw $26, $0
	sw $k0, $zero
	sw $27, $0
	sw $k1, $zero
	sw $28, $0
	sw $gp, $zero
	sw $29, $0
	sw $sp, $zero
	sw $30, $0
	sw $fp, $zero
	sw $31, $0
	sw $ra, $zero

end:
	j end
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{
		parser p;
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
	mul $t3, $t1, $t1
	add $t2, $t2, $t3
	add $t1, $t1, 1
	ble $t1, $t0, sumLoop
	sw  $t2, sumOfSquares

end:
	j end
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//#8
		parser p;
		string teststring = R"(	# a test of address modes
	.data
str:	.ascii	"hello"

	.text
main:
	lb $t8, str
	la $t0, str
	lb $t1, ($t0)
	lb $t2, 1($t0)
	lb $t3, 2($t0)
	lb $t4, 3($t0)
	lb $t5, 4($t0)
	
end:	nop
	j end
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//#5
		parser p;
		string teststring = R"(	# test of the basic control instructions
	.data
x:	.word 1
y:	.word 2

	.text
main:
	lw $t0, x
	lw $t1, y
	
	beq $t0, $t0, next1
next1:

	bne $t0, $t1, next2
next2:

	blt $t0, $t1, next3
next3:

	ble $t0, $t0, next4
next4:

	bgt $t1, $t0, next5
next5:

	bge $t0, $t0, next6
next6:
	
end:
	j	end
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
}

TEST_CASE("testing some bad scripts", "[text]")
{
	{
		parser p;
		string teststring = R"(.data
avar: .word 0
bvar: .half 1
cvar: .byte 3
	
	.text
main:	
	li $a5, 45
	lw $t1, 23
	lh $t2, bvar
	lb $t2, cvar
	sw $t1, avar
	sh $t2, bvar
	sb $t2, cvar

	move $t0, $0)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);

		REQUIRE(!parsed);
	}
	{
		parser p;
		string teststring = R"(	# A test file of data declarations only
	.data
var1:	.word 1024             # int var1 = 1024

var2:	.half 12               # short var2 = 12
	
var3:	.byte 0                # char var3 = 0

var4:	.byte 1, 2, 3, 4, 5, 6, 7,   # PARSE ERROR

var5:	.space 512             # reserve 512 bytes

var6:	.ascii "hello"

var7:	.asciiz "goodbye"

	.text
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);

		REQUIRE(!parsed);
	}
	{
		parser p;
		string teststring = R"(	# A test file of data declarations only
	.data
var1:	.word              # PARSE ERROR
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);

		REQUIRE(!parsed);
	}
	{
		parser p;
		string teststring = R"(	.data
var:	.word 0
	.word 1
	.word 2

	.text
	
	# lexer error line 10 
main:	la $t0, var 
	lw $s1, ($t0
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);
		REQUIRE(!parsed);
	}
	{//3
		parser p;
		string teststring = R"(	# a test for basic data movement
	.data
avar:	.word 	0
bvar:	.half	1
cvar:	.byte	3

	.text
main:	
	li $t0, $t1 # Parse Error
	lw $t1, avar
	lh $t2, bvar
	lb $t2, cvar
	sw $t1, avar
	sh $t2, bvar
	sb $t2, cvar

	move $t0, $0
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);
		REQUIRE(!parsed);
	}
	{
		parser p;
		string teststring = R"(	# a test of basic arithmetic operations
	.data
x:	.word	1
y:	.word	1
z:	.space 	4

	.text
main:
	lw 	$t0, x
	lw 	$t1, y
	add 	$t2, $t0, $t1
	add 	$t2, $t0, 2
	addu 	$t2, $t0, $t1
	addu 	$t2, $t0, 2
	sub 	$t2, $t0, $t1
	sub 	$t2, $t0, 2
	subu 	$t2, $t0, $t1
	subu 	$t2, $t0, 2
	mul 	$t2, $t0, $t1
	mul 	$t2, $t0, 2
	mulo 	$t2, $t0, $t1
	mulo 	$t2, $t0, 2
	mulou 	$t2, $t0, $t1
	mulou 	$t2, $t0, 2
	mult	$t0, $t1
	multu	$t0, $t1
	div 	$t2, $t0, $t1
	div 	$t2, $t0, 2
	divu 	$t2, $t0, $t1
	divu 	$t2, $t0, 2
	div	$t0, $t1
	divu	$t0, $t1
	rem 	$t2, $t0, $t1
	rem 	$t2, $t0	# parse error
	remu 	$t2, $t0, $t1
	remu 	$t2, $t0, 2
	abs	$t0, $t1
	neg	$t0, $t1
	negu	$t0, $t1
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		bool parsed = true;
		string error_message;
		//FIX THE ERROR
		parsed = p.ParseMips(list);
		REQUIRE(!parsed);
	}
	
	{//5
		parser p;
		string teststring = R"(	# test of basic logical instructions
	.data
	TRUE = 1
	FALSE = 0

test1:	.word TRUE
test2:	.word FALSE
	
	.text
main:
	lw	$t0, test1
	lw	$t1, test2
	
	and	$t2, $t0, $t1
	and	$t2, $t0, TRUE
	nor	$t2, $t0, $t1
	nor	$t2, $t0, TRUE, FALSE # parse error
	not	$t2, $t0
	not	$t2, $t0
	or	$t2, $t0, $t1
	or	$t2, $t0, TRUE
	xor	$t2, $t0, $t1
	xor	$t2, $t0, TRUE
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);
		REQUIRE(!parsed);
	}
	
	{//7
		parser p;
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
	mul $t3, $t1, $t1
	add $t2, $t2, $32	# parse error
	add $t1, $t1, 1
	ble $t1, $t0, sumLoop
	sw  $t2, sumOfSquares

end:
	j end
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);
		bool parsed = true;
		string error_message;
		parsed = p.ParseMips(list);
		REQUIRE(!parsed);
	}
}
TEST_CASE("testing all new scripts from VM tests", "[VMTest]")
{
	{//0
		parser p;
		string teststring = R"(# simple infinite loop
	.data
	.text
main:	j main
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{ //1
		parser p;
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//2
		parser p;
		string teststring = R"(        .data
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//3
		parser p;
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//4
		parser p;
		string teststring = R"(        .data
VALUE = -1234

        .text
main:
        li $t0, VALUE
        move $t1, $t0
        move $t2, $t1
        move $t3, $t2
        move $t4, $t3
        move $t5, $t4
        move $t6, $t5
        move $t7, $t6
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//5
		parser p;
		string teststring = R"(        .data
VALUE = -1
var:    .word 1
        .text
main:
        lw $t0, var
        add $t1, $t0, VALUE
        add $t2, $t1, $t0
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list

	}
	{//6
		parser p;
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//7
		parser p;
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//8
		parser p;
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
        subu $t3, $t0, VALUE # 1-2 = -1 = 4294967295
        subu $t4, $t1, $t0 # 12-1 = 11
        subu $t5, $t2, VALUE # -1-2 = -3 = 4294967293
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//9
		parser p;
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//10
		//overflow test
		parser p;
		string teststring = R"(        .data
VALUE = 4
var1:   .word 2
var2:   .word 1073741824 # = 2^30
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        mult $t0, $t1 # 2*4 = 8
        mflo $t8
        mfhi $t9

        lw $t0, var2
        li $t1, VALUE
        mult $t0, $t1 # 1073741824*4 = 4294967296 (overflow)
        mflo $t8
        mfhi $t9
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//11
		parser p;
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
        div $t0, $t1 
        mflo $t8
        mfhi $t9

        lw $t0, var2
        lw $t1, var1
        div $t0, $t1
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE
        div $t0, $t1
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE2
        div $t0, $t1 
        mflo $t8
        mfhi $t9

        # divide by 0
        div $t0, $0
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//12
		parser p;
		string teststring = R"(        .data
VALUE = 4
var1:   .word 2
var2:   .word 1073741825 # = 2^30+1
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        div $t0, $t1 # 2/4 = 0 rem 2
        mflo $t8
        mfhi $t9

        lw $t0, var2
        li $t1, VALUE
        div $t0, $t1 # 1073741825/4 = 268435456 rem 1
        mflo $t8
        mfhi $t9

        # divide by 0
        div $t0, $0
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//13
		parser p;
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//14
		parser p;
		string teststring = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        nor $t2, $t0, $t1  
        nor $t2, $t0, VALUE
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//15
		parser p;
		string teststring = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        or $t2, $t0, $t1  
        or $t2, $t0, VALUE
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//16
		parser p;
		string teststring = R"(        .data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        xor $t2, $t0, $t1  
        xor $t2, $t0, VALUE
	
)";

		istringstream iss(teststring);
		TokenList list = tokenize(iss);

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//17
		parser p;
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//18
		parser p;
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//19
		parser p;
		string teststring = R"(        .data
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
	{//20
		parser p;
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

		bool parsed = p.ParseMips(list);
		REQUIRE(parsed); //test if the parser inputted the list
	}
}

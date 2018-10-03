# mips_hex_to_assembly
A c++ program to convert 32-bit hex code into MIPS instruction.

Please refer to the book [Computer Organization and Design](https://www.amazon.com/Computer-Organization-Design-MIPS-Architecture/dp/0124077269) p.191 for the decoding table.

The program requires 2 arguments as the input/output file name.

File '`i_type`', '`r_type`' and '`registers`' are where the name of the instructions and registers written in. The program will read and build the table mentioned above out of them.

## Using the program
```
$ g++ -o converter converter.cpp
$ ./converter sample_input.txt out.txt
```

## Sample input and output
Input as follows:
```
08 00 00 12
00 00 00 00
20 0C 00 02
00 8C 60 2A
11 80 00 02
00 80 10 20
03 E0 00 08
20 08 00 01
20 09 00 00
20 0B 00 01
01 09 50 20
01 00 48 20
01 40 40 20
21 6B 00 01
15 64 FF FB
01 00 10 20
03 E0 00 08
00 00 00 00
20 04 00 19
0C 00 00 02
```

Output:
```
j 18
nop
addi $t4, $zero, 2
slt $t4, $a0, $t4
beq $t4, $zero, 2
add $v0, $a0, $zero
jr $ra
addi $t0, $zero, 1
addi $t1, $zero, 0
addi $t3, $zero, 1
add $t2, $t0, $t1
add $t1, $t0, $zero
add $t0, $t2, $zero
addi $t3, $t3, 1
bnq $t3, $a0, 65531
add $v0, $t0, $zero
jr $ra
nop
addi $a0, $zero, 25
jal 2
```

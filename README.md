# AdvancedMIPSasm
An assembler for my MIPS CPU.



## ISA definition

| Instruction            |                Encoding                 | Function                    |
| :--------------------- | :-------------------------------------: | --------------------------- |
| ``add rd, rs, rt``     | ``000000[ rs ][ rt ][ rd ]00000100010`` | rd <— rs + rt               |
| ``sub rd, rs, rt``     | ``000000[ rs ][ rt ][ rd ]00000100011`` | rd <— rs - rt               |
| ``and rd, rs, rt``     | ``000000[ rs ][ rt ][ rd ]00000100000`` | rd <— rs & rt               |
| ``or rd, rs, rt``      | ``000000[ rs ][ rt ][ rd ]00000100001`` | rd <— rs \| rt              |
| ``slt rd, rs, rt``     | ``000000[ rs ][ rt ][ rd ]00000101010`` | rd <— rs < rt ? 1 : 0       |
| ``sll rd, rt, shamt``  | ``00000000000[ rt ][ rd ][ sh ]001000`` | rd <— rt << shamt           |
| ``srl rd, rt, shamt``  | ``00000000000[ rt ][ rd ][ sh ]001001`` | rd <— rt >> shamt           |
| ``addi rt, rs, imm16`` | ``001000[ rs ][ rt ][     imm16     ]`` | rt <— rs + sext(imm16)      |
| ``andi rt, rs, imm16`` | ``001100[ rs ][ rt ][     imm16     ]`` | rt <— rs & zext(imm16)      |
| ``ori rt, rs, imm16``  | ``001101[ rs ][ rt ][     imm16     ]`` | rt <— rs \| zext(imm16)     |
| ``lw rt, rs, imm16``   | ``100011[ rs ][ rt ][     imm16     ]`` | rt <— M[rs + sext(imm16)]   |
| ``sw rt, rs, imm16``   | ``101011[ rs ][ rt ][     imm16     ]`` | M[rs + sext(imm16)] <— rt   |
| ``beq rt, rs, imm16``  | ``000100[ rs ][ rt ][     imm16     ]`` | if rs == rt then            |
|                        |                                         | PC <— PC+4+(sext(imm16)<<2) |
| ``jmp imm26``          | ``000010[          imm26            ]`` | PC <— PC[31:28]\|(imm26<<2) |
| ``jal imm26``          | ``000011[          imm26            ]`` | lr <— PC + 4                |
|                        |                                         | PC <— PC[31:28]\|(imm26<<2) |
| ``jr rs``              | ``000101[  rs  ]000000000000000000000`` | PC <— rs                    |
| ``push rt``            | ``11000000000[  rt  ]0000000000000000`` | Stack[sp -= 4] <— rt        |
| ``pop rd``             | ``1100010000000000[  rd  ]00000000000`` | rd <— Stack[sp]             |
|                        |                                         | sp += 4                     |

## Register Names

| Name1   | Name2    |
| ------- | -------- |
| ``r0``  | ``zero`` |
| ``r1``  | ``at``   |
| ``r2``  | ``v0``   |
| ``r3``  | ``v1``   |
| ``r4``  | ``a0``   |
| ``r5``  | ``a1``   |
| ``r6``  | ``a2``   |
| ``r7``  | ``a3``   |
| ``r8``  | ``t0``   |
| ``r9``  | ``t1``   |
| ``r10`` | ``t2``   |
| ``r11`` | ``t3``   |
| ``r12`` | ``t4``   |
| ``r13`` | ``t5``   |
| ``r14`` | ``t6``   |
| ``r15`` | ``t7``   |
| ``r16`` | ``s0``   |
| ``r17`` | ``s1``   |
| ``r18`` | ``s2``   |
| ``r19`` | ``s3``   |
| ``r20`` | ``s4``   |
| ``r21`` | ``s5``   |
| ``r22`` | ``s6``   |
| ``r23`` | ``s7``   |
| ``r24`` | ``t8``   |
| ``r25`` | ``t9``   |
| ``r26`` |          |
| ``r27`` |          |
| ``r28`` | ``gp``   |
| ``r29`` | ``fp``   |
| ``r30`` | ``sp``   |
| ``r31`` | ``lr``   |


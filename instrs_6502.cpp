#include <vector>

#include "cpu.h"

// CPU Instruction Table (256 total instructions)
vector<CpuInstruction> instrs_6502 {
/* 00 */ { "BRK", &Cpu::addr_imp, &Cpu::instr_brk, false, 7, 1 },
/* 01 */ { "ORA", &Cpu::addr_x_ind, &Cpu::instr_ora, false, 6, 2 },
/* 02 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 03 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 04 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 05 */ { "ORA", &Cpu::addr_zpg, &Cpu::instr_ora, false, 3, 2 },
/* 06 */ { "ASL", &Cpu::addr_zpg, &Cpu::instr_asl, false, 5, 2 },
/* 07 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 08 */ { "PHP", &Cpu::addr_imp, &Cpu::instr_php, false, 3, 1 },
/* 09 */ { "ORA", &Cpu::addr_imm, &Cpu::instr_ora, false, 2, 2 },
/* 0A */ { "ASL", &Cpu::addr_acc, &Cpu::instr_asl, false, 2, 1 },
/* 0B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 0C */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 0D */ { "ORA", &Cpu::addr_abs, &Cpu::instr_ora, false, 4, 3 },
/* 0E */ { "ASL", &Cpu::addr_abs, &Cpu::instr_asl, false, 6, 3 },
/* 0F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 10 */ { "BPL", &Cpu::addr_rel, &Cpu::instr_bpl, true, 2, 2 },
/* 11 */ { "ORA", &Cpu::addr_ind_y, &Cpu::instr_ora, true, 5, 2 },
/* 12 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 13 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 14 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 15 */ { "ORA", &Cpu::addr_zpg_x, &Cpu::instr_ora, false, 4, 2 },
/* 16 */ { "ASL", &Cpu::addr_zpg_x, &Cpu::instr_asl, false, 6, 2 },
/* 17 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 18 */ { "CLC", &Cpu::addr_imp, &Cpu::instr_clc, false, 2, 1 },
/* 19 */ { "ORA", &Cpu::addr_abs_y, &Cpu::instr_ora, true, 4, 3 },
/* 1A */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 1B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 1C */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 1D */ { "ORA", &Cpu::addr_abs_x, &Cpu::instr_ora, true, 4, 3 },
/* 1E */ { "ASL", &Cpu::addr_abs_x, &Cpu::instr_asl, false, 7, 3 },
/* 1F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 20 */ { "JSR", &Cpu::addr_abs, &Cpu::instr_jsr, false, 6, 3 },
/* 21 */ { "AND", &Cpu::addr_x_ind, &Cpu::instr_and, false, 6, 2 },
/* 22 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 23 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 24 */ { "BIT", &Cpu::addr_zpg, &Cpu::instr_bit, false, 3, 2 },
/* 25 */ { "AND", &Cpu::addr_zpg, &Cpu::instr_and, false, 3, 2 },
/* 26 */ { "ROL", &Cpu::addr_zpg, &Cpu::instr_rol, false, 5, 2 },
/* 27 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 28 */ { "PLP", &Cpu::addr_imp, &Cpu::instr_plp, false, 4, 1 },
/* 29 */ { "AND", &Cpu::addr_imm, &Cpu::instr_and, false, 2, 2 },
/* 2A */ { "ROL", &Cpu::addr_acc, &Cpu::instr_rol, false, 2, 1 },
/* 2B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 2C */ { "BIT", &Cpu::addr_abs, &Cpu::instr_bit, false, 4, 3 },
/* 2D */ { "AND", &Cpu::addr_abs, &Cpu::instr_and, false, 4, 3 },
/* 2E */ { "ROL", &Cpu::addr_abs, &Cpu::instr_rol, false, 6, 3 },
/* 2F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 30 */ { "BMI", &Cpu::addr_rel, &Cpu::instr_bmi, true, 2, 2 },
/* 31 */ { "AND", &Cpu::addr_ind_y, &Cpu::instr_and, true, 5, 2 },
/* 32 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 33 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 34 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 35 */ { "AND", &Cpu::addr_zpg_x, &Cpu::instr_and, false, 4, 2 },
/* 36 */ { "ROL", &Cpu::addr_zpg_x, &Cpu::instr_rol, false, 6, 2 },
/* 37 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 38 */ { "SEC", &Cpu::addr_imp, &Cpu::instr_sec, false, 2, 1 },
/* 39 */ { "AND", &Cpu::addr_abs_y, &Cpu::instr_and, true, 4, 3 },
/* 3A */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 3B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 3C */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 3D */ { "AND", &Cpu::addr_abs_x, &Cpu::instr_and, true, 4, 3 },
/* 3E */ { "ROL", &Cpu::addr_abs_x, &Cpu::instr_rol, false, 7, 3 },
/* 3F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 40 */ { "RTI", &Cpu::addr_imp, &Cpu::instr_rti, false, 6, 1 },
/* 41 */ { "EOR", &Cpu::addr_x_ind, &Cpu::instr_eor, false, 6, 2 },
/* 42 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 43 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 44 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 45 */ { "EOR", &Cpu::addr_zpg, &Cpu::instr_eor, false, 3, 2 },
/* 46 */ { "LSR", &Cpu::addr_zpg, &Cpu::instr_lsr, false, 5, 2 },
/* 47 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 48 */ { "PHA", &Cpu::addr_imp, &Cpu::instr_pha, false, 3, 1 },
/* 49 */ { "EOR", &Cpu::addr_imm, &Cpu::instr_eor, false, 2, 2 },
/* 4A */ { "LSR", &Cpu::addr_acc, &Cpu::instr_lsr, false, 2, 1 },
/* 4B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 4C */ { "JMP", &Cpu::addr_abs, &Cpu::instr_jmp, false, 3, 3 },
/* 4D */ { "EOR", &Cpu::addr_abs, &Cpu::instr_eor, false, 3, 4 },
/* 4E */ { "LSR", &Cpu::addr_abs, &Cpu::instr_lsr, false, 6, 3 },
/* 4F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 50 */ { "BVC", &Cpu::addr_rel, &Cpu::instr_bvc, true, 2, 2 },
/* 51 */ { "EOR", &Cpu::addr_ind_y, &Cpu::instr_eor, true, 5, 2 },
/* 52 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 53 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 54 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 55 */ { "EOR", &Cpu::addr_zpg_x, &Cpu::instr_eor, false, 0, 0 },
/* 56 */ { "LSR", &Cpu::addr_zpg_x, &Cpu::instr_lsr, false, 6, 2 },
/* 57 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 58 */ { "CLI", &Cpu::addr_imp, &Cpu::instr_cli, false, 2, 1 },
/* 59 */ { "EOR", &Cpu::addr_abs_y, &Cpu::instr_eor, true, 4, 3 },
/* 5A */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 5B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 5C */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 5D */ { "EOR", &Cpu::addr_abs_x, &Cpu::instr_eor, true, 4, 3 },
/* 5E */ { "LSR", &Cpu::addr_abs_x, &Cpu::instr_lsr, false, 7, 3 },
/* 5F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 60 */ { "RTS", &Cpu::addr_imp, &Cpu::instr_rts, false, 6, 1 },
/* 61 */ { "ADC", &Cpu::addr_x_ind, &Cpu::instr_adc, false, 6, 2 },
/* 62 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 63 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 64 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 65 */ { "ADC", &Cpu::addr_zpg, &Cpu::instr_adc, false, 3, 2 },
/* 66 */ { "ROR", &Cpu::addr_zpg, &Cpu::instr_ror, false, 5, 2 },
/* 67 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 68 */ { "PLA", &Cpu::addr_imp, &Cpu::instr_pla, false, 4, 1 },
/* 69 */ { "ADC", &Cpu::addr_imm, &Cpu::instr_adc, false, 2, 2 },
/* 6A */ { "ROR", &Cpu::addr_acc, &Cpu::instr_ror, false, 2, 1 },
/* 6B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 6C */ { "JMP", &Cpu::addr_ind, &Cpu::instr_jmp, false, 5, 3 },
/* 6D */ { "ADC", &Cpu::addr_abs, &Cpu::instr_adc, false, 4, 3 },
/* 6E */ { "ROR", &Cpu::addr_abs, &Cpu::instr_ror, false, 6, 3 },
/* 6F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 70 */ { "BVS", &Cpu::addr_rel, &Cpu::instr_bvs, true, 2, 2 },
/* 71 */ { "ADC", &Cpu::addr_ind_y, &Cpu::instr_adc, true, 5, 2 },
/* 72 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 73 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 74 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 75 */ { "ADC", &Cpu::addr_zpg_x, &Cpu::instr_adc, false, 4, 2 },
/* 76 */ { "ROR", &Cpu::addr_zpg_x, &Cpu::instr_ror, false, 6, 2 },
/* 77 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 78 */ { "SEI", &Cpu::addr_imp, &Cpu::instr_sei, false, 2, 1 },
/* 79 */ { "ADC", &Cpu::addr_abs_y, &Cpu::instr_adc, true, 4, 3 },
/* 7A */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 7B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 7C */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 7D */ { "ADC", &Cpu::addr_abs_x, &Cpu::instr_adc, true, 4, 3 },
/* 7E */ { "ROR", &Cpu::addr_abs_x, &Cpu::instr_ror, false, 7, 3 },
/* 7F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 80 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 81 */ { "STA", &Cpu::addr_x_ind, &Cpu::instr_sta, false, 6, 2 },
/* 82 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 83 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 84 */ { "STY", &Cpu::addr_zpg, &Cpu::instr_sty, false, 3, 2 },
/* 85 */ { "STA", &Cpu::addr_zpg, &Cpu::instr_sta, false, 3, 2 },
/* 86 */ { "STX", &Cpu::addr_zpg, &Cpu::instr_stx, false, 3, 2 },
/* 87 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 88 */ { "DEY", &Cpu::addr_imp, &Cpu::instr_dey, false, 2, 1 },
/* 89 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 8A */ { "TXA", &Cpu::addr_imp, &Cpu::instr_txa, false, 2, 1 },
/* 8B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 8C */ { "STY", &Cpu::addr_abs, &Cpu::instr_sty, false, 4, 3 },
/* 8D */ { "STA", &Cpu::addr_abs, &Cpu::instr_sta, false, 4, 3 },
/* 8E */ { "STX", &Cpu::addr_abs, &Cpu::instr_stx, false, 4, 3 },
/* 8F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 90 */ { "BCC", &Cpu::addr_rel, &Cpu::instr_bcc, true, 2, 2 },
/* 91 */ { "STA", &Cpu::addr_ind_y, &Cpu::instr_sta, false, 6, 2 },
/* 92 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 93 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 94 */ { "STY", &Cpu::addr_zpg_x, &Cpu::instr_sty, false, 4, 2 },
/* 95 */ { "STA", &Cpu::addr_zpg_x, &Cpu::instr_sta, false, 4, 2 },
/* 96 */ { "STX", &Cpu::addr_zpg_y, &Cpu::instr_stx, false, 4, 2 },
/* 97 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 98 */ { "TYA", &Cpu::addr_imp, &Cpu::instr_tya, false, 2, 1 },
/* 99 */ { "STA", &Cpu::addr_abs_y, &Cpu::instr_sta, false, 5, 3 },
/* 9A */ { "TXS", &Cpu::addr_imp, &Cpu::instr_txs, false, 2, 1 },
/* 9B */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 9C */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 9D */ { "STA", &Cpu::addr_abs_x, &Cpu::instr_sta, false, 5, 3 },
/* 9E */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* 9F */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* A0 */ { "LDY", &Cpu::addr_imm, &Cpu::instr_ldy, false, 2, 2 },
/* A1 */ { "LDA", &Cpu::addr_x_ind, &Cpu::instr_lda, false, 6, 2 },
/* A2 */ { "LDX", &Cpu::addr_imm, &Cpu::instr_ldx, false, 2, 2 },
/* A3 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* A4 */ { "LDY", &Cpu::addr_zpg, &Cpu::instr_ldy, false, 3, 2 },
/* A5 */ { "LDA", &Cpu::addr_zpg, &Cpu::instr_lda, false, 3, 2 },
/* A6 */ { "LDX", &Cpu::addr_zpg, &Cpu::instr_ldx, false, 3, 2 },
/* A7 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* A8 */ { "TAY", &Cpu::addr_imp, &Cpu::instr_tay, false, 0, 0 },
/* A9 */ { "LDA", &Cpu::addr_imm, &Cpu::instr_lda, false, 2, 2 },
/* AA */ { "TAX", &Cpu::addr_imp, &Cpu::instr_tax, false, 2, 1 },
/* AB */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* AC */ { "LDY", &Cpu::addr_abs, &Cpu::instr_ldy, false, 4, 3 },
/* AD */ { "LDA", &Cpu::addr_abs, &Cpu::instr_lda, false, 4, 3 },
/* AE */ { "LDX", &Cpu::addr_abs, &Cpu::instr_ldx, false, 4, 3 },
/* AF */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* B0 */ { "BCS", &Cpu::addr_rel, &Cpu::instr_bcs, true, 2, 2 },
/* B1 */ { "LDA", &Cpu::addr_ind_y, &Cpu::instr_lda, true, 5, 2 },
/* B2 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* B3 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* B4 */ { "LDY", &Cpu::addr_zpg_x, &Cpu::instr_ldy, false, 4, 2 },
/* B5 */ { "LDA", &Cpu::addr_zpg_x, &Cpu::instr_lda, false, 4, 2 },
/* B6 */ { "LDX", &Cpu::addr_zpg_y, &Cpu::instr_ldx, false, 4, 2 },
/* B7 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* B8 */ { "CLV", &Cpu::addr_imp, &Cpu::instr_clv, false, 2, 1 },
/* B9 */ { "LDA", &Cpu::addr_abs_y, &Cpu::instr_lda, true, 4, 3 },
/* BA */ { "TSX", &Cpu::addr_imp, &Cpu::instr_tsx, false, 2, 1 },
/* BB */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* BC */ { "LDY", &Cpu::addr_abs_x, &Cpu::instr_ldy, true, 4, 3 },
/* BD */ { "LDA", &Cpu::addr_abs_x, &Cpu::instr_lda, true, 4, 3 },
/* BE */ { "LDX", &Cpu::addr_abs_y, &Cpu::instr_ldx, true, 4, 3 },
/* BF */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* C0 */ { "CPY", &Cpu::addr_imm, &Cpu::instr_cpy, false, 2, 2 },
/* C1 */ { "CMP", &Cpu::addr_x_ind, &Cpu::instr_cmp, false, 6, 2 },
/* C2 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* C3 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* C4 */ { "CPY", &Cpu::addr_zpg, &Cpu::instr_cpy, false, 3, 2 },
/* C5 */ { "CMP", &Cpu::addr_zpg, &Cpu::instr_cmp, false, 3, 2 },
/* C6 */ { "DEC", &Cpu::addr_zpg, &Cpu::instr_dec, false, 5, 2 },
/* C7 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* C8 */ { "INY", &Cpu::addr_imp, &Cpu::instr_iny, false, 2, 1 },
/* C9 */ { "CMP", &Cpu::addr_imm, &Cpu::instr_cmp, false, 2, 2 },
/* CA */ { "DEX", &Cpu::addr_imp, &Cpu::instr_dex, false, 2, 1 },
/* CB */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* CC */ { "CPY", &Cpu::addr_abs, &Cpu::instr_cpy, false, 4, 3 },
/* CD */ { "CMP", &Cpu::addr_abs, &Cpu::instr_cmp, false, 4, 3 },
/* CE */ { "DEC", &Cpu::addr_abs, &Cpu::instr_dec, false, 3, 3 },
/* CF */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* D0 */ { "BNE", &Cpu::addr_rel, &Cpu::instr_bne, true, 2, 2 },
/* D1 */ { "CMP", &Cpu::addr_ind_y, &Cpu::instr_cmp, true, 5, 2 },
/* D2 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* D3 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* D4 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* D5 */ { "CMP", &Cpu::addr_zpg_x, &Cpu::instr_cmp, false, 4, 2 },
/* D6 */ { "DEC", &Cpu::addr_zpg_x, &Cpu::instr_dec, false, 6, 2 },
/* D7 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* D8 */ { "CLD", &Cpu::addr_imp, &Cpu::instr_cld, false, 2, 1 },
/* D9 */ { "CMP", &Cpu::addr_abs_y, &Cpu::instr_cmp, true, 4, 3 },
/* DA */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* DB */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* DC */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* DD */ { "CMP", &Cpu::addr_abs_x, &Cpu::instr_cmp, true, 4, 3 },
/* DE */ { "DEC", &Cpu::addr_abs_x, &Cpu::instr_dec, false, 7, 3 },
/* DF */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* E0 */ { "CPX", &Cpu::addr_imm, &Cpu::instr_cpx, false, 2, 2 },
/* E1 */ { "SBC", &Cpu::addr_x_ind, &Cpu::instr_sbc, false, 6, 2 },
/* E2 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* E3 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* E4 */ { "CPX", &Cpu::addr_zpg, &Cpu::instr_cpx, false, 3, 2 },
/* E5 */ { "SBC", &Cpu::addr_zpg, &Cpu::instr_sbc, false, 3, 2 },
/* E6 */ { "INC", &Cpu::addr_zpg, &Cpu::instr_inc, false, 5, 2 },
/* E7 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* E8 */ { "INX", &Cpu::addr_imp, &Cpu::instr_inx, false, 2, 1 },
/* E9 */ { "SBC", &Cpu::addr_imm, &Cpu::instr_sbc, false, 2, 2 },
/* EA */ { "NOP", &Cpu::addr_imp, &Cpu::instr_nop, false, 2, 1 },
/* EB */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* EC */ { "CPX", &Cpu::addr_abs, &Cpu::instr_cpx, false, 4, 3 },
/* ED */ { "SBC", &Cpu::addr_abs, &Cpu::instr_sbc, false, 4, 3 },
/* EE */ { "INC", &Cpu::addr_abs, &Cpu::instr_inc, false, 6, 3 },
/* EF */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* F0 */ { "BEQ", &Cpu::addr_rel, &Cpu::instr_beq, true, 2, 2 },
/* F1 */ { "SBC", &Cpu::addr_ind_y, &Cpu::instr_sbc, true, 5, 2 },
/* F2 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* F3 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* F4 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* F5 */ { "SBC", &Cpu::addr_zpg_x, &Cpu::instr_sbc, false, 4, 2 },
/* F6 */ { "INC", &Cpu::addr_zpg_x, &Cpu::instr_inc, false, 6, 2 },
/* F7 */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* F8 */ { "SED", &Cpu::addr_imp, &Cpu::instr_sed, false, 2, 1 },
/* F9 */ { "SBC", &Cpu::addr_abs_y, &Cpu::instr_sbc, true, 4, 3 },
/* FA */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* FB */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* FC */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 },
/* FD */ { "SBC", &Cpu::addr_abs_x, &Cpu::instr_sbc, true, 4, 3 },
/* FE */ { "INC", &Cpu::addr_abs_x, &Cpu::instr_inc, false, 7, 3 },
/* FF */ { "UND", &Cpu::addr_imp, &Cpu::instr_und, false, 2, 1 }
};

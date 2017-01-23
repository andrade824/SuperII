/**
 * 6502 CPU Implementation.
 */
#include "cpu.h"

#include <cstdint>
#include <iostream>

using std::cout;
using std::endl;

/**
 * Constructor.
 *
 * @param bus The device to perform reads/writes over.
 * @param opcodes A mapping between opcodes and the instructions/address modes
 *                that make up that opcode.
 */
Cpu::Cpu(IMemoryMapped &bus, vector<CpuInstruction> &opcodes)
    : _bus(bus), _opcodes(opcodes)
{
    Reset();
}

/**
 * Reset the state of the processor to when it was just powered on.
 */
void Cpu::Reset()
{
    _total_cycles = 0;
    _effective_addr = 0;

    _context.pc = bus_read16(0xFFFC);
    _context.acc = 0;
    _context.x = 0;
    _context.y = 0;
    _context.sp = 0xFD;
    _context.sr = 0x20;
}

/**
 * Execute a set number of instructions based on their cycle counts.
 *
 * @param num_cycles The number of cycles to execute before stopping execution.
 *
 * @return The number of cycles that actually ran.
 */
uint32_t Cpu::Execute(uint32_t num_cycles)
{
    uint32_t starting_cycles = _total_cycles;

    while (_total_cycles < (starting_cycles + num_cycles))
        SingleStep();

    return _total_cycles - starting_cycles;
}

/**
 * Execute a single instruction.
 */
void Cpu::SingleStep()
{
    bool crossed_page_boundary = false;
    _cur_opcode = _bus.Read(_context.pc++);

    // Run the instruction
    crossed_page_boundary = CALL_MEMBER_FN(_opcodes[_cur_opcode].addr_mode)();
    CALL_MEMBER_FN(_opcodes[_cur_opcode].instr)();

    _num_instr++;

    // Calculate cycles used
    _total_cycles += _opcodes[_cur_opcode].cycles;
    if(crossed_page_boundary && _opcodes[_cur_opcode].has_page_penalty)
        _total_cycles++;
}

/**
 * Helper function for performing "16-bit" bus reads. In reality, this will
 * perform two 8-bit reads since the 6502 only has an 8-bit data bus.
 *
 * @param addr 16-bit address to start reading from.
 *
 * @return 16-bit value starting at addr.
 */
uint16_t Cpu::bus_read16(uint16_t addr) const
{
    return (uint16_t)_bus.Read(addr) | ((uint16_t)_bus.Read(addr + 1) << 8);
}

/**
 * Helper function for saving an instruction's result. If the opcode uses the
 * 'Accumulator' addressing mode, the result is stored in the accumulator
 * register. Otherwise, the result is written to the bus at the calculated
 * effective address.
 *
 * @param result The value to save.
 */
void Cpu::save_result(uint16_t result)
{
    if(_opcodes[_cur_opcode].addr_mode == &Cpu::addr_acc)
        _context.acc = result;
    else
        _bus.Write(_effective_addr, result);
}

/**
 * Helper function for handling branches on flag values. Correctly handles
 * incrementing the _total_cycles variable based on whether the branch passed
 * a page boundary.
 *
 *
 * @param flag The flag to branch on.
 * @param value If the flag is set to this value, it executes the branch.
 */
void Cpu::do_branch(CpuFlag flag, uint8_t value)
{
    uint16_t old_pc = _context.pc;
    uint16_t rel = _effective_value;

    // Sign extend the 8-bit number
    if(rel & 0x80)
        rel |= 0xFF00;

    if(get_flag(flag) == value) {
        _context.pc += rel;

        if((old_pc & 0xFF00) != (_context.pc & 0xFF00))
            _total_cycles += 2;
        else
            _total_cycles++;
    }
}

/*******************************************************************************
                            STACK MANIPULATION
 ******************************************************************************/

void Cpu::push8(uint8_t value)
{
    _bus.Write(_stack_base + _context.sp--, value);
}

void Cpu::push16(uint16_t value)
{
    _bus.Write(_stack_base + _context.sp, (value >> 8) & 0xFF);
    _bus.Write(_stack_base + (_context.sp - 1), value & 0xFF);

    _context.sp -= 2;
}

uint8_t Cpu::pull8()
{
    return _bus.Read(_stack_base + (++_context.sp));
}

uint16_t Cpu::pull16()
{
    uint16_t temp = _bus.Read(_stack_base + _context.sp + 1) |
                    (_bus.Read(_stack_base + _context.sp + 2) << 8);

    _context.sp += 2;

    return temp;
}

/*******************************************************************************
                            FLAG MANIPULATION
 ******************************************************************************/

uint8_t Cpu::get_flag(CpuFlag flag) const
{
    return _context.sr & flag;
}

void Cpu::set_flag(CpuFlag flag, uint8_t value)
{
    if(value)
        _context.sr |= flag;
    else
        _context.sr &= ~(flag);
}

void Cpu::update_carry(uint16_t result)
{
    if(result & 0xFF00)
        set_flag(FLAG_CARRY, 1);
    else
        set_flag(FLAG_CARRY, 0);
}

void Cpu::update_zero(uint16_t result)
{
    if(result & 0xFF)
        set_flag(FLAG_ZERO, 0);
    else
        set_flag(FLAG_ZERO, 1);
}

void Cpu::update_overflow(uint16_t result)
{
    if((result ^ _context.acc) & (result ^ _effective_value) & 0x80)
        set_flag(FLAG_OVERFLOW, 1);
    else
        set_flag(FLAG_OVERFLOW, 0);
}

void Cpu::update_negative(uint16_t result)
{
    if(result & 0x80)
        set_flag(FLAG_NEGATIVE, 1);
    else
        set_flag(FLAG_NEGATIVE, 0);
}

/*******************************************************************************
                            ADDRESSING MODE CALCULATIONS
 ******************************************************************************/

/**
 * Accumulator Addressing Mode.
 */
bool Cpu::addr_acc()
{
    _effective_addr = 0;
    _effective_value = _context.acc;

    return false;
}

/**
 * Absolute Addressing Mode.
 */
bool Cpu::addr_abs()
{
    _effective_addr = bus_read16(_context.pc);
    _effective_value = _bus.Read(_effective_addr);

    _context.pc += 2;
    return false;
}

/**
 * Absolute (X-indexed) Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_abs_x()
{
    uint16_t start_page = 0;

    _effective_addr = bus_read16(_context.pc);
    start_page = _effective_addr & 0xFF00;
    _effective_addr += _context.x;

    _effective_value = _bus.Read(_effective_addr);

    _context.pc += 2;

    return (start_page != (_effective_addr & 0xFF00));
}

/**
 * Absolute (Y-indexed) Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_abs_y()
{
    uint16_t start_page = 0;

    _effective_addr = bus_read16(_context.pc);
    start_page = _effective_addr & 0xFF00;
    _effective_addr += _context.y;

    _effective_value = _bus.Read(_effective_addr);

    _context.pc += 2;

    return (start_page != (_effective_addr & 0xFF00));
}

/**
 * Immediate Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_imm()
{
    _effective_addr = 0;
    _effective_value = _bus.Read(_context.pc++);

    return false;
}

/**
 * Implied Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_imp()
{
    _effective_addr = 0;
    _effective_value = 0;

    return false;
}

/**
 * Indirect Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_ind()
{
    uint16_t ea_lower = 0, ea_upper = 0;

    /**
     * Have to do all this fancy stuff to replicate the page-boundary wraparound
     * bug.
     *
     * Basically, if you do an indirect jump with a low byte of $FF then the
     * CPU will not increment the high byte (like it should). It will then read
     * from the wrong page.
     *
     * Example:
     * If you do JUMP ($0FFF), then the CPU will get it's effective address
     * from $0FFF and $0F00 (although it SHOULD get it from $0FFF and $1000)
     */
    ea_lower = bus_read16(_context.pc);
    ea_upper = (ea_lower & 0xFF00) | ((ea_lower + 1) & 0x00FF);
    _effective_addr = _bus.Read(ea_lower) | (_bus.Read(ea_upper) << 8);

    _effective_value = _bus.Read(_effective_addr);

    _context.pc += 2;

    return false;
}

/**
 * Indirect (X-indexed) Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_x_ind()
{
    _effective_addr = (_bus.Read(_context.pc++) + _context.x) & 0xFF;
    _effective_addr = _bus.Read(_effective_addr) |
                      (_bus.Read((_effective_addr + 1) & 0xFF) << 8);

    _effective_value = _bus.Read(_effective_addr);

    return false;
}

/**
 * Indirect (Y-indexed) Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_ind_y()
{
    uint16_t start_page = 0;

    _effective_addr = _bus.Read(_context.pc++);
    _effective_addr = _bus.Read(_effective_addr) |
                      (_bus.Read((_effective_addr + 1) & 0xFF) << 8);

    start_page = _effective_addr & 0xFF00;

    _effective_addr += _context.y;

    _effective_value = _bus.Read(_effective_addr);

    return (start_page != (_effective_addr & 0xFF00));
}

/**
 * Relative Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_rel()
{
    _effective_value = _bus.Read(_context.pc++);

    _effective_addr = 0;

    return false;
}

/**
 * Zero-page Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_zpg()
{
    _effective_addr = _bus.Read(_context.pc++);
    _effective_value = _bus.Read(_effective_addr);

    return false;
}

/**
 * Zero-page (X-indexed) Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_zpg_x()
{
    _effective_addr = (_bus.Read(_context.pc++) + _context.x) & 0xFF;
    _effective_value = _bus.Read(_effective_addr);

    return false;
}

/**
 * Zero-page (Y-indexed) Addressing Mode.
 *
 * @return True if effective address passed over a page boundary.
 */
bool Cpu::addr_zpg_y()
{
    _effective_addr = (_bus.Read(_context.pc++) + _context.y) & 0xFF;
    _effective_value = _bus.Read(_effective_addr);

    return false;
}

/*******************************************************************************
                                 INSTRUCTIONS
 ******************************************************************************/

/**
 * Add with Carry.
 */
void Cpu::instr_adc()
{
    uint16_t result = _context.acc + _effective_value + get_flag(FLAG_CARRY);

    update_carry(result);
    update_zero(result);
    update_overflow(result);
    update_negative(result);

    /**
     * Handle decimal mode addition.
     */
    if(get_flag(FLAG_DECIMAL)) {
        set_flag(FLAG_CARRY, 0);

        if((result & 0xF) > 0x9)
            result += 6;

        if((result & 0xF0) > 0x90) {
            result += 0x60;
            set_flag(FLAG_CARRY, 1);
        }

        /**
         * Decimal mode adds a cycle.
         */
        _total_cycles++;
    }

    _context.acc = result;
}

/**
 * AND logical operation.
 */
void Cpu::instr_and()
{
    uint16_t result = _context.acc & _effective_value;

    update_zero(result);
    update_negative(result);

    _context.acc = result;
}

/**
 * Arithmatic Shift Left.
 */
void Cpu::instr_asl()
{
    uint16_t result = _effective_value << 1;

    update_carry(result);
    update_zero(result);
    update_negative(result);

    save_result(result);
}

/**
 * Branch if Carry Clear.
 */
void Cpu::instr_bcc()
{
    do_branch(FLAG_CARRY, 0);
}

/**
 * Branch if Carry Set.
 */
void Cpu::instr_bcs()
{
    do_branch(FLAG_CARRY, FLAG_CARRY);
}

/**
 * Branch if Equal to Zero.
 */
void Cpu::instr_beq()
{
    do_branch(FLAG_ZERO, FLAG_ZERO);
}

/**
 * Memory Bit Test.
 */
void Cpu::instr_bit()
{
    uint16_t result = _context.acc & _effective_value;

    update_zero(result);
    set_flag(FLAG_OVERFLOW, (_effective_value & FLAG_OVERFLOW));
    set_flag(FLAG_NEGATIVE, (_effective_value & FLAG_NEGATIVE));
}

/**
 * Branch on Minus (negative).
 */
void Cpu::instr_bmi()
{
    do_branch(FLAG_NEGATIVE, FLAG_NEGATIVE);
}

/**
 * Branch on Not Equal to Zero.
 */
void Cpu::instr_bne()
{
    do_branch(FLAG_ZERO, 0);
}

/**
 * Branch on Plus (positive).
 */
void Cpu::instr_bpl()
{
    do_branch(FLAG_NEGATIVE, 0);
}

/**
 * Break (software interrupt).
 */
void Cpu::instr_brk()
{
    _context.pc++;

    push16(_context.pc);
    push8(_context.sr | FLAG_BRK);

    set_flag(FLAG_IRQ, 1);

    _context.pc = bus_read16(0xFFFE);
}

/**
 * Branch on Overflow Clear.
 */
void Cpu::instr_bvc()
{
    do_branch(FLAG_OVERFLOW, 0);
}

/**
 * Branch on Overflow Set.
 */
void Cpu::instr_bvs()
{
    do_branch(FLAG_OVERFLOW, FLAG_OVERFLOW);
}

/**
 * Clear Carry.
 */
void Cpu::instr_clc()
{
    set_flag(FLAG_CARRY, 0);
}

/**
 * Clear Decimal.
 */
void Cpu::instr_cld()
{
    set_flag(FLAG_DECIMAL, 0);
}

/**
 * Clear Interrupt.
 */
void Cpu::instr_cli()
{
    set_flag(FLAG_IRQ, 0);
}

/**
 * Clear Overflow.
 */
void Cpu::instr_clv()
{
    set_flag(FLAG_OVERFLOW, 0);
}

/**
 * Compare with accumulator.
 */
void Cpu::instr_cmp()
{
    update_negative(_context.acc - _effective_value);

    if(_context.acc >= _effective_value)
        set_flag(FLAG_CARRY, 1);
    else
        set_flag(FLAG_CARRY, 0);

    if(_context.acc == _effective_value)
        set_flag(FLAG_ZERO, 1);
    else
        set_flag(FLAG_ZERO, 0);
}

/**
 * Compare with X-index.
 */
void Cpu::instr_cpx()
{
    update_negative(_context.x - _effective_value);

    if(_context.x >= _effective_value)
        set_flag(FLAG_CARRY, 1);
    else
        set_flag(FLAG_CARRY, 0);

    if(_context.x == _effective_value)
        set_flag(FLAG_ZERO, 1);
    else
        set_flag(FLAG_ZERO, 0);
}

/**
 * Compare with Y-index.
 */
void Cpu::instr_cpy()
{
    update_negative(_context.y - _effective_value);

    if(_context.y >= _effective_value)
        set_flag(FLAG_CARRY, 1);
    else
        set_flag(FLAG_CARRY, 0);

    if(_context.y == _effective_value)
        set_flag(FLAG_ZERO, 1);
    else
        set_flag(FLAG_ZERO, 0);
}

/**
 * Decrement Memory.
 */
void Cpu::instr_dec()
{
    uint16_t result = _effective_value - 1;

    update_zero(result);
    update_negative(result);

    save_result(result);
}

/**
 * Decrement Index X by One.
 */
void Cpu::instr_dex()
{
    _context.x--;

    update_zero(_context.x);
    update_negative(_context.x);
}

/**
 * Decrement Index Y by One.
 */
void Cpu::instr_dey()
{
    _context.y--;

    update_zero(_context.y);
    update_negative(_context.y);
}

/**
 * Exclusive-OR Memory with Accumulator.
 */
void Cpu::instr_eor()
{
    uint16_t result = _context.acc ^ _effective_value;

    update_zero(result);
    update_negative(result);

    _context.acc = result;
}

/**
 * Increment Memory by One.
 */
void Cpu::instr_inc()
{
    uint16_t result = _effective_value + 1;

    update_zero(result);
    update_negative(result);

    save_result(result);
}

/**
 * Increment Index X by One.
 */
void Cpu::instr_inx()
{
    _context.x++;

    update_zero(_context.x);
    update_negative(_context.x);
}

/**
 * Increment Index Y by One.
 */
void Cpu::instr_iny()
{
    _context.y++;

    update_zero(_context.y);
    update_negative(_context.y);
}

/**
 * Jump to New Location.
 */
void Cpu::instr_jmp()
{
    _context.pc = _effective_addr;
}

/**
 * Jump to New Location Saving Return Address.
 */
void Cpu::instr_jsr()
{
    push16(_context.pc - 1);
    _context.pc = _effective_addr;
}

/**
 * Load Accumulator with Memory.
 */
void Cpu::instr_lda()
{
    _context.acc = _effective_value;

    update_zero(_context.acc);
    update_negative(_context.acc);
}

/**
 * Load Index X with Memory.
 */
void Cpu::instr_ldx()
{
    _context.x = _effective_value;

    update_zero(_context.x);
    update_negative(_context.x);
}

/**
 * Load Index Y with Memory.
 */
void Cpu::instr_ldy()
{
    _context.y = _effective_value;

    update_zero(_context.y);
    update_negative(_context.y);
}

/**
 * Shift One Bit Right (Memory or Accumulator).
 */
void Cpu::instr_lsr()
{
    uint16_t result = _effective_value >> 1;

    set_flag(FLAG_CARRY, _effective_value & 1);
    update_zero(result);
    update_negative(result);

    save_result(result);
}

/**
 * No Operation.
 */
void Cpu::instr_nop()
{
    /**
     * This is a NOP, what do you think it does?
     */
}

/**
 * OR Memory with Accumulator.
 */
void Cpu::instr_ora()
{
    uint16_t result = _context.acc | _effective_value;

    update_zero(result);
    update_negative(result);

    _context.acc = result;
}

/**
 * Push Accumulator on Stack.
 */
void Cpu::instr_pha()
{
    push8(_context.acc);
}

/**
 * Push Processor Status on Stack.
 */
void Cpu::instr_php()
{
    push8(_context.sr | FLAG_BRK);
}

/**
 * Pull Accumulator from Stack.
 */
void Cpu::instr_pla()
{
    _context.acc = pull8();

    update_zero(_context.acc);
    update_negative(_context.acc);
}

/**
 * Pull Processor Status from Stack.
 */
void Cpu::instr_plp()
{
    _context.sr = pull8() | FLAG_UNUSED;
}

/**
 * Rotate One Bit Left (Memory or Accumulator).
 */
void Cpu::instr_rol()
{
    uint16_t result = (_effective_value << 1) | get_flag(FLAG_CARRY);

    update_carry(result);
    update_zero(result);
    update_negative(result);

    save_result(result);
}

/**
 * Rotate One Bit Right (Memory or Accumulator).
 */
void Cpu::instr_ror()
{
    uint16_t result = (_effective_value >> 1) | (get_flag(FLAG_CARRY) << 7);

    set_flag(FLAG_CARRY, _effective_value & 1);
    update_zero(result);
    update_negative(result);

    save_result(result);
}

/**
 * Return from Interrupt.
 */
void Cpu::instr_rti()
{
    _context.sr = pull8();
    _context.pc = pull16();
}

/**
 * Return from Subroutine.
 */
void Cpu::instr_rts()
{
    _context.pc = pull16() + 1;
}

/**
 * Subtract Memory from Accumulator with Borrow.
 */
void Cpu::instr_sbc()
{
    uint16_t result = _context.acc +
                     (_effective_value ^ 0xFF) +
                     get_flag(FLAG_CARRY);

    update_carry(result);
    update_zero(result);
    update_overflow(result);
    update_negative(result);

    /**
     * Handle decimal mode subtraction.
     */
    if(get_flag(FLAG_DECIMAL)) {
        set_flag(FLAG_CARRY, 0);

        result -= 0x66;
        if((result & 0xF) > 0x9)
            result += 6;

        if((result & 0xF0) > 0x90) {
            result += 0x60;
            set_flag(FLAG_CARRY, 1);
        }

        /**
         * Decimal mode adds a cycle.
         */
        _total_cycles++;
    }

    _context.acc = result;
}

/**
 * Set Carry Flag.
 */
void Cpu::instr_sec()
{
    set_flag(FLAG_CARRY, 1);
}

/**
 * Set Decimal Flag.
 */
void Cpu::instr_sed()
{
    set_flag(FLAG_DECIMAL, 1);
}

/**
 * Set Interrupt Disable Status.
 */
void Cpu::instr_sei()
{
    set_flag(FLAG_IRQ, 1);
}

/**
 * Store Accumulator in Memory.
 */
void Cpu::instr_sta()
{
    save_result(_context.acc);
}

/**
 * Store Index X in Memory.
 */
void Cpu::instr_stx()
{
    save_result(_context.x);
}

/**
 * Store Index Y in Memory.
 */
void Cpu::instr_sty()
{
    save_result(_context.y);
}

/**
 * Transfer Accumulator to Index X.
 */
void Cpu::instr_tax()
{
    _context.x = _context.acc;

    update_zero(_context.x);
    update_negative(_context.x);
}

/**
 * Transfer Accumulator to Index Y.
 */
void Cpu::instr_tay()
{
    _context.y = _context.acc;

    update_zero(_context.y);
    update_negative(_context.y);
}

/**
 * Transfer Stack Pointer to Index X.
 */
void Cpu::instr_tsx()
{
    _context.x = _context.sp;

    update_zero(_context.x);
    update_negative(_context.x);
}

/**
 * Transfer Index X to Accumulator.
 */
void Cpu::instr_txa()
{
    _context.acc = _context.x;

    update_zero(_context.acc);
    update_negative(_context.acc);
}

/**
 * Transfer Index X to Stack Pointer.
 */
void Cpu::instr_txs()
{
    _context.sp = _context.x;
}

/**
 * Transfer Index Y to Accumulator.
 */
void Cpu::instr_tya()
{
    _context.acc = _context.y;

    update_zero(_context.acc);
    update_negative(_context.acc);
}

/**
 * Undefined Instruction.
 */
void Cpu::instr_und()
{
    cout << "Undocumented Opcode used: 0x" << std::hex << _cur_opcode << endl;
}

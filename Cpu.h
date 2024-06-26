#ifndef CPU_H
#define CPU_H

#include "IState.h"
#include "SystemBus.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

/**
 * Helper macro for calling function pointers to member functions.
 */
#define CALL_MEMBER_FN(ptrToMember)  (this->*(ptrToMember))

/**
 * Entity representing a single CPU instruction.
 */
class Cpu;
struct CpuInstruction {
    /**
     * Three letter acronym for this instruction.
     */
    std::string acronym;

    /**
     * Function pointer to the method implementing the addressing mode.
     */
    bool (Cpu::*addr_mode)(void);

    /**
     * Function pointer to the method implementing the instruction's
     * functionality.
     */
    void (Cpu::*instr)(void);

    /**
     * True if passing a page boundary adds a cycle.
     */
    bool has_page_penalty;

    /**
     * Number of cycles this instruction takes (don't include the cycle added if
     * crossing a page boundary adds a cycle).
     */
    uint8_t cycles;

    /**
     * Size in bytes (including opcode).
     */
    uint8_t size;
};

/**
 * The excecution context for the CPU Core.
 */
struct CpuContext {
    /**
     * Program Counter.
     */
    uint16_t pc;

    /**
     * Accumulator Register.
     */
    uint8_t acc;

    /**
     * X-Index Register.
     */
    uint8_t x;

    /**
     * Y-Index Register.
     */
    uint8_t y;

    /**
     * Stack Pointer Register.
     */
    uint8_t sp;

    /**
     * Status (flags) Regsiter.
     */
    uint8_t sr;
};

/**
 * Bit mask values for the CPU flags
 */
enum CpuFlag {
    FLAG_CARRY = 0x1,
    FLAG_ZERO = 0x2,
    FLAG_IRQ = 0x4,
    FLAG_DECIMAL = 0x8,
    FLAG_BRK = 0x10,
    FLAG_UNUSED = 0x20,
    FLAG_OVERFLOW = 0x40,
    FLAG_NEGATIVE = 0x80
};

/**
 * 6502 CPU Core.
 */
class Cpu : public IState
{
public:
    explicit Cpu(SystemBus &bus, std::vector<CpuInstruction> &opcodes);

    Cpu(const Cpu &copy) = delete;
    Cpu& operator=(const Cpu &rhs) = delete;

    void Reset();

    uint32_t Execute(uint32_t num_cycles);

    void SingleStep();

    uint32_t GetTotalCycles() const;

    CpuContext GetContext() const;

    uint16_t GetBpAddr() const;
    void SetBpAddr(uint16_t addr);

    bool GetBpEnabled() const;
    void SetBpEnabled(bool enabled);

    void SaveState(std::ofstream &output) override;
    void LoadState(std::ifstream &input) override;

private:
    uint16_t bus_read16(uint16_t addr) const;

    void save_result(uint16_t result);

    void do_branch(CpuFlag flag, uint8_t value);

    void push8(uint8_t value);
    void push16(uint16_t value);
    uint8_t pull8();
    uint16_t pull16();

    uint8_t get_flag(CpuFlag flag) const;
    void set_flag(CpuFlag flag, uint8_t value);

    void update_carry(uint16_t result);
    void update_zero(uint16_t result);
    void update_overflow(uint16_t result, uint8_t effective_value);
    void update_negative(uint16_t result);

public:
    bool addr_acc();
    bool addr_abs();
    bool addr_abs_x();
    bool addr_abs_y();
    bool addr_imm();
    bool addr_imp();
    bool addr_ind();
    bool addr_x_ind();
    bool addr_ind_y();
    bool addr_rel();
    bool addr_zpg();
    bool addr_zpg_x();
    bool addr_zpg_y();

    void instr_adc();
    void instr_and();
    void instr_asl();
    void instr_bcc();
    void instr_bcs();
    void instr_beq();
    void instr_bit();
    void instr_bmi();
    void instr_bne();
    void instr_bpl();
    void instr_brk();
    void instr_bvc();
    void instr_bvs();
    void instr_clc();
    void instr_cld();
    void instr_cli();
    void instr_clv();
    void instr_cmp();
    void instr_cpx();
    void instr_cpy();
    void instr_dec();
    void instr_dex();
    void instr_dey();
    void instr_eor();
    void instr_inc();
    void instr_inx();
    void instr_iny();
    void instr_jmp();
    void instr_jsr();
    void instr_lda();
    void instr_ldx();
    void instr_ldy();
    void instr_lsr();
    void instr_nop();
    void instr_ora();
    void instr_pha();
    void instr_php();
    void instr_pla();
    void instr_plp();
    void instr_rol();
    void instr_ror();
    void instr_rti();
    void instr_rts();
    void instr_sbc();
    void instr_sec();
    void instr_sed();
    void instr_sei();
    void instr_sta();
    void instr_stx();
    void instr_sty();
    void instr_tax();
    void instr_tay();
    void instr_tsx();
    void instr_txa();
    void instr_txs();
    void instr_tya();
    void instr_und();

private:
    /**
     * Base address of the stack.
     */
    static constexpr uint16_t _stack_base = 0x100;

    /**
     * Currently executing opcode.
     */
    uint8_t _cur_opcode = 0;

    /**
     * Number of instructions that have executed.
     */
    uint32_t _num_instr = 0;

    /**
     * Total number of cycles the processor has ran for.
     */
    uint32_t _total_cycles = 0;

    /**
     * Effective address generated by address mode.
     */
    uint16_t _effective_addr = 0;

    /**
     * 16-bit address at which to break on if the PC ever becomes this value.
     */
    int _bp_addr = 0;

    /**
     * True if the breakpoint is enabled, false otherwise.
     */
    bool _bp_enabled = false;

    /**
     * Execution Context.
     */
    CpuContext _context { 0, 0, 0, 0, 0xFD, 0x20 };

    /**
     * System bus to perform read/write cycles on.
     */
    SystemBus &_bus;

    /**
     * CPU Instruction Table (256 total instructions).
     */
    std::vector<CpuInstruction> &_opcodes;
};

#endif // CPU_H

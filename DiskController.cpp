/**
 * Handles reading/writing to disks. The disk ROM that handles booting up a DOS
 * disk is in the DiskRom module.
 *
 * This class keeps track of the state of the stepper motor used to change
 * tracks, and motor position (used to figure out which bit to read on the
 * current track).
 *
 * This disk controller only supports reading disks.
 */
#include "DiskController.h"

/**
 * Constructor.
 *
 * @param cpu Reference to the CPU so cycle counts can be detected.
 */
DiskController::DiskController(Cpu &cpu)
{

}

/**
 * Read data from the disk or toggle a switch.
 *
 * @param addr The address to read.
 * @param no_side_fx True if this read shouldn't cause any side effects
 *                   (used by the memory view and disassembly).
 *
 * @return Data register (if address is even) or zero.
 */
uint8_t DiskController::Read(uint16_t addr, bool no_side_fx)
{
    if(!no_side_fx)
        toggle_switch(addr);

    /**
     * UPDATE THE DATA REGISTER.
     *
     * Figure out cycle delta, and move that number of bits.
     * Hold for 7 cycles after each byte (MSB is 1)
     */

    /**
     * A read to any even address will dump the data register onto the data bus.
     */
    return (addr & 1) ? 0 : _data_reg;
}

/**
 * Write data to the disk or toggle a switch.
 *
 * @param addr The address to write to.
 * @param data The data to write.
 */
void DiskController::Write(uint16_t addr, uint8_t data)
{

}

void DiskController::perform_read_write()
{
    // Calculate cycle/bit delta
    // Update cur_bit

    // run all bits except the last
    //    set data register to WP, or shift in one bit/set hold for 7 cycles

    // toggle switches
    //     READ_ON = clear data register

    // run last bit
}

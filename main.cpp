#include <iostream>
#include <QCoreApplication>

#include "cpu.h"
#include "instrs_6502.h"
#include "memory.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Memory mem(65536);

    /**
     * Run test.bin at 0x4000, address 0x0210 should be 0xFF for passing.
     *
     * Run ehbasic.bin at 0xC000.
     */

    if(mem.LoadExecutable("asm/test.bin", 0x4000)) {
        Cpu cpu(mem, instrs_6502);
        cpu.Execute(100000000);

        unsigned int value = mem.Read(0x210);
        cout << "Value: 0x" << std::hex << value << endl;
    }

    return a.exec();
}

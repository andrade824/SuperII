/**
 * Provides the main access point between all of the components in the emulated
 * system. When the processor makes a read/write request, that request is
 * sent to this module to determine which component needs to satisfy that
 * request.
 *
 * If the CPU tries to read from an address with no associated component, it
 * will read 0xFF. Writes to dummy addresses have no effect.
 */

#include "SystemBus.h"

SystemBus::SystemBus()
{

}

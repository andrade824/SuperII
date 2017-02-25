/**
 * Any classes that inherit from this class support saving and loading their
 * state at runtime.
 */
#ifndef ISTATE_H
#define ISTATE_H

#include <fstream>

/**
 * Standard interface for modules that provide a way to read/write memory mapped
 * values.
 */
class IState
{
public:
    /**
     * Write any state out to a file.
     *
     * @param output The file to write to.
     */
    virtual void SaveState(std::ofstream &output) = 0;

    /**
     * Load any state from a file.
     *
     * @param input The file to read from.
     */
    virtual void LoadState(std::ifstream &input) = 0;

    /**
     * Required for polymorphism.
     */
    virtual ~IState() {}
};

#endif // ISTATE_H

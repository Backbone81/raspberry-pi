#pragma once

#include <cstdint>

/*
 * This class provides access to the Broadcom Serial Controller (BSC) which is compliant
 * with the I2C bus/interface.
 */
class CBsc
{
public:
    /*
     * No need for copying objects of this class
     */
    CBsc(const CBsc&) = delete;
    CBsc& operator=(const CBsc&) = delete;

public:
    /*
     * Constructor
     * Provide the physical memory address to the BSC as address.
     */
    explicit CBsc(void* Base);

    /*
     * Reads Size bytes of data from the module at ModuleAddress out of its RegisterAddress and places that data into Data.
     */
    void Read(uint8_t ModuleAddress, uint8_t RegisterAddress, void* Data, std::size_t Size);

    /*
     * Writes Size bytes of data from Data to the module at ModuleAddress into its RegisterAddress.
     */
    void Write(uint8_t ModuleAddress, uint8_t RegisterAddress, const void* Data, std::size_t Size);

private:
    /*
     * This method resets the status register for a new transaction.
     */
    void ClearStatus();

    /*
     * This method set the length of data we are about to read or write.
     * This method will thor an exception if the maximum supported data length is exceeded.
     */
    void SetDataLength(std::size_t Length);

    /*
     * Write data to the fifo register byte by byte. Respects the state information
     * about the FIFO being full. Will wait with writing data to the fifo until it has
     * space free.
     */
    void WriteFifo(const void* Data, std::size_t Size);

    /*
     * Read data from the fifo register byte by byte. Respects the state information
     * about the FIFO being empty. Will wait with reading data from the FIFO until it
     * has data available.
     */
    void ReadFifo(void* Data, std::size_t Size);

    /*
     * Signal a write transaction to start
     */
    void StartWriteTransfer();

    /*
     * Signal a read transaction to start
     */
    void StartReadTransfer();

    /*
     * Check if the FIFO has data ready to be read
     */
    bool IsReadPossible() const;

    /*
     * Blocks until the FIFO has data ready for reading
     * Will throw an exception if the timeout is reached.
     */
    void WaitForReadPossible() const;

    /*
     * Checks if the FIFO has free space to write to
     */
    bool IsWritePossible() const;

    /*
     * Blocks until the FIFO has free space to write to
     * Will throw an exception if the timeout is reached.
     */
    void WaitForWritePossible() const;

    /*
     * Check if the status register signals the end of the current transaction.
     */
    bool IsDone() const;

    /*
     * Blocks until the status register signals the end of the current transaction.
     * Will throw an exception if the timeout is reached.
     */
    void WaitUntilDone() const;

private:
    volatile uint32_t* m_Register; /* the base memory address */

    volatile uint32_t* m_Control;
    volatile uint32_t* m_Status;
    volatile uint32_t* m_DataLength;
    volatile uint32_t* m_SlaveAddress;
    volatile uint32_t* m_DataFifo;

}; // class CBsc

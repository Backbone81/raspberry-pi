#pragma once

/*
 * This is a helper class for accessing physical memory.
 *
 * NOTE: This class works by opening /dev/mem which can only be accessed by root.
 *       You have to run this application with root privileges by using sudo.
 */
class CPhysicalMemory
{
public:
    /*
     * No need for copying objects of this class
     */
    CPhysicalMemory(const CPhysicalMemory&) = delete;
    CPhysicalMemory& operator=(const CPhysicalMemory&) = delete;

public:
    /*
     * Constructor
     * Opens physical memory as a file descriptor for reading and writing.
     */
    CPhysicalMemory();

    /*
     * Destructor
     * Closes the file descriptor for the physical memory.
     */
    ~CPhysicalMemory();

    /*
     * Returns the file handle for accessing the physical memory.
     */
    int GetHandle() const;

private:
    int m_Handle; /* file handle */

}; // class CPhysicalMemory

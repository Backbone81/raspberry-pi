#pragma once

#include <cstddef>

/*
 * This class maps contents of a file into the address space of the current process. That way you can easily access
 * Individual bytes without having to constantly seek around the file.
 */
class CMemoryMapper
{
public:
    /*
     * No need for copying objects of this class
     */
    CMemoryMapper(const CMemoryMapper&) = delete;
    CMemoryMapper& operator=(const CMemoryMapper&) = delete;

public:
    /*
     * Constructor
     * Provide an open file handle, an offset from the start of the file and the size in bytes as parameters.
     */
    CMemoryMapper(int FileHandle, std::size_t Offset, std::size_t Size);

    /*
     * Destructor
     * Closes the memory mapping.
     */
    ~CMemoryMapper();

    /*
     * Returns the starting address of the memory mapped file.
     */
    void* GetHandle() const;

private:
    const std::size_t m_Size; /* number of bytes requested in the constructor*/
    void* m_Handle; /* the base address where the file is mapped in the memory */

};

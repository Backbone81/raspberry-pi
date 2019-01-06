#include "CPhysicalMemory.hpp"

#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

CPhysicalMemory::CPhysicalMemory()
{
    m_Handle = open("/dev/mem", O_RDWR | O_SYNC);
    if (m_Handle == -1)
    {
        throw std::runtime_error("Unable to open /dev/mem for read and write access. Check permissions.");
    }
}

CPhysicalMemory::~CPhysicalMemory()
{
    close(m_Handle);
}

int CPhysicalMemory::GetHandle() const
{
    return m_Handle;
}

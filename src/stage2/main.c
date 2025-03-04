#include "constants.h"
#include "io.h"
#include "ports.h"

// See kernel/sections for more details
#define FS_SECTOR ((int*) (KERNEL_OFFSET + 16))

// The number of sectors before the kernel
// Defined in sections.asm
extern short KERNEL_SECTOR_BEGIN;

void loadKernel()
{
    int loadedSectors = 0;
    int *loadDestination = (int*)KERNEL_OFFSET;

    do
    {
        // Load a sector
        readDisk(KERNEL_SECTOR_BEGIN + loadedSectors, loadDestination);

        ++loadedSectors;

        if (loadedSectors > KERNEL_MAX_SIZE)
            fatal("Boot: Kernel is damaged. The size is above the preferred maximum.");

        // Set the location for next sector
        loadDestination += 512 / sizeof(int);

    // Load until the magic number
    } while (loadDestination[-1] != END_OF_KERNEL);

    // Set the last read sector
    *FS_SECTOR = KERNEL_SECTOR_BEGIN + loadedSectors;
}

void main()
{
    // Clear screen
    flush();

    // Check for disk IO errors
    identifyDisk();

    // Load kernel from disk to memory
    loadKernel();

    // Jump to kernel
    goto *(void*)KERNEL_OFFSET;
}

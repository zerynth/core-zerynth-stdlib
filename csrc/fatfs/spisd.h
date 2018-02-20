
#ifndef _SPISD_DEFINED
#define _SPISD_DEFINED

#include "zerynth.h"
#include "diskio.h"
#include "vbl.h"

DSTATUS spi_disk_initialize (uint32_t spi, vhalSpiConf *conf);
DSTATUS spi_disk_status (uint32_t spi);
DRESULT spi_disk_read (uint32_t spi, BYTE* buff, DWORD sector, UINT count);
DRESULT spi_disk_write (uint32_t spi, const BYTE* buff, DWORD sector, UINT count);
DRESULT spi_disk_ioctl (uint32_t spi, BYTE cmd, void* buff);

#endif

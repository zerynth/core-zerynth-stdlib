/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "zerynth.h"
#include "diskio.h"		/* FatFs lower layer API */
#include "spisd.h"
#include "vbl.h"

//#define printf(...) vbl_printf_stdout(__VA_ARGS__)

/* Definitions of physical drive number for each drive */
#define SPISD	0	/* Example: Map ATA harddisk to physical drive 0 */
#define SDIO    1



PObject *disks_dict = NULL;

C_NATIVE(__update_disks_dict) {
    NATIVE_UNWARN();
    if (disks_dict == NULL) {
        disks_dict = pdict_new(4);
        pdict_put(disks_dict, args[0], args[1]);
    }
    else {
        pdict_put(disks_dict, args[0], args[1]);
    }

    *res = disks_dict;

    return ERR_OK;
}

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    PObject *disk_args = pdict_get(disks_dict, PSMALLINT_NEW(pdrv));
    uint32_t disk_type = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 0));


    switch (disk_type) {
        case SPISD:
            ;
            uint32_t spi_drv = (PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff);
            return spi_disk_status(spi_drv);
#if defined(VHAL_CUSTOM_SDIO) && VHAL_CUSTOM_SDIO
        case SDIO:
            ;
            uint32_t sdio_drv = (PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff);
            return sdio_disk_status(sdio_drv);
#endif
    }
    return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    PObject *disk_args = pdict_get(disks_dict, PSMALLINT_NEW(pdrv));
    uint32_t disk_type = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 0));
    SpiPins *spipins = ((SpiPins*)_vm_pin_map(PRPH_SPI));

    switch (disk_type) {
        case SPISD:
            ;
            uint32_t spi_drv = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff;
            vhalSpiConf conf;
            conf.nss = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 2));
            conf.clock = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 3));
            conf.mode = 0;
            conf.bits = 0;
            conf.master = 1;
            conf.mosi = spipins[spi_drv].mosi;
            conf.miso = spipins[spi_drv].miso;
            conf.sclk = spipins[spi_drv].sclk;

            return spi_disk_initialize(spi_drv, &conf);
#if defined(VHAL_CUSTOM_SDIO) && VHAL_CUSTOM_SDIO
        case SDIO:
            ;
            uint32_t sdio_drv = (PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff);
            uint32_t sdio_bits = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 2));
            uint32_t sdio_freq = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 3));
            return sdio_disk_initialize(sdio_drv, sdio_bits, sdio_freq);
#endif
    }

	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{

    PObject *disk_args = pdict_get(disks_dict, PSMALLINT_NEW(pdrv));
    uint32_t disk_type = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 0));

    switch (disk_type) {
        case SPISD:
            ;
            uint32_t spi_drv = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff;
            return spi_disk_read(spi_drv, buff, sector, count);
#if defined(VHAL_CUSTOM_SDIO) && VHAL_CUSTOM_SDIO
        case SDIO:
            ;
            uint32_t sdio_drv = (PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff);
            return sdio_disk_read(sdio_drv, buff, sector, count);
#endif
    }

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{

    PObject *disk_args = pdict_get(disks_dict, PSMALLINT_NEW(pdrv));
    uint32_t disk_type = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 0));

    switch (disk_type) {
        case SPISD:
            ;
            uint32_t spi_drv = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff;
            return spi_disk_write(spi_drv, buff, sector, count);
#if defined(VHAL_CUSTOM_SDIO) && VHAL_CUSTOM_SDIO
        case SDIO:
            ;
            uint32_t sdio_drv = (PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff);
            return sdio_disk_write(sdio_drv, buff, sector, count);
#endif
    }

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    PObject *disk_args = pdict_get(disks_dict, PSMALLINT_NEW(pdrv));
    uint32_t disk_type = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 0));

    switch (disk_type) {
        case SPISD:
            ;
            uint32_t spi_drv = PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff;
            return spi_disk_ioctl(spi_drv, cmd, buff);
#if defined(VHAL_CUSTOM_SDIO) && VHAL_CUSTOM_SDIO
        case SDIO:
            ;
            uint32_t sdio_drv = (PSMALLINT_VALUE(PLIST_ITEM(disk_args, 1)) & 0xff);
            return sdio_disk_ioctl(sdio_drv, cmd, buff);
#endif
    }

	return RES_PARERR;
}

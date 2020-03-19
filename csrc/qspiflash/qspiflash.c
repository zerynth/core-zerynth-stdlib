#define ZERYNTH_PRINTF
#include "zerynth.h"

vhalQspiConf qspi_conf;
vhalQspiFlashConf flash_conf;


C_NATIVE(qspiflash_init) {
    NATIVE_UNWARN();

    uint32_t d0,d1,d2,d3,clk,cs,flash_size,block_size,subblock_size,sector_size,page_size;
    uint32_t dummy_cycles_read,dummy_cycles_read_dual,dummy_cycles_read_quad,dummy_cycles_2read,dummy_cycles_4read;
    uint32_t alt_bytes_pe_mode,alt_bytes_no_pe_mode,sr_wip,sr_wel,sr_bp,sr_srwd,sr1_qe,sr1_sus;

    if (parse_py_args("iiiiiiiiiiiiiiiiiiiiiiii", nargs, args, &d0, &d1, &d2,
                                                 &d3, &clk, &cs,
                                                 &flash_size, &block_size,
                                                 &subblock_size,
                                                 &sector_size, &page_size,
                                                 &dummy_cycles_read,
                                                 &dummy_cycles_read_dual,
                                                 &dummy_cycles_read_quad,
                                                 &dummy_cycles_2read,
                                                 &dummy_cycles_4read,
                                                 &alt_bytes_pe_mode,
                                                 &alt_bytes_no_pe_mode,
                                                 &sr_wip, &sr_wel, &sr_bp,
                                                 &sr_srwd, &sr1_qe, &sr1_sus) != 24)
        return ERR_TYPE_EXC;

    qspi_conf.d0 = d0;
    qspi_conf.d1 = d1;
    qspi_conf.d2 = d2;
    qspi_conf.d3 = d3;
    qspi_conf.clk = clk;
    qspi_conf.cs = cs;
    flash_conf.flash_size = flash_size;
    flash_conf.block_size = block_size;
    flash_conf.subblock_size = subblock_size;
    flash_conf.sector_size = sector_size;
    flash_conf.page_size = page_size;
    flash_conf.dummy_cycles_read = dummy_cycles_read;
    flash_conf.dummy_cycles_read_dual = dummy_cycles_read_dual;
    flash_conf.dummy_cycles_read_quad = dummy_cycles_read_quad;
    flash_conf.dummy_cycles_2read = dummy_cycles_2read;
    flash_conf.dummy_cycles_4read = dummy_cycles_4read;
    flash_conf.alt_bytes_pe_mode = alt_bytes_pe_mode;
    flash_conf.alt_bytes_no_pe_mode = alt_bytes_no_pe_mode;
    flash_conf.sr_wip = sr_wip;
    flash_conf.sr_wel = sr_wel;
    flash_conf.sr_bp = sr_bp;
    flash_conf.sr_srwd = sr_srwd;
    flash_conf.sr1_qe = sr1_qe;
    flash_conf.sr1_sus = sr1_sus;

    if (vhalQspiFlashInit(0, &qspi_conf, &flash_conf) != VHAL_OK) {
        return ERR_PERIPHERAL_ERROR_EXC;
    }
    return ERR_OK;
}

C_NATIVE(qspiflash_get_geometry) {
    NATIVE_UNWARN();

    PTuple* tpl = ptuple_new(5,NULL);
    PTUPLE_SET_ITEM(tpl,0,PSMALLINT_NEW(flash_conf.flash_size));
    PTUPLE_SET_ITEM(tpl,1,PSMALLINT_NEW(flash_conf.block_size));
    PTUPLE_SET_ITEM(tpl,2,PSMALLINT_NEW(flash_conf.subblock_size));
    PTUPLE_SET_ITEM(tpl,3,PSMALLINT_NEW(flash_conf.sector_size));
    PTUPLE_SET_ITEM(tpl,4,PSMALLINT_NEW(flash_conf.page_size));
    *res = tpl;
    return ERR_OK;
}

C_NATIVE(qspiflash_done) {
    NATIVE_UNWARN();

    /* Call the DeInit function to reset the driver */
    if (vhalQspiFlashDone(0) != VHAL_OK) {
        return ERR_PERIPHERAL_ERROR_EXC;
    }
    return ERR_OK;
}

C_NATIVE(qspiflash_read_data) {
    NATIVE_UNWARN();

    uint32_t readaddr, toread_len;

    if (parse_py_args("ii", nargs, args, &readaddr, &toread_len) != 2) 
        return ERR_TYPE_EXC;

    *res = pbytes_new(toread_len, NULL);
    if (vhalQspiFlashRead(0, readaddr, PSEQUENCE_BYTES(*res), toread_len) != VHAL_OK) {
        return ERR_PERIPHERAL_ERROR_EXC;
    };

    return ERR_OK;
}

C_NATIVE(qspiflash_write_data) {
    NATIVE_UNWARN();

    uint32_t end_addr, current_size, current_addr;
    uint32_t writeaddr;
    uint32_t len;
    uint8_t *tosend; 

    if (parse_py_args("is", nargs, args, &writeaddr, &tosend, &len) != 2) 
        return ERR_TYPE_EXC;


    /* Calculation of the size between the write address and the end of the page */
    current_size = flash_conf.page_size - (writeaddr % flash_conf.page_size);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > len) {
        current_size = len;
    }

    /* Initialize the address variables */
    current_addr = writeaddr;
    end_addr = writeaddr + len;

    /* Perform the write page by page */
    do {

        if (vhalQspiFlashWrite(0, current_addr, tosend, current_size) != VHAL_OK) {
            return ERR_PERIPHERAL_ERROR_EXC;
        }

        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        tosend += current_size;
        current_size = ((current_addr + flash_conf.page_size) > end_addr) ? (end_addr - current_addr) : flash_conf.page_size;

    } while (current_addr < end_addr);

    return ERR_OK;
}

C_NATIVE(qspiflash_erase_block) {
    NATIVE_UNWARN();

    uint32_t blockaddr;

    if (parse_py_args("i", nargs, args, &blockaddr) != 1)
        return ERR_TYPE_EXC;

    if (blockaddr >= flash_conf.flash_size) {
        return ERR_VALUE_EXC;
    }

    if (vhalQspiFlashEraseBlock(0, blockaddr, 1) != VHAL_OK) {
        return ERR_PERIPHERAL_ERROR_EXC;
    }

    return ERR_OK;
}

C_NATIVE(qspiflash_erase_sector) {
    NATIVE_UNWARN();

    uint32_t sectoraddr;

    if (parse_py_args("i", nargs, args, &sectoraddr) != 1)
        return ERR_TYPE_EXC;

    if (sectoraddr >= flash_conf.flash_size) {
        return ERR_VALUE_EXC;
    }

    if (vhalQspiFlashEraseSector(0, sectoraddr, 1) != VHAL_OK) {
        return ERR_PERIPHERAL_ERROR_EXC;
    }
    return ERR_OK;
}

C_NATIVE(qspiflash_erase_chip) {
    NATIVE_UNWARN();

    if (vhalQspiFlashEraseChip(0, 1) != VHAL_OK) {
        return ERR_PERIPHERAL_ERROR_EXC;
    }

    return ERR_OK;
}

C_NATIVE(qspiflash_sleep) {
    NATIVE_UNWARN();

    if (vhalQspiFlashSleep(0) != VHAL_OK) {
        return ERR_PERIPHERAL_ERROR_EXC;
    }

    return ERR_OK;
}

C_NATIVE(qspiflash_wakeup) {
    NATIVE_UNWARN();

    if (vhalQspiFlashWakeup(0) != VHAL_OK) {
        return ERR_PERIPHERAL_ERROR_EXC;
    }

    return ERR_OK;
}

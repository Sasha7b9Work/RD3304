#include "config.h"
#include "hrd_Iap.h"
#include <gd32e23x.h>

//#define FREQ_CCLK  48000000

/* Delay definition */
#define EraseTimeout             ((u32)0x00000FFF)
#define ProgramTimeout           ((u32)0x0000000F)


/* FLASH Keys */
#define RDP_Key                  ((u16)0x00A5)
#define FLASH_KEY1               ((u32)0x45670123)
#define FLASH_KEY2               ((u32)0xCDEF89AB)

static void Unlock();
static fmc_state_enum WordProgram(uint address, uint data);
static fmc_state_enum ReadyWait(uint timeout);
static fmc_state_enum StateGet();
static fmc_state_enum PageErase(uint page_address);

/* erase flash sectors from start_addr to end_addr */
// прерывания должны быть зарпрещены заранее
void hrd_Iap_EraseSectors()
{
    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);

    for (int i = 0; i < 56; i++)
    {
        fmc_page_erase(MAIN_PROG_FIRST_ADDR + i * 1024);

        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
    }

    fmc_lock();

    uint *pointer = (uint *)MAIN_PROG_FIRST_ADDR;

    for (int i = 0; i < 32; i++)
    {
        if (*pointer++ != (uint)-1)
        {
            break;
        }
    }
}

/* writes down flash sector                         *
 *   flash_addr - address to begin write            *
 *   ram_addr   - source data address               *
 *   len        - data size (256, 512, 1024, 4096)  */
 // прерывания должны быть зарпрещены заранее
uint8_t hrd_Iap_FlashWrite(uint32_t flash_addr, uint32_t ram_addr, uint32_t len)
{
    Unlock();

    uint *data = (uint *)ram_addr;

    for (uint i = 0; i < len; i += 4)
    {
        WordProgram(flash_addr, *data);

        flash_addr += 4;

        data++;

        FMC_STAT = FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR;
    }

    FMC_CTL |= FMC_CTL_LK;

    return 0;
}


static void Unlock()
{
    if ((RESET != (FMC_CTL & FMC_CTL_LK)))
    {
        /* write the FMC key */
        FMC_KEY = UNLOCK_KEY0;
        FMC_KEY = UNLOCK_KEY1;
    }
}


static fmc_state_enum WordProgram(uint address, uint data)
{
    fmc_state_enum fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

    if (FMC_READY == fmc_state) {
        /* set the PG bit to start program */
        FMC_CTL |= FMC_CTL_PG;

        REG32(address) = data;
        /* wait for the FMC ready */
        fmc_state = ReadyWait(FMC_TIMEOUT_COUNT);

        /* reset the PG bit */
        FMC_CTL &= ~FMC_CTL_PG;
    }

    /* return the FMC state */
    return fmc_state;
}


static fmc_state_enum ReadyWait(uint timeout)
{
    fmc_state_enum fmc_state = FMC_BUSY;

    /* wait for FMC ready */
    do {
        /* get FMC state */
        fmc_state = StateGet();
        timeout--;
    } while ((FMC_BUSY == fmc_state) && (0U != timeout));

    if (FMC_BUSY == fmc_state) {
        fmc_state = FMC_TOERR;
    }
    /* return the FMC state */
    return fmc_state;
}


static fmc_state_enum StateGet()
{
    fmc_state_enum fmc_state = FMC_READY;

    if ((uint)0x00U != (FMC_STAT & FMC_STAT_BUSY)) {
        fmc_state = FMC_BUSY;
    }
    else {
        if ((uint)0x00U != (FMC_STAT & FMC_STAT_WPERR)) {
            fmc_state = FMC_WPERR;
        }
        else {
            if ((uint)0x00U != (FMC_STAT & FMC_STAT_PGERR)) {
                fmc_state = FMC_PGERR;
            }
        }
    }
    /* return the FMC state */
    return fmc_state;
}

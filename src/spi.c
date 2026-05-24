#include "spi.h"

#include "hardware/spi.h"

#include "display.h"
#include "types.h"

ExitCode_t SPI_PortInit() {
    
    // spi display port init
    spi_init(DISPLAY_SPI_PORT, 20 * 1000 * 1000); // 40 MHz
    spi_set_format(DISPLAY_SPI_PORT, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    return SUCCESS;
}
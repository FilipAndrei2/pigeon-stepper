#include "pigeon_stepper.h"

#include "test_funcs.h"

#include "pico/stdlib.h"

#include "buzzer.h"
#include "display.h"
#include "gya.h"
#include "types.h"
#include "utils.h"

ExitCode_t Test_DisplayTest(void) {
    for (int i = 0; i < DISPLAY_RESOLUTION_WIDTH * DISPLAY_RESOLUTION_HEIGHT; i++) {
        ((uint16_t*)g_displayBuffer)[i] = 0xF800; // Rosu
    }
    
    // SETEAZA ZONA DE DESENARE INAINTE DE A TRIMITE BUFFERUL
    Display_SetWindow(0, 0, DISPLAY_RESOLUTION_WIDTH - 1, DISPLAY_RESOLUTION_HEIGHT - 1);

    if (FAIL == Display_SendBuffer(g_displayBuffer, sizeof(g_displayBuffer))) {
        LOG("Nu s-a putut desena");
        return FAIL;
    }

    return SUCCESS;
}

/// @brief Achizitioneaza acceleratia, unghiul si temperatura si le afiseaza la stdout la un interval de 1.5 secunde
/// @param  
/// @return 
ExitCode_t Test_Gya(void) {
    int16_t acceleratie[3], giroscop[3], temperatura;
    GYA_ReadRaw(acceleratie, giroscop, &temperatura);

    VLOG("Acceleratie: %d %d %d\n ", acceleratie[0], acceleratie[1], acceleratie[2]);
    VLOG("Giroscop: %d %d %d\n ", giroscop[0], giroscop[1], giroscop[2]);
    VLOG("Temperatura: %d\n", temperatura);

    sleep_ms(1500);
    return SUCCESS;
}

#include <stdio.h>

#include "student_code.h"
#include "memory_management_unit.h"

void write_string(MMU m, VirtualAddress va, char* str) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        write_byte(m, va + i, str[i]);
    }
    write_byte(m, va + i, '\0');
}

int main() {
    // Create a new MMU
    MMU m = MMU__pagetable__init();

    for (VPN vpn = 0; vpn < NUM_PAGES; vpn++) {
        if (vpn == NUM_PAGES - 1) {
            m.map_page(&m, vpn, true, true, true);
        }
        else {
            m.map_page(&m, vpn, vpn % 2 == 0, vpn % 3 == 0, vpn % 5 == 0);
        }
    }

    VirtualAddress va = ((0b0 << NUM_VPN_BITS) << NUM_OFFSET_BITS) | 0b0;

    char* str;

    str = "hello";
    write_string(m, va, str);

    return 0;
}

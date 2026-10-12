#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "student_code.h"



bool is_entry_valid(PageTableEntry pte) {
	return is_bit_set(pte, VALID_BIT);
}

bool is_read_enabled(PageTableEntry pte) {
	return is_bit_set(pte, READ_BIT);
}

bool is_write_enabled(PageTableEntry pte) {
	return is_bit_set(pte, WRITE_BIT);
}

bool is_execute_enabled(PageTableEntry pte) {
	return is_bit_set(pte, EXEC_BIT);
}

VPN get_vpn_from_va(VirtualAddress va) {
	return (va >> (NUM_OFFSET_BITS));
}

PFN find_free_page(MMU m) {
	for (int i = 0; i < NUM_FRAMES; i++) {
		if (!m.page_used[i]) {
			return i;
		}
	}
	return -1;
}

PFN convert_PageTableEntry_to_PFN(PageTableEntry pte) {
	return (pte & (get_mask(NUM_PFN_BITS)));
}


/*
 * Page Table Functions
 */
PageTableEntry get_pagetableentry__MMU_pagetable(MMU m, VPN vpn) {
	return m.page_pointer[vpn];
}

PFN map_page__MMU_pagetable(MMU* m, VPN vpn, bool can_read, bool can_write, bool can_exec) {
	PFN newpage = find_free_page(*m);

	PageTableEntry pte = get_mask_single_bit(VALID_BIT);
	
	if (can_read) {
		pte = pte | get_mask_single_bit(READ_BIT);
	}

	if (can_write) {
		pte = pte | get_mask_single_bit(WRITE_BIT);
	}

	if (can_exec) {
		pte = pte | get_mask_single_bit(EXEC_BIT);
	}

	pte = pte | newpage;

	m->page_pointer[vpn] = pte;
	m->page_used[newpage] = true;

	return newpage;

}


/*
 * Page Directory Functions
 */

Page* get_page(MMU m, VirtualAddress va, bool for_read, bool for_write, bool for_execute) {
	VPN vpn = get_vpn_from_va(va);

	PageTableEntry pte = get_pagetableentry__MMU_pagetable(m, vpn);

	if (is_entry_valid(pte)) {
		if (for_read && !is_read_enabled(pte)) {
			printf("Error: Read Permission!\n");
			return NULL;
		}
		if (for_write && !is_write_enabled(pte)) {
			printf("Error: Write Permission!\n");
			return NULL;
		}
		if (for_execute && !is_execute_enabled(pte)) {
			printf("Error: Exec Permission!\n");
			return NULL;
		}

		PFN pfn = convert_PageTableEntry_to_PFN(pte);

		return (Page*) &m.physical_memory[pfn];
	}
	else {
		printf("Error: entry not valid.\n");
		return NULL;
	}
}

char read_byte(MMU m, VirtualAddress va) {
	Page* read_addr = get_page(m, va, true, false, false);
	OFFSET offset = (OFFSET) (va & get_mask(NUM_OFFSET_BITS));
	return read_addr->data[offset];
}

void write_byte(MMU m, VirtualAddress va, char val) {
	Page* write_addr = get_page(m, va, false, true, false);
	OFFSET offset = (OFFSET) (va & get_mask(NUM_OFFSET_BITS));
	write_addr->data[offset] = val;
}

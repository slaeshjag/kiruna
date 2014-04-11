#include "LPC11xx.h"

/* Note to self: Acztronicz? */

int main(int ram, char **argv);
void system_reset();

extern int __stack_end;

void __startup() {
	extern int __data_init_start;
	extern int __data_start;
	extern int __data_end;
	extern int __bss_start;
	extern int __bss_end;
	int *cur, *target, *data;
	uint32_t ram;
	

	/* Clear .bss */
	cur = &__bss_start;
	target = &__bss_end;
	ram = target - cur;
	while (cur != target)
		*cur++ = 0;
	
	/* Initialize variables with compiler time defined content */
	data = &__data_init_start;
	cur = &__data_start;
	target = &__data_end;
	ram += target - cur;
	while (cur != target)
		*cur++ = *data++;
	
	/* Done setting everything up! */
	main(ram, 0);
	for (;;);

	return;
}


void *vectors[29] __attribute__((section(".vectors"))) = {
	&__stack_end,
	 __startup,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
	system_reset,
};

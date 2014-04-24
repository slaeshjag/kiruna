#include "LPC11xx.h"

/* Note to self: Acztronicz? */

int main(int ram, char **argv);
void system_reset();
void TIMER16_0_IRQHandler(void);

extern int __stack_end;

__attribute__((section(".after_vectors")))
void __startup() {
	extern int __data_init_start;
	extern int __data_start;
	extern int __data_end;
	extern int __bss_start;
	extern int __bss_end;
	int *cur, *target, *data;
	uint32_t ram;
	
	LPC_GPIO3->DIR |= 0x20;
	LPC_GPIO3->MASKED_ACCESS[0x20] = 0x0;
	

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


void arne() {
	return;
}

/*void *vectors[32] __attribute__((section(".vectors"))) = {
	&__stack_end,
	 __startup,
	0,
	system_reset,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};*/

extern void (* const __vectors[32])(void)  __attribute__ ((section(".vectors"))) = {
	(void (*)(void)) & __stack_end,	
	__startup,
	0,          /* NMI Handler */
	0,          /* Hard Fault Handler */
	0,          /* Reserved */
	0,          /* Reserved */
	0,          /* Reserved */
	0,          /* Reserved */
	0,          /* Reserved */
	0,          /* Reserved */
	0,          /* Reserved */
	0,          /* SVCall Handler */
	0,          /* Reserved */
	0,          /* Reserved */
	0,          /* PendSV Handler */
	0,          /* SysTick Handler */
	0,          /* IRQ0 */
	0,          /* IRQ1 */
	0,          /* IRQ2 */
	0,          /* IRQ3 */
	0,          /* IRQ4 */
	0,          /* IRQ5 */
	0,          /* IRQ6 */
	0,          /* IRQ7 */
	0,          /* IRQ8 */
	0,          /* IRQ9 */
	0,          /* IRQ10 */
	0,          /* IRQ11 */
	0,          /* IRQ12 */
	0,          /* IRQ13 */
	0,          /* IRQ14 - SPI1    */
	0,          /* IRQ15 - I2C     */
	arne,
	0,          /* IRQ16 - CT16B0  */
	0,          /* IRQ17 - CT16B1  */
	0,          /* IRQ18 - CT32B0  */
	0,          /* IRQ19 - CT32B1  */
	0,          /* IRQ20 - SPI0    */
	0           /* IRQ21 - UART    */

};


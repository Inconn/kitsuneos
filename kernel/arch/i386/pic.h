#ifndef ARCH_I386_PIC_H
#define ARCH_I386_PIC_H

#include <stdint.h>

void arch_pic_send_eoi(uint8_t irq);

void arch_pic_disable();

#endif

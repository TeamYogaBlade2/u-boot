#ifndef __MTK_TIMER_H__
#define __MTK_TIMER_H__
typedef unsigned long ulong;

#include <asm/arch/mt65xx_typedefs.h>

extern void gpt_busy_wait_us(U32 timeout_us);
extern void gpt_busy_wait_ms(U32 timeout_ms);

extern ulong get_timer(ulong base);
extern void mdelay(unsigned long msec);
extern void udelay(unsigned long usec);
extern void mtk_timer_init(void);

void gpt_one_shot_irq(unsigned int ms);
int gpt_irq_init(void);
void gpt_irq_ack(void);
U32 gpt4_get_current_tick (void);
bool gpt4_timeout_tick (U32 start_tick, U32 timeout_tick);
U32 gpt4_time2tick_us (U32 time_us);


#endif

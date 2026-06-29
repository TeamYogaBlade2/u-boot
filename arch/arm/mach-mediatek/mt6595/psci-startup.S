#include <linux/linkage.h>

.pushsection ._secure.text, "ax"
.arch_extension sec

ENTRY(v7_invalidate_l1)
       mov     r0, #0
       mcr     p15, 2, r0, c0, c0, 0
       mrc     p15, 1, r0, c0, c0, 0

       ldr     r1, =0x7fff
       and     r2, r1, r0, lsr #13

       ldr     r1, =0x3ff

       and     r3, r1, r0, lsr #3      @ NumWays - 1
       add     r2, r2, #1              @ NumSets

       and     r0, r0, #0x7
       add     r0, r0, #4      @ SetShift

       clz     r1, r3          @ WayShift
       add     r4, r3, #1      @ NumWays
1:     sub     r2, r2, #1      @ NumSets--
       mov     r3, r4          @ Temp = NumWays
2:     subs    r3, r3, #1      @ Temp--
       mov     r5, r3, lsl r1
       mov     r6, r2, lsl r0
       orr     r5, r5, r6      @ Reg = (Temp<<WayShift)|(NumSets<<SetShift)
       mcr     p15, 0, r5, c7, c6, 2
       bgt     2b
       cmp     r2, #0
       bgt     1b
       dsb
       isb
       mov     pc, lr
ENDPROC(v7_invalidate_l1)

ENTRY(mtk_psci_entry)
    @ from downstream
    bl      v7_invalidate_l1

    mrc     p15, 0, r0, c0, c0, 5           @ MPIDR
    ubfx    r0, r0, #8, #4                  @ Cluster Id
    cmp     r0, #0                          @ Cluster 1
    beq     cluster0

    mrc     p15, 1, r1, c9, c0, 2           @ L2CTLR of CA17
    ldr     r2, =0xFFFC0000                 @ Latency
    and     r1, r1, r2
    ldr     r2, =0x00009251
    orr     r1, r1, r2
    mcr     p15, 1, r1, c9, c0, 2

@ wow undocumented errata
@ CONFIG_ARM_ERRATA_828420
    mrc     p15, 0, r1, c0, c0, 0
    ubfx    r1, r1, #4, #12
    ldr     r2, =0x00000c0e
    teq     r1, r2
    bne     1f
    mrc     p15, 0, r1, c15, c0, 1
    orr     r1, r1, #1 << 30
    mcr     p15, 0, r1, c15, c0, 1
1:
@ end

cluster0:
    b psci_cpu_entry
END(mtk_psci_entry)

.popsection

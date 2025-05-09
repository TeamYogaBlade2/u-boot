#ifndef __DISP_DRV_LOG_H__
#define __DISP_DRV_LOG_H__

#include <stdio.h>

#define DISP_LOG_PRINT(level, sub_module, fmt, arg...)      \
    do {                                                    \
        printf("[DISP/"sub_module"]"fmt, ##arg);            \
    }while(0)

#define LOG_PRINT(level, module, fmt, arg...)               \
    do {                                                    \
        printf("["module"]"fmt, ##arg);                     \
    }while(0)

#endif // __DISP_DRV_LOG_H__

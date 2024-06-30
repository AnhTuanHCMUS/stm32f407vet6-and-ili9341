
#ifndef XPT2046_XPT2046_H_
#define XPT2046_XPT2046_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "stdbool.h"
#define XPT_XMIN     3000
#define XPT_YMIN     4100
#define XPT_XMAX     60000
#define XPT_YMAX     63000
#define XPT_WIDTH    (XPT_XMAX - XPT_XMIN)
#define XPT_HEIGHT   (XPT_YMAX - XPT_YMIN)
#define GUI_WIDTH    320
#define GUI_HEIGHT   240



#define Z_THRESHOLD     5000

#define MSEC_THRESHOLD  3

void       XPT2046_Init(void);
void       XPT2046_SetCS(void);
void       XPT2046_ResetCS(void);
uint16_t   XPT2046_Read_AD(uint8_t CMD);
void       XPT2046_Update(uint16_t *x, uint16_t *y, uint16_t *z);
void       ConvXPTtoILI(uint16_t *x, uint16_t *y);
bool       XPT2046_TouchPressed(void);
void 	   XPT2046_UnTouch(void);

#ifdef __cplusplus
}
#endif


#endif /* XPT2046_XPT2046_H_ */

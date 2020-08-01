#pragma once
#ifndef __LCDM__H__
#define __LCDM__H__

#include <stdbool.h>
#include <stdint.h>

bool LCDM_init(void);
void LCDHW_Write8(uint8_t bIsData, uint8_t Data);
void SPIM_WriteTxData(uint8_t Data);
#endif // __LCDM__H__

#ifndef	_EE24_H
#define	_EE24_H

#ifdef __cplusplus
extern "C" {
#endif
/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    2.2.1
  
  
  Reversion History:
  
  (2.2.1)
  Fix erase chip bug.
  
  (2.2.0)
  Add erase chip function.
  
  (2.1.0)
  Fix write bytes.
  
  (2.0.0)
  Rewrite again.

*/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ee24Config.h"
#include "main.h"

enum Error {
	ErrorNo = 0,
	ErrorNotConnected = -1,
	ErrorRead = -2,
	ErrorWrite = -3,
	ErrorIndex = -4
};

void    ee24_init(I2C_HandleTypeDef* hi2c, uint16_t devAddress, uint16_t memSizeKbits);
bool    ee24_isConnected(void);
int    ee24_write(uint32_t address, void *data, uint32_t size);
int    ee24_read(uint32_t address, void *data, uint32_t size);
bool    _ee24_write(uint16_t address, uint8_t *data, size_t lenInBytes, uint32_t timeout);
bool    _ee24_read(uint16_t address, uint8_t *data, size_t lenInBytes, uint32_t timeout);
bool    ee24_eraseChip(void);
#ifdef __cplusplus
}
#endif

#endif

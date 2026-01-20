

#include "ee24.h"
#if (_EEPROM_USE_FREERTOS == 1)
#include "cmsis_os.h"
#define ee24_delay(x)   osDelay(x)
#else
#define ee24_delay(x)   HAL_Delay(x)
#endif

I2C_HandleTypeDef* _hi2c = NULL;
uint16_t _devAddress = 0;
uint16_t _memSizeKbits = 0;
uint16_t _eepromPSize = 0;
enum Error mErrorLast = ErrorNo;
uint32_t _timeout = 1000;

uint8_t ee24_lock = 0;

void ee24_init(I2C_HandleTypeDef* hi2c, uint16_t devAddress, uint16_t memSizeKbits)
{
	_hi2c = hi2c;
	_devAddress = devAddress;
	_memSizeKbits = memSizeKbits;
	if ((_memSizeKbits == 1) || (_memSizeKbits == 2))
				_eepromPSize = 8;
	else if ((_memSizeKbits == 4) || (_memSizeKbits == 8) || (_memSizeKbits == 16))
				_eepromPSize = 16;
	else
		_eepromPSize = 32;
}
//################################################################################################################
bool ee24_isConnected(void)
{
	if(_hi2c == NULL) return false;
  #if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
  #endif

  if (HAL_I2C_IsDeviceReady(_hi2c, _devAddress, 2, 100)==HAL_OK)
    return true;
  else
    return false;	
}

int ee24_write(uint32_t address, void *data, uint32_t size)
{
	uint16_t _addr = (uint16_t)address;
		size_t _size = (size_t)size;
	if (ee24_isConnected())
		{
			bool res = _ee24_write(_addr, (uint8_t *)data, _size, _timeout);
			if (res) {
				mErrorLast = ErrorNo;
			} else {
				mErrorLast = ErrorWrite;
			}
		} else {
			mErrorLast = ErrorNotConnected;
		}
		return mErrorLast;
}

int ee24_read(uint32_t address, void *data, uint32_t size)
{
	uint16_t _addr = (uint16_t)address;
	size_t _size = (size_t)size;
	if (ee24_isConnected())
	{
		bool res = _ee24_read(_addr, (uint8_t *)data, _size, _timeout);
		if (res) {
			mErrorLast = ErrorNo;
		} else {
			mErrorLast = ErrorRead;
		}
	} else {
		mErrorLast = ErrorNotConnected;
	}
	return mErrorLast;
}


//################################################################################################################
bool _ee24_write(uint16_t address, uint8_t *data, size_t len, uint32_t timeout)
{
	if(_hi2c == NULL) return false;
  if (ee24_lock == 1)
    return false;
  ee24_lock = 1; 
  uint16_t w;
  uint32_t startTime = HAL_GetTick();
  #if	(_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN,GPIO_PIN_RESET);
  #endif
  while (1)
  {
	  w = _eepromPSize - (address  % _eepromPSize);
	  if (w > len)
		  w = len;
	  HAL_StatusTypeDef result = HAL_ERROR;
	  if ((_memSizeKbits==1) || (_memSizeKbits==2))
		  result = HAL_I2C_Mem_Write(_hi2c, _devAddress, address, I2C_MEMADD_SIZE_8BIT, data, w, 100);
	  else if (_memSizeKbits==4)
		  result = HAL_I2C_Mem_Write(_hi2c, _devAddress | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, 100);
	  else if (_memSizeKbits==8)
		  result = HAL_I2C_Mem_Write(_hi2c, _devAddress | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, 100);
	  else if (_memSizeKbits==16)
		  result = HAL_I2C_Mem_Write(_hi2c, _devAddress | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, w, 100);
	  else
		  result = HAL_I2C_Mem_Write(_hi2c, _devAddress, address, I2C_MEMADD_SIZE_16BIT, data, w, 100);
	  if (result == HAL_OK)
	  {
		  ee24_delay(10);
		  len -= w;
		  data += w;
		  address += w;
		  if (len == 0)
		  {
#if (_EEPROM_USE_WP_PIN==1)
			  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
#endif
			  ee24_lock = 0;
			  return true;
		  }
		  if (HAL_GetTick() - startTime >= timeout)
		  {
			  ee24_lock = 0;
			  return false;
		  }
	  }
	  else
	  {
#if (_EEPROM_USE_WP_PIN==1)
		  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
#endif
		  ee24_lock = 0;
		  return false;
	  }
  }
}
//################################################################################################################
bool _ee24_read(uint16_t address, uint8_t *data, size_t len, uint32_t timeout)
{
	if(_hi2c == NULL) return false;
  if (ee24_lock == 1)
    return false;
  ee24_lock = 1;
  #if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO, _EEPROM_WP_PIN, GPIO_PIN_SET);
  #endif
  HAL_StatusTypeDef result = HAL_ERROR;
  if ((_memSizeKbits==1) || (_memSizeKbits==2))
	  result = HAL_I2C_Mem_Read(_hi2c, _devAddress, address, I2C_MEMADD_SIZE_8BIT, data, len, 100);
  else if (_memSizeKbits == 4)
	  result = HAL_I2C_Mem_Read(_hi2c, _devAddress | ((address & 0x0100) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 100);
  else if (_memSizeKbits == 8)
	  result = HAL_I2C_Mem_Read(_hi2c, _devAddress | ((address & 0x0300) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 100);
  else if (_memSizeKbits==16)
	  result = HAL_I2C_Mem_Read(_hi2c, _devAddress | ((address & 0x0700) >> 7), (address & 0xff), I2C_MEMADD_SIZE_8BIT, data, len, 100);
  else
	  result = HAL_I2C_Mem_Read(_hi2c, _devAddress, address, I2C_MEMADD_SIZE_16BIT, data, len, timeout);
  if (result == HAL_OK)
  {
	  ee24_lock = 0;
	  return true;
  }
  else
  {
    ee24_lock = 0;
    return false;	
  }    
}
//################################################################################################################
bool ee24_eraseChip(void)
{
  const uint8_t eraseData[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF\
    , 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint32_t bytes = 0;
  while ( bytes < (_memSizeKbits * 256))
  {
    if (_ee24_write(bytes, (uint8_t*)eraseData, sizeof(eraseData), 100) == false)
      return false;
    bytes += sizeof(eraseData);           
  }
  return true;  
}
//################################################################################################################

#ifndef	_EE24CONFIG_H
#define	_EE24CONFIG_H

#define		_EEPROM_SIZE_KBIT							EEPROM_SIZE_KBIT //define this in another user file.
#define		_EEPROM_I2C   								EEPROM_I2C
#define		_EEPROM_USE_FREERTOS          EEPROM_USE_FREERTOS
#define		_EEPROM_ADDRESS               EEPROM_ADDRESS
#define		_EEPROM_USE_WP_PIN            EEPROM_USE_WP_PIN

#if (_EEPROM_USE_WP_PIN==1)
#define		_EEPROM_WP_GPIO								EE_WP_GPIO_Port
#define		_EEPROM_WP_PIN								EE_WP_Pin
#endif

#endif


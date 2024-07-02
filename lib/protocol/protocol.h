#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <Arduino.h>

#define START_SYMB		'$'
#define DEV_DISPLAY		0xC3
#define DEV_VASCA		0xC5
#define DEV_GATEWAY		0xD3
#define DEV_UDP			0xD5

// Codici Operazione ------------------------------------------------------------------------------
typedef enum {
	ON_FUNC		= 0x13,
	OFF_FUNC	= 0x15,
	SET_PARAM	= 0x19,
	STATO_REQ	= 0x1A
}op_t;

// Codici SottoFunzione relativi al codice funzione 'Z' -------------------------------------------
typedef enum {
	Z_INPUT		= '1',
	Z_OUTPUT	= '2',
	Z_EEPROM	= '3',
	Z_CLOCK		= '4'
}sub_func_t;

void Check_sum (uint8_t Dato, uint8_t *Crc);
uint8_t Check_nby (uint8_t N_by, uint8_t *Punt );

#endif  /* PROTOCOL_H_ */
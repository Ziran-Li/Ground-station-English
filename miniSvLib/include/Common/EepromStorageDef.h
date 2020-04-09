

#ifndef INCLUDED_EEPROMSTORAGEDEF_H
#define INCLUDED_EEPROMSTORAGEDEF_H

#include "Common/StandardDef.h"

#define EEPROM_DATASIZE    29
#define EEPROM_BLOCKCOUNT  64

typedef struct _tag_EepromData
{
	_UWORD BlockID;
	_UBYTE byte[EEPROM_DATASIZE];
} EepromData;

#endif
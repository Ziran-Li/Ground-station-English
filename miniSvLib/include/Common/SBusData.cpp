#include "SBusData.h"

_tag_SBusData::_tag_SBusData()
{
	for( int i = 0; i < SBUS_PROPORTIONAL_CHANNELS; i++ ) {
		this->Prop[i] = 0;
	}
	this->Flags = 0;
}

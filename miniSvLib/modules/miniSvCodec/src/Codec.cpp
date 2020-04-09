#include "stdafx.h"
#include "Codec.h"

namespace MiniSvCodec
{

	_UQWORD ConvPropoPacket(const PropoData* pPropo)
	{
		_UQWORD Ret;

		Ret = Fix9bit((pPropo->PPitch >> 1) - 500);
		Ret <<= 9; Ret |= Fix9bit((pPropo->PRoll >> 1) - 500);
		Ret <<= 9; Ret |= Fix9bit((pPropo->PYaw >> 1) - 500);
		Ret <<= 9; Ret |= Fix9bit((pPropo->PPower >> 1) - 500);
		Ret <<= 1; Ret |= pPropo->Sw1 != 0;
		Ret <<= 1; Ret |= pPropo->Sw2 != 0;
		Ret <<= 1; Ret |= pPropo->Sw3 != 0;
		Ret <<= 1; Ret |= pPropo->Sw4 != 0;

		return Ret;
	}

	void DecPropoPacket(_UQWORD data, PropoData* pPropo)
	{
		pPropo->Sw4 = ((unsigned int)(data & 0x001)); data >>= 1;
		pPropo->Sw3 = ((unsigned int)(data & 0x001)); data >>= 1;
		pPropo->Sw2 = ((unsigned int)(data & 0x001)); data >>= 1;
		pPropo->Sw1 = ((unsigned int)(data & 0x001)); data >>= 1;
		pPropo->PPower = ((unsigned int)(data & 0x1FF) + 500) << 1; data >>= 9;
		pPropo->PYaw = ((unsigned int)(data & 0x1FF) + 500) << 1; data >>= 9;
		pPropo->PRoll = ((unsigned int)(data & 0x1FF) + 500) << 1; data >>= 9;
		pPropo->PPitch = ((unsigned int)(data & 0x1FF) + 500) << 1; data >>= 9;
	}

}
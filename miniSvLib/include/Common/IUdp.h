///
/// @file       IUdp.h
///
/// @brief      UDP通信インターフェース定義
///
/// @author     iwakura
///

#pragma once

#include "Common/IConnection.h"

class IUdp : public IConnection
{
public:
	virtual CRESULT ReadString(void* ptr, _UWORD len) = 0; // 連続データ送信(UDP専用)
};

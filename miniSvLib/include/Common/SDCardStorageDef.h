// 201412xx TCS.Y.Nakamura ADD_S
#ifndef INCLUDED_SDCARDSTORAGEDEF_H
#define INCLUDED_SDCARDSTORAGEDEF_H

#include "Common/StandardDef.h"

// 201506xx TCS.Y.Nakamura ADD_S [ファイル転送機能拡張対応]
// Lib/SDCard.hから移動しました.
#define FILE_NAME_LEN	11	// ファイル名の最大サイズ（単位はバイト）
// ↓既存では直値使ってたので一応定義.必要な箇所で使用して下さい.既存箇所は修正しません.
#define FILE_NAME_LEN_L	8	// 8:3形式の左側
#define FILE_NAME_LEN_R	3	// 8:3形式の右側
// 201506xx TCS.Y.Nakamura ADD_E

typedef struct _tag_SDCardFileInfo{		// SDファイル情報(20)
// 201506xx TCS.Y.Nakamura MOD_S [ファイル転送機能拡張対応]
	_UBYTE	DirName[FILE_NAME_LEN];		// ファイル名(11)
// 201506xx TCS.Y.Nakamura MOD_E
	_UBYTE	DirAttr;					// 属性(1)
	_UWORD	DirCrtTime;					// ファイル作成時間(2)
	_UWORD	DirCrtDate;					// ファイル作成日付(2)
	_UWORD	DirWrtTime;					// 最後にファイル作成した時間(2)
	_UWORD	DirWrtDate;					// 最後にファイル作成した日付(2)
} SD_FILE_INFO_T;

#endif	// INCLUDED_SDCARDSTORAGEDEF_H
// 201412xx TCS.Y.Nakamura ADD_E

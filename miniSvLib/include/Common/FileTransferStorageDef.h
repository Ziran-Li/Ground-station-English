#ifndef _INCLUDED_FILE_TRANSFER_STORAGE_DEF_H_
#define _INCLUDED_FILE_TRANSFER_STORAGE_DEF_H_

#include "Common/StandardDef.h"			// 基本データ型
#include "Common/SDCardStorageDef.h"	// SDファイル情報型

//--------
//定数定義
//--------
// 処理結果コード
#define PARAM_RESULT_CODE_OK           (0x00) // 成功
#define PARAM_RESULT_CODE_NG           (0xFF) // 失敗
#define PARAM_RESULT_CODE_STATE_ERR    (0xFE) // 状態異常
#define PARAM_RESULT_CODE_PARAM_ERR    (0xFD) // パラメータ異常
#define PARAM_RESULT_CODE_CRC_ERR      (0xFC) // CRC異常
#define PARAM_RESULT_CODE_SDCARD_BUSY  (0xFB) // SDカードビジー

#define TRANS_MODE_NORMAL              (0x00) // デフォルトモード(通常運用状態)
#define TRANS_MODE_BOOT                (0x01) // ブートモード

// CRC32計算関連
#define CRC32_POLYNOMIAL               (0x04C11DB7UL)  // CRC32計算用(生成多項式X32+X26+X23+X22+X16+X12+X11+X10+X8+X7+X5+X4+X2+X1+1)
#define CRC32_INITIAL_VAL              (0xFFFFFFFFUL)  // 初期値
#define CRC32_OUT_XOR_VAL              (0xFFFFFFFFUL)  // 出力XOR

// 受信データ管理ビットマップ関連
#define DATA_BITMAP_SIZE               (32)    // 受信データ管理ビットマップ領域サイズ
#define DATA_BITMAP_VALID_VAL          (0xFF)  // 受信済みを表す1byte単位の正常値
#define DATA_BITMAP_DIV32_SHIFT        (5)     // 32除算時の右シフト量
#define DIV32_MOD_MASK                 (0x1F)  // 剰余算出用マスク値

// データサイズ関連
#define DATA_TRANS_USER_DATA_LEN       (32)    // バースト転送データ長(暫定で固定値)
#define DATA_BLOCK_SIZE                (8192)  // データブロックサイズ
#define VALID_DATA_LEN_MIN             (1)     // 有効データ長最小値(0は許容しない)
//#define VALID_DATA_LEN_MAX           (536870912)  // 全データ長最大サイズ(512MB)(拡張用)
#define VALID_DATA_LEN_MAX             (512*1024*1024)  // 全データ長最大サイズ(512MB)(拡張用)

// ディレクトリ移動、表示関連定義
#define FILE_PATH_LENGTH               (256)  // ファイルパスサイズ

#define STR_SPACE                      " "        // スペース
#define STR_PERIOD                     "."        // ピリオド
#define STR_UP                         "../"      // 上へ
#define STR_NULL                       "\0"       // 終端文字
#define STR_SLASH                      "/"        // カレントフォルダ
#define STR_FIRMWARE                   "FIRMWARE" // カレントフォルダ
#define STR_SYSTEM                     "SYSTEM"   // カレントフォルダ
	

//-------------
// 仕様定義関連
//-------------

// RW転送種別
#define SPECDEF_TRANSFER_KIND_READ     (0x00)  // 読み込み
#define	SPECDEF_TRANSFER_KIND_WRITE    (0x01)  // 書き込み

// 201506xx TCS.Y.Nakamura ADD_S [ファイル転送機能拡張対応]
// パケット送信種別(UAV -> GCS)
typedef enum
{
	SENDTYPE_UAV_NONE                    = 0x00, // 送信無し
	SENDTYPE_UAV_MODE_CHANGE_RES         = 0x01, // データ転送モード変更応答
	SENDTYPE_UAV_START_NTF               = 0x02, // データ転送開始通知
	SENDTYPE_UAV_STOP_NTF                = 0x03, // データ転送停止通知
	SENDTYPE_UAV_MULTI_NTF               = 0x04, // 汎用通知
	SENDTYPE_UAV_RESEND_REQ              = 0x05, // データ再送要求
	SENDTYPE_UAV_USERDATA                = 0x06, // ユーザーデータ
	SENDTYPE_UAV_MULTI_REQ               = 0x07, // 汎用要求
	SENDTYPE_UAV_UPDATE_FILE_LIST_RSP    = 0x08, // ファイルリスト更新応答
	SENDTYPE_UAV_FILE_LIST_DATA          = 0x09, // ファイルリストデータ
	SENDTYPE_UAV_BUSY                    = 0xFE, // ビジー中
	SENDTYPE_UAV_INVALID                 = 0xFF, // 無効値(仮)
} SENDTYPE_UAV_E;

// パケット送信種別(UAV <- GCS)
typedef enum
{
	SENDTYPE_GCS_NONE                    = 0x00, // 送信無し
	SENDTYPE_GCS_MODE_CHANGE_REQ         = 0x01, // データ転送モード変更要求
	SENDTYPE_GCS_START_REQ               = 0x02, // データ転送開始要求
	SENDTYPE_GCS_STOP_REQ                = 0x03, // データ転送停止要求
	SENDTYPE_GCS_MULTI_REQ               = 0x04, // 汎用要求
	SENDTYPE_GCS_RESEND_REQ              = 0x05, // データ再送要求
	SENDTYPE_GCS_USERDATA                = 0x06, // ユーザーデータ
	SENDTYPE_GCS_MULTI_NTF               = 0x07, // 汎用通知
	SENDTYPE_GCS_UPDATE_FILE_LIST_REQ    = 0x08, // ファイルリスト更新要求
	SENDTYPE_GCS_FILE_LIST_DATA_SEND_REQ = 0x09, // ファイルリストデータ送信要求
	SENDTYPE_GCS_INVALID                 = 0xFF, // 無効値(仮)
} SENDTYPE_GCS_E;
// 201506xx TCS.Y.Nakamura ADD_E

#define SPECDEF_FILE_LIST_DATA_REQ             (0x01)  // ファイルリストデータ送信要求
#define SPECDEF_FILE_LIST_DATA_NTF             (0x02)  // ファイルリストデータ送信通知

// コマンド種別
// 要求系(PC⇒UAV)
#define PARAM_CMD_KIND_START_BLOCK_SET_REQ       (0x01)  // データ転送開始ブロック設定要求
#define PARAM_CMD_KIND_COMP_QUERY_REQ            (0x02)  // データ転送完了問い合わせ要求
#define PARAM_CMD_KIND_SDCARD_WRITE_REQ          (0x03)  // SDカード書き込み要求
#define PARAM_CMD_KIND_SDCARD_READ_REQ           (0x04)  // SDカード読み込み要求
#define PARAM_CMD_KIND_SDDATA_SEND_REQ           (0x05)  // SDデータ送信要求
// 通知系(UAV⇒PC)
#define PARAM_CMD_KIND_START_BLOCK_SET_COMP_NTF  (0x01)  // データ転送開始ブロック設定完了通知
#define PARAM_CMD_KIND_TRANS_COMP_NTF            (0x02)  // データ転送完了通知
#define PARAM_CMD_KIND_SDCARD_WRITE_COMP_NTF     (0x03)  // SDカード書き込み完了通知
#define PARAM_CMD_KIND_SDCARD_READ_COMP_NTF      (0x04)  // SDカード読み込み完了通知


//-------------
// 受信データ型
//-------------
typedef struct
{
	_UBYTE					PacketKind;			// パケット種別
	void*					pData;				// データポインタ
} FILE_TRANSFER_RX_PACKET_T;

//--------------------------------------
// データ転送モード変更要求情報(全1byte)
//--------------------------------------
typedef struct
{
	_UBYTE					Mode;				// 転送モード(1)	1:ブートモード、1以外:無効
} MODE_CHG_REQ_INFO_T;

//--------------------------------------
// データ転送モード変更応答情報(全1byte)
//--------------------------------------
typedef struct
{
	_UBYTE					Result;				// 結果
} MODE_CHG_RES_INFO_T;

//---------------------------------
// データ転送開始要求情報(全36byte)
//---------------------------------
typedef struct
{
	_UBYTE					RWKind;				// RW要求種別(1) 有効値:0,1
	_UDWORD					DataLen;			// 有効データ長(4)	※Read時はDon't Care
// 201506xx TCS.Y.Nakamura ADD_S [ファイル転送機能拡張対応]
	_UBYTE			DirName[FILE_NAME_LEN];	// フォルダ名(11)
	_UDWORD			FileSize;		// ファイルサイズ
// 201506xx TCS.Y.Nakamura ADD_E
	SD_FILE_INFO_T			stSdFileInfo;		// ファイル情報(20)	※Read時はDirName以外Don't Care
} START_REQ_SFT_INFO_T;

//---------------------------------
// データ転送開始通知情報(全26byte)
//---------------------------------
typedef struct
{
	_UBYTE					RWKind;				// RW通知種別(1) 有効値:0,1
	_UBYTE					Result;				// データ転送開始要求に対する結果(1)
	_UDWORD					DataLen;			// 有効データ長(4)
	SD_FILE_INFO_T			stSdFileInfo;		// SDファイル情報(20)
} START_NTF_SFT_INFO_T;
	
//----------------------
// 汎用要求情報(全7byte)
//----------------------
typedef struct
{
	_UBYTE					CmdReqKind;			// 要求種別(1) 有効値:1～5
	_UWORD					BlockNo;			// ブロック番号(2) 有効値:0～65535
	_UDWORD					CRC32Data;			// CRC32データ(4)
} MULTI_REQ_SFT_INFO_T;

//----------------------
// 汎用通知情報(全4byte)
//----------------------
typedef struct
{
	_UBYTE					CmdNtfKind;			// 通知種別(1) 有効値:1～4
	_UBYTE					Result;				// 通知結果(1)
	_UWORD					BlockNo;			// ブロック番号(2) 有効値:0～65535
} MULTI_NTF_INFO_T;

//--------------------------------
// データ転送停止要求情報(全1byte)
//--------------------------------
typedef struct
{
	_UBYTE					Flag;				// データ転送停止モード(1) 停止する:1
} STOP_REQ_SFT_INFO_T;

//--------------------------------
// データ転送停止要求情報(全1byte)
//--------------------------------
typedef struct
{
	_UBYTE					Result;				// データ転送停止要求に対する結果(1)
} STOP_NTF_SFT_INFO_T;

//-----------------------------
// ユーザーデータ情報(全33byte)
//-----------------------------
typedef struct
{
	_UBYTE					DataNo;				// データ番号(1) 有効値:0～255
	_UBYTE	UserData[DATA_TRANS_USER_DATA_LEN];	// ユーザーデータ領域(32)
} USER_DATA_SFT_INFO_T;

//-----------------------------
// データ再送要求情報(全32byte)
//-----------------------------
typedef struct
{
	_UBYTE	ResendData[DATA_TRANS_USER_DATA_LEN];	// 再送要求データ(32)
} RESEND_REQ_INFO_T;

// 201506xx TCS.Y.Nakamura ADD_S [ファイル転送機能拡張対応]
//---------------------------------
// ファイルリスト更新要求(全11byte)
//---------------------------------
typedef struct
{
	_UBYTE	CurDirName[FILE_NAME_LEN];	// カレントディレクトリ名
} UPDATE_FILE_LIST_REQ_INFO_T;

//--------------------------------
// ファイルリスト更新応答(全3byte)
//--------------------------------
typedef struct
{
	_UBYTE					Result;				// 結果(1)
	_UWORD					nFileCnt;			// ファイル数(フォルダ)情報数(2)
} UPDATE_FILE_LIST_RSP_INFO_T;

//-------------------------------------
// ファイル(フォルダ)固有情報(全24byte)
//-------------------------------------
typedef struct
{
	_UDWORD					nSize;				// ファイルサイズ(4)
	SD_FILE_INFO_T			stSdFileInfo;		// ファイル(フォルダ)情報(20)
} FILE_UNIQUE_INFO_T;


//-------------------------------
// ファイルリストデータ(全27byte)
//-------------------------------
typedef struct
{
	_UBYTE					nMode;				// モード(1)
	_UWORD					nDataNo;			// データ番号(2)
	FILE_UNIQUE_INFO_T		stFileUniqueInfo;	// ファイル(フォルダ)固有情報(24)
} FILE_LIST_DATA_T;
// 201506xx TCS.Y.Nakamura ADD_E

#endif	// _INCLUDED_FILE_TRANSFER_STORAGE_DEF_H_


// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Windows ヘッダーから使用されていない部分を除外します。
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 一部の CString コンストラクターは明示的です。

// 一般的で無視しても安全な MFC の警告メッセージの一部の非表示を解除します。
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC の Internet Explorer 4 コモン コントロール サポート
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC におけるリボンとコントロール バーのサポート



//*******************************************************************************
// ユーザー定義メッセージ
//*******************************************************************************
#define WM_UPDATE_DIALOGVIEW    (WM_APP + 1)    // 再描画メッセージ (wParam : メッセージの間隔[ms])
#define WM_UPDATE_CONTROL       (WM_APP + 2)    // コントロールの設定更新メッセージ
#define WM_APPEND_LOG           (WM_APP + 3)    // メインウィンドウのログに文字列を表示するためのメッセージ（文字列は，theApp のキューに保存して受け渡しする．）
#define WM_DESTROY_MODELESS     (WM_APP + 4)    // 子のモードレスダイアログが破棄されたときに親ウィンドウに通知するメッセージ (wParam : 破棄されたダイアログのポインタ)
#define WM_SERVER_DATA_RECEIVE  (WM_APP + 5)    // X-Monitor Server がデータの受信をスレッドに通知するために使用するメッセージ (wParam : 受信したデータの種類)
#define WM_CLIENT_ACCEPT        (WM_APP + 6)    // X-Monitor Server がクライアントの接続をスレッドに通知するために使用するメッセージ (wParam : IPアドレス, lParam : 合計)
#define WM_CLIENT_CLOSE         (WM_APP + 7)    // X-Monitor Server がクライアントの切断をスレッドに通知するために使用するメッセージ (wParam : IPアドレス, lParam : 合計)
#define WM_NOTIFY_SYSTEM_RESUME (WM_APP + 8)    // システムの再開をスレッドに通知するために使用するメッセージ
#define WM_DIALOG_MESSAGE       (WM_APP + 1000) // これ以降は，各ダイアログ内で定義されるメッセージとする








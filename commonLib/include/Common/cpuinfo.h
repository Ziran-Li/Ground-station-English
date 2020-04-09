//
//		cpuinfo.h
//
//		Win32専用 ハードウェア定義ファイル
//
//		'09/12/21	Ver 0.01	とりあえず初版 by iwakura
//
//		ハードウェアが変わらない限り変更されないCPU固有の情報をここに記述する
//
//

#ifndef INCLUDED_CPUINFO_H
#define INCLUDED_CPUINFO_H

// エンディアン関連の定義
#define BIG_ENDIAN        0 // BYTEORDER のための定義
#define LITTLE_ENDIAN     1 // BYTEORDER のための定義
#define SELECTABLE_ENDIAN 2 // BYTEORDER のための定義（予約．本当に使うかは未定）

#ifndef BYTEORDER                // CPU固有情報
#define BYTEORDER  LITTLE_ENDIAN // Intel x86
#endif

#endif
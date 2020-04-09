///
/// @file       Debug.h
///
/// @brief      コンソールデバッグ用機能（HEWでコンパイルすると空の関数になります）
///

#ifndef INCLUDED_DEBUG_H
#define INCLUDED_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

// 画面表示系関数
void debug_puts(char* string);
void debug_printf(char* pszFormat, ...);

// 画面消去
void debug_cls(); // 遅いので使い勝手悪し

#ifdef __cplusplus
}
#endif

#endif
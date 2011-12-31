/* DLLsha512.h
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

// 以下の ifdef ブロックは DLL からのエクスポートを容易にするマクロを作成するための
// 一般的な方法です。この DLL 内のすべてのファイルは、コマンド ラインで定義された DLLSHA512_EXPORTS
// シンボルでコンパイルされます。このシンボルは、この DLL を使うプロジェクトで定義することはできません。
// ソースファイルがこのファイルを含んでいるほかのプロジェクトは、
// DLLSHA512_API 関数を DLL からインポートされたと見なすのに対し、この DLL は、このマクロで定義された
// シンボルをエクスポートされたと見なします。
#ifdef DLLSHA512_EXPORTS
#define DLLSHA512_API __declspec(dllexport)
#else
#define DLLSHA512_API __declspec(dllimport)
#endif

DLLSHA512_API void DLLsha512_DataHash(uchar *out, int outbytes, uchar *seed, int seedbytes);

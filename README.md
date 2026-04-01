# libcmdline
Simple command line parsing library for X68k

elf2x68k + newlib環境向けのクオート考慮コマンドライン解析ライブラリです。

XC2.1のスタートアップは"や'を使った引数のクオートに対応しており、空白文字の入ったファイル名やメッセージを渡すことができるのですが、newlibのスタートアップはそのようなサポートはありません。

XC前提に書かれたコードの移植性を上げるために用意したものです。なお、HUPAIR規格とは関係ありません。

以下の関数が利用可能です。
```
#include <cmdline.h>

int cmdline_get_argc();
char** cmdline_get_argv();
```

使う時は、サブモジュールとして組み込むのが簡単です。例えばプロジェクト直下にて以下を実行します。

```
git submodule add https://github.com/tantanGH/libcmdline.git libs/libcmdline
```

以下のようなツリーとなります。

```
my_app/
├── .git/
├── .gitmodules
├── libs/
│   └── libcmdline/
│       ├── include/cmdline.h
│       └── lib/libcmdline.a
└── src/
    ├── main.c
    └── Makefile
```

ヘッダー検索パスとライブラリ検索パスをMakefile内で
```
-I../libs/libmxdrv/include
-L../libs/libmxdrv/lib
```
のように指定し、`-lcmdline` でリンクできます。
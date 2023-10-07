# TiktokenCpp

[English](README.md) | [简体中文]

[![Linux](https://badgen.net/badge/Linux/success/green?icon=github)](https://github.com/inte2000/llm_cpp/actions/workflows/CI.yml?query=branch%3Amaster)
[![Windows](https://badgen.net/badge/Windows/success/green?icon=github)](https://github.com/inte2000/llm_cpp/actions/workflows/CI.yml?query=branch%3Amaster)
[![issues](https://badgen.net/github/issues/inte2000/llm_cpp?icon=github)](https://github.com/inte2000/llm_cpp/issues)
[![license](https://badgen.net/github/license/inte2000/llm_cpp?icon=github)](LICENSE)



[tiktoken](https://github.com/openai/tiktoken) 是 openai 的开源词法分析器，TiktokenCpp 是 C++ 移植版本。TiktokenCpp 采用现代 C++ 语言特性，只为提供与 tiktoken 的 Python 接口几乎一致的接口函数。



## ✨ 使用

```c++
#include "tiktoken.h"

using namespace TiktokenCpp;

//auto encoding = EncodingForModel("GPT-4"); //same as next line
auto encoding = GetEncoding("cl100k_base");

//ordinary text 
std::string text = "tiktoken is great!";
auto tokens = encoding->EncodeOrdinary(text);
PrintTokens(std::cout, tokens);// [83, 1609, 5963, 374, 2294, 0]
std::string dec_text = encoding->Decode(tokens);
assert(dec_text == text);

//non-English text
std::string text = "Hi，试一下中文字符！";
auto tokens = encoding->EncodeOrdinary(text);
PrintTokens(std::cout, tokens);// [13347, 3922, 42421, 15120, 17297, 16325, 17161, 49491, 6447]
std::string dec_text = encoding->Decode(tokens);
assert(dec_text == text);

//special text 1
std::string text = "hello <|endoftext|>";
//allowedSpecial: "all" , disallowedSpecial: "all" (default)
auto tokens = encoding->Encode(text, "all"); 
PrintTokens(std::cout, enc); //[15339, 220, 100257]
auto dec_text = encoding->Decode(tokens);
assert(dec_text == text);

//special text 2
std::string text = "<|endoftext|>";
//allowedSpecial: "<|endoftext|>" , disallowedSpecial: "all" (default)
auto tokens = encoding->Encode(text, {"<|endoftext|>"});
PrintTokens(std::cout, enc); //[100257]
auto dec_text = encoding->Decode(tokens);
assert(dec_text == text);

//special text 3
std::string text = "<|endoftext|>";
//allowedSpecial: null (default), disallowedSpecial: null
auto tokens = encoding->Encode(text, {}, {});
PrintTokens(std::cout, enc); //[27, 91, 8862, 728, 428, 91, 29]
auto dec_text = encoding->Decode(tokens);
assert(dec_text == text);
```

## ✨ 下载 encoding 文件

```c++
auto encodingnames = ListEncodingNames();
for (const auto& name : encodingnames)
{
    if (!IsLocalEncodingCacheExisted(name))
    {
        std::cout << "Downloading encoding file for " << name << "...";
        bool rtn = DownloadEncoding(name); //DownloadEncoding(name, proxy);
        std::cout << (rtn ? "successfully!" : "failed!") << std::endl;
    }
}
```

## ✨ Build

### 依赖库准备

​        TiktokenCpp 依赖这些开源的库：

- [Boost](https://www.boost.org)
- [curl](https://curl.se/libcurl/)
- [pcre2](https://github.com/PCRE2Project/pcre2)
- [iconv](https://www.gnu.org/software/libiconv/)
- [cppcodec](https://github.com/tplgy/cppcodec)（已经附带在 3rdparty 目录中）

### For Linux

```shell
git clone https://github.com/inte2000/llm_cpp.git
cd llm_cpp
mkdir build
cd build/
cmake ..
make
```

如果编译没有问题，可以安装 TiktokenCpp 库：

```shell
sudo make install
```

默认安装位置是 '\usr\local\'，可以运行 token_test 测试一下：

```shell
cd /usr/local/bin
token_test
```

输出结果预期如下：

```text
Current encoding cache location: /usr/local/bin/cache
Encoding: "hello world"
[15339, 1917]
Encoding: "goodbye world"
[19045, 29474, 1917]
Encoding: "tiktoken is great!"
[83, 1609, 5963, 374, 2294, 0]
Encoding: "Hi，试一下中文字符！"
[13347, 3922, 42421, 15120, 17297, 16325, 17161, 49491, 6447]
Encoding: "Hello, 😀"
[9906, 11, 91416]
Encoding: ""
[]
Encoding: "hello <|endoftext|>", with allowedSpecial: all , disallowedSpecial: all
[15339, 220, 100257]
Encoding: "<|endoftext|>", with allowedSpecial: <|endoftext|> , disallowedSpecial: all
[100257]
Encoding: "<|endoftext|>", with allowedSpecial: null, disallowedSpecial: null
[27, 91, 8862, 728, 428, 91, 29]
Symbols test for: "tiktoken is great!"
[83, 1609, 5963, 374, 2294, 0]
[t, ik, token,  is,  great, !]
```

### For Windows

​        使用 git clone 或 将源代码包解压缩，比如："d:\code\llm_cpp"。然后根据本地依赖库的编译和安装位置，修改 "d:\code\llm_cpp\vsprj" 目录下的 'boost_1_83_0_static_runtime.props'、`libcurl-8.2.1_static.props`、`libIconv-1.16_static.props` 和 `pcre2-10.42_static.props` 四个属性表文件。以 `pcre2-10.42_static.props` 属性表文件为例，如果你本地编译的库目录结构与下面的目录结构一致：

```
$(PCRE2_ROOT)
--lib
  --Debug
    --x86
    --x64
  --Release
    --x86
    --x64    
```

 则只需要修改 `PCRE2_ROOT` 宏，将其替换成 Pcre2 安装的根目录即可：

```xml
<PropertyGroup Label="UserMacros">
  <PCRE2_ROOT>D:\development\pcre2-10.42</PCRE2_ROOT>
</PropertyGroup>
```

否则的话，就需要同时修改 `AdditionalLibraryDirectories` 属性，使其与本地编译安装的目录结构一致。

​        使用 Visual Studio 2019 打开 "d:\code\llm_cpp\vsprj\llm_cpp.sln"，依次编译 tiktoken 和 token_test 项目。

​        在 Windows 平台编译和运行 token_test 需要注意源代码文件的编码问题，TiktokenCpp 使用 UTF-8 编码作为输入文字的编码。在 Windows 平台编译和运行 token_test 或自己的测试项目，需要注意源代码文件使用 UTF-8 编码，否则，就需要进行平台本地编码与 UTF-8 编码的转换。`Utf8String.h` 提供了 `UTF8StrFromLocalMBCS` 和 `LocalMBCSFromUTF8Str` 两个函数，会自动判断平台的代码集，提供相应的转换功能。


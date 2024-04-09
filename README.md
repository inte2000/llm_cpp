# TiktokenCpp

English | [简体中文](README_zh.md)

[![Linux](https://badgen.net/badge/Linux/success/green?icon=github)](https://github.com/inte2000/llm_cpp/actions/workflows/CI.yml?query=branch%3Amaster)
[![Windows](https://badgen.net/badge/Windows/success/green?icon=github)](https://github.com/inte2000/llm_cpp/actions/workflows/CI.yml?query=branch%3Amaster)
[![issues](https://badgen.net/github/issues/inte2000/llm_cpp?icon=github)](https://github.com/inte2000/llm_cpp/issues)
[![license](https://badgen.net/github/license/inte2000/llm_cpp?icon=github)](LICENSE)



[tiktoken](https://github.com/openai/tiktoken) is an open-source tokeniser for OpenAI, and TiktokenCpp is a C++ported version. TiktokenCpp using modern C++ language features and providing interface functions that are similar to Tiktoken's Python interface.  



## ✨ Using

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

## ✨ Download encoding files

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

### Dependent Libraries 

​        TiktokenCpp relies on these open source libraries: 

- [Boost](https://www.boost.org)
- [curl](https://curl.se/libcurl/)
- [pcre2](https://github.com/PCRE2Project/pcre2)
- [iconv](https://www.gnu.org/software/libiconv/)
- [cppcodec](https://github.com/tplgy/cppcodec)（Already included in the 3rdparty directory）

### For Linux

```shell
git clone https://github.com/inte2000/llm_cpp.git
cd llm_cpp
mkdir build
cd build/
cmake ..
make
```

If there are no issues with compilation, you can install the TiktokenCpp library:

```shell
sudo make install
```

The default installation location is "\usr\local". You can run token_test for test:

```shell
cd /usr/local/bin
token_test
```

The expected output results are as follows:

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

​        Use git clone or decompress the source code package, such as: "d:\code\llm_cpp". Then modify the 'boost_1_83_0_static_runtime.props', 'libcurl-8.2.1_static.props', 'libIconv-1.16_static.props', and 'pcre2-10.42_static.props' property sheet files in the  "d:\code\llm_cpp\vsprj" directory based on the compilation and installation location of the local dependency library.  Taking the 'pcre2-10.42_static.props' property sheet file as an example, if the directory structure of your locally installed library is consistent with the following directory structure:  

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

Then you only need to modify ` PCRE2_ ROOT ` macro, replace it with the root directory of the Pcre2 installation to:

```xml
<PropertyGroup Label="UserMacros">
  <PCRE2_ROOT>D:\development\pcre2-10.42</PCRE2_ROOT>
</PropertyGroup>
```

Otherwise, it is necessary to modify the 'AdditionalLibraryDirectories' property simultaneously to make it consistent with the directory structure of the local installation.

​        Open "d:\code\llm_cpp\vsprj\llm_cpp.sln" using Visual Studio 2019 and compile the tiktoken and tokent_test projects in sequence.  

​        Compiling and running token_test on the Windows platform needs to pay attention to the encoding issue of the source code file. TiktokenCpp uses UTF-8 as the input text encoding. Compiling and running token_test or your own testing program on the Windows platform, it is important to note that the source code files should use UTF-8 encoding. Otherwise, you will need to convert the platform's local encoding to UTF-8. ` Utf8String. h ' provides two functions: ' UTF8StrFromLocalMBCS' and 'LocalMBCSFromUTF8Str', which automatically determine the platform's coding-set and provide corresponding conversion functions.

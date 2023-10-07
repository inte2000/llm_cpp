#include <iostream>
#include <filesystem>
#include <vector>
#include <cassert>
#include "Utf8String.h"
#include "tiktoken.h"
#include "Timer.h"

using namespace std::literals;
using namespace TiktokenCpp;

std::ostream& PrintTokens(std::ostream& os, const std::vector<int32_t>& tokens)
{
    os << "[";
    for (std::size_t i = 0; i < tokens.size(); i++)
    {
        if (i != (tokens.size() - 1))
            os << tokens[i] << ", ";
        else
            os << tokens[i];
    }
    os << "]" << std::endl;

    return os;
}

std::ostream& PrintSymbols(std::ostream& os, const std::vector<std::string>& symbols)
{
    os << "[";
    for (std::size_t i = 0; i < symbols.size(); i++)
    {
        if (i != (symbols.size() - 1))
            os << symbols[i] << ", ";
        else
            os << symbols[i];
    }
    os << "]" << std::endl;

    return os;
}


int main()
{
    //Enumerate and download encoding files to local cache
#if 0
    try
    {
        std::string_view proxy = "..."; //proxy, if needed
        auto encodingnames = ListEncodingNames();
        for (const auto& name : encodingnames)
        {
            std::cout << "Downloading encoding file for " << name << "...";
            bool rtn = DownloadEncoding(name, proxy);
            std::cout << (rtn ? "successfully!" : "failed!") << std::endl;
        }
    }
    catch (...)
    {
        std::cout << "Downloading exception occured" << std::endl;
    }
#endif

    //Testing data:
    std::vector<std::string> texts = {
        {"hello world"},
        { "goodbye world" },
        {"tiktoken is great!"},
        {"Hi，试一下中文字符！"},
        {"Hello, 😀"},
        {""}
    };
    
    std::vector < std::vector<int32_t>> tokens = { 
        {15339, 1917}, 
        {19045, 29474, 1917}, 
        {83, 1609, 5963, 374, 2294, 0}, 
        {13347, 3922, 42421, 15120, 17297, 16325, 17161, 49491, 6447},
        {9906, 11, 91416},
        {}
    };

    //Get encoding object, using "cl100k_base" encoding file
    std::unique_ptr<TikToken> encoding = GetEncoding("cl100k_base");
    //std::unique_ptr<TikToken> encoding = EncodingForModel("GPT-4");

    //std::string tes = "Hello, 中国 😀";
    //std::string lotes = LocalMBCSFromUTF8Str(tes);
    //std::cout << lotes << std::endl;
    // 
    //For ordinary text, using quick function
    for (std::size_t i = 0; i < texts.size(); i++)
    {
#ifdef _WIN32
        std::cout << "Encoding: \"" << LocalMBCSFromUTF8Str(texts[i]) << "\"" << std::endl;
#else
        std::cout << "Encoding: \"" << texts[i] << "\"" << std::endl;
#endif
        auto enc_result = encoding->EncodeOrdinary(texts[i]);
        PrintTokens(std::cout, enc_result);
        assert(enc_result == tokens[i]);
        std::string dec_result = encoding->Decode(enc_result);
        assert(dec_result == texts[i]);
    }

    //For special text
    std::string text = "hello <|endoftext|>";
    std::cout << "Encoding: \"" << text << "\", with allowedSpecial: all , disallowedSpecial: all" << std::endl;
    //allowedSpecial: "all" , disallowedSpecial: "all" (default)
    auto enc = encoding->Encode(text, "all"); 
    PrintTokens(std::cout, enc);
    std::vector<int32_t> spec1{ 15339, 220, 100257 };
    assert(enc == spec1);
    auto dec = encoding->Decode(enc);
    assert(dec == text);
    
    text = "<|endoftext|>";
    std::cout << "Encoding: \"" << text << "\", with allowedSpecial: <|endoftext|> , disallowedSpecial: all" << std::endl;
    //allowedSpecial: "<|endoftext|>" , disallowedSpecial: "all"
    enc = encoding->Encode(text, AllowedSpecial{"<|endoftext|>"});
    PrintTokens(std::cout, enc);
    spec1 = { 100257 };
    assert(enc == spec1);
    dec = encoding->Decode(enc);
    assert(dec == text);

    text = "<|endoftext|>";
    std::cout << "Encoding: \"" << text << "\", with allowedSpecial: null, disallowedSpecial: null" << std::endl;
    //allowedSpecial: null (default), disallowedSpecial: null
    enc = encoding->Encode(text, AllowedSpecial{}, DisallowedSpecial{});
    PrintTokens(std::cout, enc);
    spec1 = { 27, 91, 8862, 728, 428, 91, 29 };
    assert(enc == spec1);
    dec = encoding->Decode(enc);
    assert(dec == text);

    //Tokens symbol test
    text = "tiktoken is great!";
    std::cout << "Symbols test for: \"" << text << "\"" << std::endl;
    auto tks = encoding->EncodeOrdinary(text);
    PrintTokens(std::cout, tks);
    auto symbols = encoding->TokenToSymbols(tks);
    //[t, ik, token,  is,  great, !]
    PrintSymbols(std::cout, symbols);

    //decode speed test
    std::vector < std::vector<int32_t>> dec_tokens = { 
        {15339, 1917}, {19045, 29474, 1917},
        {83, 1609, 5963, 374, 2294, 0}, 
        {13347, 3922, 42421, 15120, 17297, 16325, 17161, 49491, 6447},
        {16325, 86461, 17792, 70821, 55999, 34208, 29504, 3922, 31106, 111, 49409, 58911, 68464, 19361, 48044, 31809, 58911, 14276, 239, 3922, 14276, 239, 70349, 19361, 48044, 31809, 10890, 102, 5877, 104, 31809, 31958},
        {5451, 11, 499, 1440, 356, 2192, 355, 2947, 5979, 355, 374, 10388, 9354, 311, 279, 1274, 13},
        {396, 10402, 284, 281, 846, 17, 10938, 5921, 11, 320, 4977, 793, 17, 18220, 2434, 8, 11760, 11, 6812, 793, 17, 40470, 81002, 9182, 11, 1212, 7062, 11, 220, 15, 11, 2489, 1807, 11, 1808, 1237},
        {13997, 220, 59795, 46961, 17599, 223, 84565},
        {9906, 73958, 86461, 17792, 70821, 55999, 34208, 29504, 1917, 0},
        {27, 91, 8862, 728, 428, 91, 29},
        {22656, 74445, 94249, 35287, 15120, 98184, 40053, 11883, 19817, 228, 35722, 253, 9554, 16325, 17161, 83, 1609, 5963, 54872, 25287, 1811},
        {16906, 233, 25906, 102, 96455, 3922, 54581, 16906, 233, 25906, 102, 17161, 22656, 39404, 18476, 9554, 60455, 28873, 3922, 21043, 17161, 22656, 16906, 233, 25906, 102, 34547, 9554, 5963, 9039, 93132, 58318, 16906, 233, 25906, 102, 25580, 9554, 93132, 55030, 57106, 1811, 16906, 233, 25906, 102, 96455, 50266, 232, 31809, 3922, 67494, 17620, 6744, 235, 32648, 33764, 17161, 22656, 31968, 16882, 60455, 96455, 50266, 232, 45736, 1811, 33281, 247, 70349, 98739, 51504, 79, 1135, 74, 7806, 19967, 18184, 75146, 44368, 3922, 44388, 21043, 38, 2898, 17, 9554, 17620, 6744, 235, 32648, 3922, 38129, 16205, 225, 18184, 82042, 14888, 249, 1811},
        {68171, 33091, 31540, 90070, 97150, 16325, 17161, 19817, 228, 35722, 253, 71461, 10414, 255, 12774, 225, 17620, 6744, 235, 32648, 19361, 60632, 35304, 16906, 233, 25906, 102, 5963, 99502, 3922, 6271, 227, 11883, 38, 2898, 19, 31968, 16882, 32648, 16, 4, 9554, 6744, 235, 21405, 9039, 3922, 18655, 50928, 35287, 31968, 16882, 60455, 96455, 91272, 8239, 123, 1811, 220, 6744, 235, 21405, 9039, 34226, 31809, 37689, 17857, 111, 84949, 222, 90112, 23187, 50021, 42016, 33765, 9554, 34547, 12774, 255, 72456, 77195, 17297, 3922, 68171, 35304, 19361, 34226, 31809, 9554, 161, 113, 234, 17701, 23602, 224, 3922, 64531, 33014, 72456, 33765, 33857, 34226, 83747, 3922, 63212, 33091, 77913, 19361, 15120, 23187, 9554, 38743, 70203, 95399, 27479, 91272, 8239, 123, 1811, 50702, 254, 33091, 3922, 16325, 17161, 17620, 6744, 235, 32648, 66, 58, 16, 11, 605, 11, 508, 11, 1490, 60, 74, 19000, 16325, 17161, 19817, 228, 35722, 253, 17297, 58318, 38, 2898, 18, 13, 20, 14, 19, 33035, 11883, 31968, 16882, 32648, 34226, 77913, 31968, 16882, 5486, 38743, 70203, 95399, 27479, 91272, 8239, 123, 1811},
        {46281, 17905, 43032, 21405, 35083, 31540, 90070, 3922, 38, 2898, 18, 13, 20, 14, 19, 9554, 16906, 233, 25906, 102, 96455, 58318, 38, 2898, 17, 50021, 57106, 29172, 11239, 229, 95337, 2137, 71082, 38, 2898, 18, 13, 20, 14, 19, 9554, 6744, 235, 21405, 9039, 57106, 38, 2898, 17, 50285, 21601, 35287, 15120, 98406, 3922, 78388, 16906, 233, 25906, 102, 96455, 11881, 122, 164, 239, 245, 29172, 11239, 229, 1811, 74662, 16325, 17161, 73981, 42506, 17905, 10414, 255, 12774, 225, 9554, 66, 16, 74, 17620, 6744, 235, 32648, 16906, 233, 25906, 102, 96455, 57106, 38, 2898, 19, 29172, 11239, 229, 95337, 22, 13689, 66, 605, 74, 29172, 11239, 229, 95337, 1544, 13689, 66, 508, 74, 29172, 11239, 229, 95337, 966, 13689, 66, 1490, 74, 29172, 11239, 229, 95337, 1958, 4, 1811, 74662, 83947, 17161, 73981, 42506, 17905, 10414, 255, 12774, 225, 9554, 68, 16, 74, 17620, 6744, 235, 32648, 16906, 233, 25906, 102, 96455, 17297, 14309, 235, 95337, 3391, 4, 1811},
        {8085, 29, 16, 1811, 11883, 37507, 6701, 80426, 65571, 19361, 8729, 482, 220, 845, 31640, 22649, 20370, 49473, 3922, 27, 611, 281, 1822, 79, 62225, 1312, 5468, 76788, 13056, 61150, 1308, 38, 2390, 7672, 8, 612, 308, 5532, 26, 612, 5792, 26, 612, 5792, 26, 320, 15, 16141, 4728, 612, 26237, 26, 284, 320, 331, 8, 612, 23598, 26, 612, 1141, 26, 320, 331, 15011, 4937, 26, 284, 220, 15, 16141, 32303, 8, 366, 1437, 871, 674, 1312, 5468, 76788, 13056, 32721, 61150, 1308, 38, 2390, 7672, 8, 612, 308, 5532, 26, 612, 5792, 26, 612, 5792, 26, 320, 15, 16141, 4728, 612, 26237, 26, 284, 320, 331, 8, 612, 23598, 26, 612, 1141, 26, 320, 331, 15011, 4937, 26, 284, 220, 15, 83444, 1785, 8, 366, 1437, 871, 674, 1312, 5468, 76788, 13056, 26916, 61150, 1308, 38, 2390, 7672, 8, 612, 308, 5532, 26, 612, 5792, 26, 612, 5792, 26, 320, 15, 85089, 410, 612, 26237, 26, 284, 320, 331, 8, 612, 23598, 26, 612, 1141, 26, 320, 331, 15011, 4937, 26, 284, 220, 15, 16141, 32303, 8, 366, 611, 281, 30448, 281, 29, 17, 1811, 46281, 8729, 845, 50338, 16882, 9554, 39282, 39607, 30624, 17297, 3922, 27, 611, 281, 1822, 79, 62225, 1312, 5468, 76788, 70417, 61150, 1308, 38, 31404, 73070, 11, 3428, 8, 612, 308, 5532, 26, 4544, 1347, 29, 612, 308, 5532, 26, 612, 5792, 26, 11861, 1209, 14149, 81414, 50, 19, 2432, 12156, 8, 612, 23598, 26, 220, 15, 87, 2839, 1785, 8, 612, 26237, 26, 612, 4937, 26, 220, 605, 8, 765, 612, 5792, 26, 612, 5792, 26, 612, 5792, 26, 4544, 1347, 29, 612, 308, 5532, 26, 612, 5792, 26, 612, 5792, 26, 1819, 14149, 81414, 50, 19, 2432, 10516, 8, 612, 23598, 26, 220, 15, 87, 2839, 1785, 595, 489, 220, 15, 87, 1041, 410, 8, 366, 611, 281, 30448, 79, 29, 26130, 5232, 14149, 81414, 50, 19, 92382, 5232, 5399, 2687, 843, 530, 5468, 81414, 50, 19, 26, 366, 611, 281, 1822, 281, 29, 18, 1811, 18655, 50928, 12156, 73950, 34208, 10516, 73950, 9554, 8676, 237, 47200, 30624, 17297, 27, 611, 281, 29},
        {},
    };

    std::string rrr;
    Timer st(true);
    for (int round = 0; round < 1000; round++)
    {
        for (auto& token : dec_tokens)
        {
            rrr = encoding->Decode(token);
        }
    }
    
    auto spantime = st.GetMS();
    std::cout << "Span time: " << spantime << ", " << rrr <<std::endl;
}

/*
 * main.cpp
 *
 *  Created on: 2018年2月5日
 *      Author: nullpointer
 */

#include "ChiSquareKeyWordExtractor.h"
#include "StopWordFilter.h"
#include <algorithm>
#include <iostream>
#include "WordVectorJsonStorage.h"
#include <boost/algorithm/string/join.hpp>
#include "WordVectorLevelDBStroage.h"
#include <boost/range/adaptor/transformed.hpp>

int myrandom (int i) { return std::rand()%i;}

#define VAR(var) #var << " = " << var


int main()
{    
    int argc = 3;
    const char* argv[] = {"ShortTextClassfier", "淘宝刷单，保底100/天 ，适合宝妈、学生党，详情请联系QQ:1234312", "5"};
	if (argc != 3){
        std::cout << "proc <sms> <num of keywords>" << std::endl;
		return -1;
	}

	std::string default_stop_wrods = " \n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n\"\n#\n$\n%\n&\n\'\n(\n)\n*\n+\n,\n-\n--\n.\n..\n...\n......\n...................\n./\n//\n:\n://\n::\n;\n<\n=\n>\n>>\n?\n@\nA\nLex\n[\n\\\n]\n^\n_\n`\nexp\nsub\nsup\n|\n}\n~\n~~~~\n·\n×\n×××\nΔ\nΨ\nγ\nμ\nφ\nφ．\nВ\n—\n——\n———\n‘\n’\n’‘\n“\n”\n”，\n…\n……\n…………………………………………………③\n′∈\n′｜\n℃\nⅢ\n↑\n→\n∈［\n∪φ∈\n≈\n①\n②\n②ｃ\n③\n③］\n④\n⑤\n⑥\n⑦\n⑧\n⑨\n⑩\n──\n■\n▲\n\u3000\n、\n。\n〈\n〉\n《\n》\n》），\n」\n『\n』\n【\n】\n〔\n〕\n〕〔\n㈧\n︿\n！\n＃\n＄\n％\n＆\n＇\n（\n）\n）÷（１－\n）、\n＊\n＋\n＋ξ\n＋＋\n，\n－\n－β\n－－\n－［＊］－\n．\n／\n：\n；\n＜\n＜±\n＜Δ\n＜λ\n＜φ\n＜＜\n＝\n＝″\n＝☆\n＝（\n＝－\n＝［\n＝｛\n＞\n＞λ\n？\n＠\nＡ\nＬＩ\nＲ．Ｌ．\nＺＸＦＩＴＬ\n［\n［①①］\n［①②］\n［①③］\n［①④］\n［①⑤］\n［①⑥］\n［①⑦］\n［①⑧］\n［①⑨］\n［①Ａ］\n［①Ｂ］\n［①Ｃ］\n［①Ｄ］\n［①Ｅ］\n［①］\n［①ａ］\n［①ｃ］\n［①ｄ］\n［①ｅ］\n［①ｆ］\n［①ｇ］\n［①ｈ］\n［①ｉ］\n［①ｏ］\n［②\n［②①］\n［②②］\n［②③］\n［②④\n［②⑤］\n［②⑥］\n［②⑦］\n［②⑧］\n［②⑩］\n［②Ｂ］\n［②Ｇ］\n［②］\n［②ａ］\n［②ｂ］\n［②ｃ］\n［②ｄ］\n［②ｅ］\n［②ｆ］\n［②ｇ］\n［②ｈ］\n［②ｉ］\n［②ｊ］\n［③①］\n［③⑩］\n［③Ｆ］\n［③］\n［③ａ］\n［③ｂ］\n［③ｃ］\n［③ｄ］\n［③ｅ］\n［③ｇ］\n［③ｈ］\n［④］\n［④ａ］\n［④ｂ］\n［④ｃ］\n［④ｄ］\n［④ｅ］\n［⑤］\n［⑤］］\n［⑤ａ］\n［⑤ｂ］\n［⑤ｄ］\n［⑤ｅ］\n［⑤ｆ］\n［⑥］\n［⑦］\n［⑧］\n［⑨］\n［⑩］\n［＊］\n［－\n［］\n］\n］∧′＝［\n］［\n＿\nａ］\nｂ］\nｃ］\nｅ］\nｆ］\n｛\n｛－\n｜\n｝\n｝＞\n～\n～±\n～＋";
	std::vector<std::string> stop_words;
	boost::split(stop_words, default_stop_wrods, boost::is_any_of("\n"));
	StopWordFilter filter(stop_words);

	int n = ::atoi(argv[2]);
	boost::shared_ptr<ChiSquareKeyWordExtractor> kw_extractor = ChiSquareKeyWordExtractor::load("model.bin");
    WordVectorLevelDBStroage<double> wvec_stroage("./wordvector.bin");

	std::vector<std::pair<std::string, double> > kws =
			kw_extractor->get_top_keywords(filter.filter(argv[1]), n);

	std::vector<std::pair<std::string, double> >::const_iterator pos = kws.begin();
    WordVector<double> doc_vec(250);

	for (; pos != kws.end(); ++pos){
		std::cout  << "<" << pos->first << ", " << pos->second << ">" << std::endl;        
        doc_vec += wvec_stroage.get_wvec(pos->first) * kw_extractor->get_idf(pos->first);
	}       

	std::cout << std::endl;
    std::cout << "doc vector = " << doc_vec.to_string() << std::endl;

    return 0;
}

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

WordVector<double> get_doc_vec(std::string const& doc, int k,
                               ChiSquareKeyWordExtractor & extractor,
                               WordVectorLevelDBStroage<double> & wvec_stroage)
{
    std::vector<std::pair<std::string, double> > kws = extractor.get_top_keywords(doc, k);
    std::vector<std::pair<std::string, double> >::const_iterator pos = kws.begin();
    WordVector<double> doc_vec(250);//  dim = 250
    for (; pos != kws.end(); ++pos){
        try{
            doc_vec += wvec_stroage.get_wvec(pos->first) * extractor.get_idf(pos->first);
        }catch(std::invalid_argument const& e){
            continue;
        }
    }

    return doc_vec;
}

int main()
{    
    int argc = 4;
    const char* argv[] = {"Class", "淘宝刷单，保底300元/天，适合学生党、宝妈，详情联系马化腾12312142", "网络类诈骗主要包括网络购物、刷单、兼职、办理各项业务、冒充老板诈骗财务人员等", "5"};
    if (argc != 4){
        std::cout << "proc <sms1> <sms2> <num of keywords>" << std::endl;
		return -1;
	}

    std::string default_stop_wrods = "\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n\"\n#\n$\n%\n&\n\'\n(\n)\n*\n+\n,\n-\n--\n.\n..\n...\n......\n...................\n./\n//\n:\n://\n::\n;\n<\n=\n>\n>>\n?\n@\nA\nLex\n[\n\\\n]\n^\n_\n`\nexp\nsub\nsup\n|\n}\n~\n~~~~\n·\n×\n×××\nΔ\nΨ\nγ\nμ\nφ\nφ．\nВ\n—\n——\n———\n‘\n’\n’‘\n“\n”\n”，\n…\n……\n…………………………………………………③\n′∈\n′｜\n℃\nⅢ\n↑\n→\n∈［\n∪φ∈\n≈\n①\n②\n②ｃ\n③\n③］\n④\n⑤\n⑥\n⑦\n⑧\n⑨\n⑩\n──\n■\n▲\n\u3000\n、\n。\n〈\n〉\n《\n》\n》），\n」\n『\n』\n【\n】\n〔\n〕\n〕〔\n㈧\n︿\n！\n＃\n＄\n％\n＆\n＇\n（\n）\n）÷（１－\n）、\n＊\n＋\n＋ξ\n＋＋\n，\n－\n－β\n－－\n－［＊］－\n．\n／\n：\n；\n＜\n＜±\n＜Δ\n＜λ\n＜φ\n＜＜\n＝\n＝″\n＝☆\n＝（\n＝－\n＝［\n＝｛\n＞\n＞λ\n？\n＠\nＡ\nＬＩ\nＲ．Ｌ．\nＺＸＦＩＴＬ\n［\n［①①］\n［①②］\n［①③］\n［①④］\n［①⑤］\n［①⑥］\n［①⑦］\n［①⑧］\n［①⑨］\n［①Ａ］\n［①Ｂ］\n［①Ｃ］\n［①Ｄ］\n［①Ｅ］\n［①］\n［①ａ］\n［①ｃ］\n［①ｄ］\n［①ｅ］\n［①ｆ］\n［①ｇ］\n［①ｈ］\n［①ｉ］\n［①ｏ］\n［②\n［②①］\n［②②］\n［②③］\n［②④\n［②⑤］\n［②⑥］\n［②⑦］\n［②⑧］\n［②⑩］\n［②Ｂ］\n［②Ｇ］\n［②］\n［②ａ］\n［②ｂ］\n［②ｃ］\n［②ｄ］\n［②ｅ］\n［②ｆ］\n［②ｇ］\n［②ｈ］\n［②ｉ］\n［②ｊ］\n［③①］\n［③⑩］\n［③Ｆ］\n［③］\n［③ａ］\n［③ｂ］\n［③ｃ］\n［③ｄ］\n［③ｅ］\n［③ｇ］\n［③ｈ］\n［④］\n［④ａ］\n［④ｂ］\n［④ｃ］\n［④ｄ］\n［④ｅ］\n［⑤］\n［⑤］］\n［⑤ａ］\n［⑤ｂ］\n［⑤ｄ］\n［⑤ｅ］\n［⑤ｆ］\n［⑥］\n［⑦］\n［⑧］\n［⑨］\n［⑩］\n［＊］\n［－\n［］\n］\n］∧′＝［\n］［\n＿\nａ］\nｂ］\nｃ］\nｅ］\nｆ］\n｛\n｛－\n｜\n｝\n｝＞\n～\n～±\n～＋";
    std::stringstream user_stop_words_ss;
    std::ifstream stop_words_if("./dict/stop_words.utf8");
    user_stop_words_ss << stop_words_if.rdbuf();
    default_stop_wrods += user_stop_words_ss.str();
    std::vector<std::string> stop_words;
	boost::split(stop_words, default_stop_wrods, boost::is_any_of("\n"));
	StopWordFilter filter(stop_words);

	boost::shared_ptr<ChiSquareKeyWordExtractor> kw_extractor = ChiSquareKeyWordExtractor::load("model.bin");
    WordVectorLevelDBStroage<double> wvec_stroage("./wordvector.bin");

    int k = ::atoi(argv[3]);
    WordVector<double> sms1_vec = get_doc_vec(filter.filter(argv[1]), k, *kw_extractor, wvec_stroage);
    WordVector<double> sms2_vec = get_doc_vec(filter.filter(argv[2]), k, *kw_extractor, wvec_stroage);

    std::cout << "dot(sms1, sms2) = " << sms1_vec * sms2_vec << std::endl;
    return 0;
}

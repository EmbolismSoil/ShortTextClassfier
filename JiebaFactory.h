/*
 * JiebaFactory.h
 *
 *  Created on: 2018年2月5日
 *      Author: nullpointer
 */

#ifndef JIEBAFACTORY_H_
#define JIEBAFACTORY_H_

#include <boost/noncopyable.hpp>
#include <cppjieba/Jieba.hpp>
#include <boost/shared_ptr.hpp>

class JieBaFactory : private boost::noncopyable {
public:
	static boost::shared_ptr<cppjieba::Jieba> get_jieba(std::string k)
	{
		if (k == "default"){
			std::string const DICT_PATH ("dict/jieba.dict.utf8");
			std::string const HMM_PATH ("dict/hmm_model.utf8");
			std::string const USER_DICT_PATH("dict/user.dict.utf8");
			std::string const IDF_PATH("dict/idf.utf8");
			std::string const STOP_WORD_PATH("dict/stop_words.utf8");

			return boost::shared_ptr<cppjieba::Jieba>(
					new cppjieba::Jieba(DICT_PATH,
			        HMM_PATH,
			        USER_DICT_PATH,
			        IDF_PATH,
			        STOP_WORD_PATH));
		}else{
			throw std::runtime_error(std::string("not implement, k = ") + k);
		}
	}
};

#endif /* JIEBAFACTORY_H_ */

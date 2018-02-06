/*
 * WordVectorJsonStorage.h
 *
 *  Created on: 2018年2月5日
 *      Author: nullpointer
 */

#ifndef WORDVECTORJSONSTORAGE_H_
#define WORDVECTORJSONSTORAGE_H_

#include "WordVectorStorage.h"
#include <fstream>
#include <stdint.h>
#include <boost/format.hpp>
#include <json/json.h>

class WordVectorJsonStorage: public WordVectorStorage
{
public:
	WordVectorJsonStorage(std::string const& path):
		_ifile(path.c_str())
	{
		if (!_json_reader.parse(_ifile, _root)){
			throw std::runtime_error("failed in parse json");
		}
	}

	virtual std::vector<int64_t> const get_word_vector(std::string const& sms)
	{
		Json::Value vec;
		if (!(vec = _root[sms].isArray())){
			boost::format fmt("failed in parse json, <%s> is not an array");
			fmt % sms;
			throw std::runtime_error(fmt.str());
		}

		std::vector<int64_t> result;
		for (Json::Value::const_iterator pos = vec.begin(); pos != vec.end(); ++pos){
			result.push_back(pos->asInt64());
		}

		return result;
	}

private:
	std::ifstream _ifile;
	Json::Reader _json_reader;
	Json::Value _root;
};

#endif /* WORDVECTORJSONSTORAGE_H_ */

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
#include <boost/algorithm/string.hpp>
#include <wordvector.h>

template<class T>
class WordVectorJsonStorage;

template<>
class WordVectorJsonStorage<int64_t>: public WordVectorStorage<int64_t>
{
public:
	WordVectorJsonStorage(std::string const& path)
	{
		std::ifstream ifile(path.c_str());
		Json::CharReaderBuilder builder;
		std::string err;
		if (!Json::parseFromStream(builder, ifile, &_root, &err)){
			throw std::runtime_error("failed in parse json");
		}
	}

    virtual WordVector<int64_t>  get_wvec(std::string const& sms)
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

        WordVector<int64_t> wvec(result);
        return wvec;
	}



private:
	Json::Value _root;
};

template<>
class WordVectorJsonStorage<double>: public WordVectorStorage<double>
{
public:
	WordVectorJsonStorage(std::string const& path)
	{
		std::ifstream ifile(path.c_str());
		Json::CharReaderBuilder builder;
		std::string err;
		if (!Json::parseFromStream(builder, ifile, &_root, &err)){
			throw std::runtime_error("failed in parse json");
		}
	}

    virtual WordVector<double>  get_wvec(std::string const& sms)
	{
		Json::Value vec;
		if (!(vec = _root[sms].isArray())){
			boost::format fmt("failed in parse json, <%s> is not an array");
			fmt % sms;
			throw std::runtime_error(fmt.str());
		}

		std::vector<double> result;
		for (Json::Value::const_iterator pos = vec.begin(); pos != vec.end(); ++pos){
			result.push_back(pos->asDouble());
		}

        return WordVector<double>(result);
	}

private:
	Json::Value _root;
};

#endif /* WORDVECTORJSONSTORAGE_H_ */

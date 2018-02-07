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

	virtual std::vector<int64_t> const get_wvec(std::string const& sms)
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

	std::string get_wvec_as_string(std::string const& sms)
	{
		std::vector<int64_t> vec = get_wvec(sms);
		std::string str_vec;
		std::vector<std::string> str_vec_dim;
		std::stringstream ss;
		if (vec.empty()){
			return "[]";
		}

		for (std::vector<int64_t>::const_iterator pos = vec.begin(); pos != vec.end(); ++pos)
		{
			ss << *pos;
			str_vec_dim.push_back(ss.str());
			ss.clear();
		}

		str_vec = boost::join(str_vec_dim, ", ");
		boost::format fmt("[%s]");
		fmt % str_vec;
		return fmt.str();
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

	virtual std::vector<double> const get_wvec(std::string const& sms)
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

		return result;
	}

	std::string get_wvec_as_string(std::string const& sms)
	{
		std::vector<double> vec = get_wvec(sms);
		std::string str_vec;
		std::vector<std::string> str_vec_dim;
		std::stringstream ss;
		if (vec.empty()){
			return "[]";
		}

		for (std::vector<double>::const_iterator pos = vec.begin(); pos != vec.end(); ++pos)
		{
			ss << *pos;
			str_vec_dim.push_back(ss.str());
			ss.clear();
		}

		str_vec = boost::join(str_vec_dim, ", ");
		boost::format fmt("[%s]");
		fmt % str_vec;
		return fmt.str();
	}

private:
	Json::Value _root;
};

#endif /* WORDVECTORJSONSTORAGE_H_ */

/*
 * WordVectorLevelDBStroage.h
 *
 *  Created on: 2018年2月7日
 *      Author: nullpointer
 */

#ifndef WORDVECTORLEVELDBSTROAGE_H_
#define WORDVECTORLEVELDBSTROAGE_H_

#include "WordVectorStorage.h"
#include <boost/shared_ptr.hpp>
#include <json/json.h>
#include <leveldb/db.h>

template<class T>
class WordVectorLevelDBStroage;

template<>
class WordVectorLevelDBStroage<double>: public WordVectorStorage<double>
{
public:
	WordVectorLevelDBStroage(std::string const& path)
	{
		leveldb::DB* db;
		leveldb::Options opts;
		opts.create_if_missing = false;
		leveldb::Status status = leveldb::DB::Open(opts, path, &db);
		if (!status.ok()){
			throw std::invalid_argument(status.ToString());
		}

		_db = boost::shared_ptr<leveldb::DB>(db);
	}

	virtual ~WordVectorLevelDBStroage(){}
    virtual WordVector<double> get_wvec(std::string const& word)
	{
        std::string svec;
        _db->Get(leveldb::ReadOptions(), word, &svec);
        std::vector<double> vec =  _parse_vector(svec);
        WordVector<double> wvec(vec);
        return wvec;
	}

private:
	boost::shared_ptr<leveldb::DB> _db;
	Json::Reader _reader;

	std::vector<double> _parse_vector(std::string const& vec_json)
	{
		Json::Value vec;
		std::vector<double> result;

		if (!_reader.parse(vec_json, vec) || !vec.isArray()){
			throw std::invalid_argument("json format eror");
		}

		for (Json::Value::const_iterator pos = vec.begin(); pos != vec.end(); ++pos){
			result.push_back(pos->asDouble());
		}

		return result;
	}
};

#endif /* WORDVECTORLEVELDBSTROAGE_H_ */

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
#include <eigen3/Eigen/Dense>

template<class T, int _DIM=250>
class WordVectorLevelDBStroage;

template<int _DIM>
class WordVectorLevelDBStroage<double, _DIM>{
public:
    using Vector =  Eigen::VectorXd;
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
    virtual Vector get_wvec(std::string const& word)
	{
        std::string svec;
        _db->Get(leveldb::ReadOptions(), word, &svec);
        std::vector<double> vec =  _parse_vector(svec);
        if (vec.size() != _DIM){
            throw std::invalid_argument("dim error");
        }

        Vector wvec = Eigen::VectorXd::Map(&(*vec.begin()), _DIM);
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

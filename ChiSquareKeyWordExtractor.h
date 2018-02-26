/*
 * ChiSquareKeyWordExtractor.h
 *
 *  Created on: 2018年2月5日
 *      Author: nullpointer
 */

#ifndef CHISQUAREKEYWORDEXTRACTOR_H_
#define CHISQUAREKEYWORDEXTRACTOR_H_

#include <cppjieba/Jieba.hpp>
#include <vector>
#include <utility>
#include <boost/unordered_map.hpp>
#include <string>
#include <boost/noncopyable.hpp>
#include <set>
#include "CSVSampleDataReader.h"
#include "JiebaFactory.h"
#include <functional>
#include <algorithm>
#include <numeric>
#include <leveldb/db.h>
#include <assert.h>
#include <json/json.h>
#include <boost/scoped_ptr.hpp>
/*
 * 卡方检验特征提取算法:
 *  +------------------+---------+---------+-----+
 *  |     group        | class 1 | class 2 | sum |
 *  +------------------+---------+---------+-----+
 *  |   contain word   |    A    |    B    | A+B |
 *  +------------------+---------+---------+-----+
 *  |not contain word  |    C    |    D    | C+D |
 *  +------------------+---------+---------+-----+
 *  |       sum        |   A+C   |   B+D   |  N  |
 *  +------------------+---------+---------+-----+
 *
 *    CHI = N*(A*D-B*C) / (A+B)*(A+C)*(B+D)*(C+D)
 *    其中class 1, class 2 必须要是连续的整数
 * */

#define __BIN_CLS__ 0

class ChiSquareKeyWordExtractor : private boost::noncopyable {
public:
	typedef int CT;
	ChiSquareKeyWordExtractor(std::vector<std::pair<std::string, CT> > const& data, uint64_t cls):
		_N(data.size()),
		_cls(cls)
	{
		_jieba = JieBaFactory::get_jieba("default");
#if __BIN_CLS__
		_do_init(data);
#else
		_do_init_mat(data);
#endif
	}

	static boost::shared_ptr<ChiSquareKeyWordExtractor> load(std::string const& path)
	{

		leveldb::DB* db;
		leveldb::Options opts;
		opts.create_if_missing = false;
		leveldb::Status status = leveldb::DB::Open(opts, path, &db);
		if (!status.ok()){
			throw std::invalid_argument(status.ToString());
		}

		boost::scoped_ptr<leveldb::DB> db_guard(db);

		uint64_t N = 0;
		std::string N_str;
		db->Get(leveldb::ReadOptions(), "ChiSquareKeyWordExtractor::_N", &N_str);
		std::stringstream sN;
		sN << N_str;
		sN >> N;

		leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
		boost::unordered_map<std::string, std::vector<std::vector<uint64_t> > > mat;

		for (it->SeekToFirst(); it->Valid(); it->Next()){
			std::string word(it->key().ToString());
			if (word == "ChiSquareKeyWordExtractor::_N"){
				continue;
			}

			std::string model(it->value().ToString());
			std::vector<std::vector<uint64_t> > wmat;

			_parse_mat(model, wmat);
			if (wmat.empty()){
				throw std::invalid_argument("model is empty");
			}

			mat[word] = wmat;
		}

		boost::shared_ptr<ChiSquareKeyWordExtractor> extractor(new ChiSquareKeyWordExtractor(mat, N));
		return extractor;
	}

	void save(std::string const& path)
	{
		leveldb::DB* db;
		leveldb::Options opts;
		opts.create_if_missing = true;
		leveldb::Status status = leveldb::DB::Open(opts, path, &db);
		if (!status.ok()){
			throw std::invalid_argument(status.ToString());
		}

		std::stringstream N;
		N << _N;
		db->Put(leveldb::WriteOptions(), "ChiSquareKeyWordExtractor::_N", N.str());
		boost::scoped_ptr<leveldb::DB> db_guard(db);
		boost::unordered_map<std::string, std::vector<std::vector<uint64_t> > >::const_iterator pos = _mat.begin();
		for (; pos != _mat.end(); ++pos){
			std::string json_mat = _json_dump_mat(pos->second);
			db->Put(leveldb::WriteOptions(), pos->first, json_mat);
		}
	}

	virtual std::vector<std::pair<std::string, double> > get_threshold_keywords(std::string const& sms, double threshold)
	{
		std::vector<std::string> words;
		_jieba->Cut(sms, words, true);

		std::vector<std::pair<std::string, double> > result;
		std::set<std::string> uique_words(words.begin(), words.end());
		std::set<std::string>::const_iterator pos = uique_words.begin();

		double chi = 0.0;
		for (; pos != uique_words.end(); ++pos){
			if ((chi = _CHI(*pos)) < 0){
				continue;
			}

			if (chi < threshold){
				result.push_back(std::make_pair(*pos, chi));
			}
		}

		return result;
	}

	virtual std::vector<std::pair<std::string, double> > get_top_keywords(std::string const& sms, uint64_t k)
	{
		std::vector<std::string> words;
		_jieba->Cut(sms, words, true);

		std::vector<std::pair<std::string, double> > result;
		std::set<std::string> uique_words(words.begin(), words.end());
		std::set<std::string>::const_iterator pos = uique_words.begin();

		for (; pos != uique_words.end(); ++pos){

#if __BIN_CLS__
			uint64_t A = _contain_words[*pos][1];
			uint64_t B = _contain_words[*pos][0];
			uint64_t C = _not_contain_words[*pos][1];
			uint64_t D = _not_contain_words[*pos][0];
			double chi = _CHI(A, B , C, D);
#else
			double chi = _CHI(*pos);
			if (chi < 0){
				continue;
			}
#endif

			if (result.size() >= k){
				std::vector<std::pair<std::string, double> >::iterator rpos = result.begin();
				while (rpos != result.end()){
					if (rpos->second > chi){
						rpos = result.erase(rpos);
						result.push_back(std::make_pair(*pos, chi));
						break;
					}else{
						++rpos;
					}
				}
			}else{
				result.push_back(std::make_pair(*pos, chi));
			}
		}

		return result;
	}

	virtual double const get_chi(std::string const& word)
	{
		return _CHI(word);
	}

	virtual std::vector<std::vector<uint64_t> > const& get_mat(std::string const& word)
	{
		return _mat[word];
	}

    virtual double const get_idf(std::string const& word)
    {
        return _idf(word);
    }

	virtual ~ChiSquareKeyWordExtractor(){};

private:
	boost::shared_ptr<cppjieba::Jieba> _jieba;
	//key: string, value: count
	boost::unordered_map<std::string, boost::unordered_map<CT, uint64_t> > _contain_words;
	boost::unordered_map<std::string, boost::unordered_map<CT, uint64_t> > _not_contain_words;
	boost::unordered_map<CT, uint64_t> _cls_cnt;
	std::vector<std::string> _words;
	boost::unordered_map<std::string, std::vector<std::vector<uint64_t> > > _mat;
	uint64_t _N;
	uint64_t _cls;

	ChiSquareKeyWordExtractor(boost::unordered_map<std::string, std::vector<std::vector<uint64_t> > > const& mat, uint64_t const N):
		_N(N)
	{
		if (mat.empty()){
			throw std::invalid_argument("mat is empty");
		}

		_jieba = JieBaFactory::get_jieba("default");
		_mat = mat;
		_cls = mat.begin()->second.size();
	}

	double _CHI(std::string const& word)
	{
		if (_mat.find(word) == _mat.end()){
			return -1.0;
		}

		std::vector<std::vector<uint64_t> > const& mat = _mat[word];
		double chi = _CHI(mat, _N);

		return chi;
	}

	double _CHI(uint64_t A, uint64_t B, uint64_t C, uint64_t D)
	{
		uint64_t N = A + B + C + D;
		uint64_t above = (N * (A*D-B*C) * (A*D-B*C));
		uint64_t btm = ( (A+B)*(A+C)*(B+D)*(C+D));

		if (above == 0){
			return 0.0;
		}else if (btm == 0){
			return 1000000.0;
		}else{
			return  double(above)/ double(btm);
		}
	}

	double _CHI(std::vector<std::vector<uint64_t> > const& mat, uint64_t N)
	{
		double chi = 0;
		double dn = N;
		for (size_t x = 0; x < mat.size(); ++x){
			for (size_t y = 0; y < mat[0].size(); ++y){
				uint64_t x_sum = 0;
				uint64_t y_sum = 0;
				_get_sum(mat, x, y, x_sum, y_sum);
				double v = double(x_sum*y_sum) / double(dn); //计算理论值
				double error = v - double(mat[x][y]); //计算误差
				error *= error;
				if (error == 0){
					continue;
				}else if (v == 0){
					chi += 1000000.0;
				}else{
					chi += (error / v); //计算卡方值
				}
			}
		}

		return chi;
	}

	double _idf(std::string const& word)
	{
		if (_mat.find(word) == _mat.end()){
			return -1.0;
		}else{
			uint64_t x_sum = 0;
			uint64_t y_sum = 0;
			_get_sum(_mat[word], 0, 0, x_sum, y_sum);
			double idf = 1.0 / ::log(double(_N)/double(x_sum));
			return idf;
		}
	}

	void _get_sum(std::vector<std::vector<uint64_t> > const& mat,
			size_t const x, size_t const y, uint64_t &x_sum, uint64_t& y_sum)
	{
		x_sum = std::accumulate(mat[x].begin(), mat[x].end(), 0);
		y_sum = 0;
		for (size_t pos = 0; pos < mat.size(); ++pos){
			y_sum += mat[pos][y];
		}
	}

	void _do_init(std::vector<std::pair<std::string, CT> > const& data)
	{
		std::vector<std::pair<std::string, CT> >::const_iterator pos = data.begin();
		uint64_t N = data.size();

		for (; pos != data.end(); ++pos){
			std::pair<std::string, CT> sms = *pos;
			std::string content(sms.first);
			CT cls = sms.second;
			std::vector<std::string> words;

			_jieba->Cut(content, words, true);

			std::set<std::string> uique_words(words.begin(), words.end());
			std::set<std::string>::const_iterator wpos = uique_words.begin();

			for (; wpos != uique_words.end(); ++wpos){
				_contain_words[*wpos][cls] += 1;
			}

			_cls_cnt[cls] += 1;
		}

		boost::unordered_map<std::string, boost::unordered_map<CT, uint64_t> >::const_iterator cpos = _contain_words.begin();
		for (; cpos != _contain_words.end(); ++cpos){
			boost::unordered_map<CT, uint64_t> const& word_cls_cnt(cpos->second);
			boost::unordered_map<CT, uint64_t>::const_iterator wpos = word_cls_cnt.begin();

			for (; wpos != word_cls_cnt.end(); ++wpos){
				CT cls = wpos->first;
				uint64_t cls_cnt = wpos->second;
				_not_contain_words[cpos->first][cls] = _cls_cnt[cls] - cls_cnt;
			}
		}

		if (_cls_cnt.size() < 2){
			throw std::logic_error("kind of sample less than 2");
		}
	}

	/*
	 * 初始化卡方矩阵
	 * */
	void _do_init_mat(std::vector<std::pair<std::string, CT> > const& data)
	{
		std::vector<std::pair<std::string, CT> >::const_iterator pos = data.begin();
		for (; pos != data.end(); ++pos){
			_update_mat(*pos);
		}

		boost::unordered_map<std::string, std::vector<std::vector<uint64_t> > >::iterator mpos = _mat.begin();
		for (; mpos != _mat.end(); ++mpos){
			std::vector<std::vector<uint64_t> > &mat = mpos->second;
			for (size_t cls = 0; cls < mat.size(); ++cls){
				uint64_t cls_cnt = _cls_cnt[cls];
				uint64_t contain_cnt = mat[0][cls];

				mat[1][cls] = cls_cnt - contain_cnt;
			}
		}
	}

	/*更新卡方矩阵*/
	void _update_mat(std::pair<std::string, CT> const& data)
	{
		std::vector<std::string> words;
		std::string content = data.first;
		CT cls = data.second;
		_cls_cnt[cls] += 1;

		_jieba->Cut(content, words, true);

		std::set<std::string> uique_words(words.begin(), words.end());
		std::set<std::string>::const_iterator wpos = uique_words.begin();

		for (; wpos != uique_words.end(); ++wpos){
			std::vector<std::vector<uint64_t> >& w_mat = _mat[*wpos]; //这个词的卡方矩阵

			//初始化卡方矩阵为2*cls
			if (w_mat.empty()){
				w_mat.resize(2);
				w_mat[0].resize(_cls, 0);
				w_mat[1].resize(_cls, 0);
			}

			w_mat[0][cls] += 1;
		}
	}

	static bool _chi_pair_comparetor(std::pair<std::string, double> const& lhs, std::pair<std::string, double> const& rhs)
	{
		return lhs.second < rhs.second;
	}


	static std::string _json_dump_mat(std::vector<std::vector<uint64_t> > const& mat)
	{
	    Json::FastWriter writer;

		if (mat.empty()){
			throw std::invalid_argument("mat is empty");
		}

		Json::Value json_mat;
		std::vector<std::vector<uint64_t> >::const_iterator pos = mat.begin();

		for ( ; pos != mat.end(); ++pos){
			if (pos->empty()){
				throw std::invalid_argument("vec is empty");
			}
			Json::Value json_vec;
			for (std::vector<uint64_t>::const_iterator vpos = pos->begin(); vpos != pos->end(); ++vpos){
				json_vec.append(*vpos);
			}

			json_mat.append(json_vec);
		}

		return writer.write(json_mat);
	}

	static void _parse_mat(std::string const& json, std::vector<std::vector<uint64_t> > & mat)
	{
		Json::Reader reader;
		Json::Value root;

		if (!reader.parse(json, root)){
			throw std::invalid_argument("json format eror");
		}

		if (!root.isArray() || root.empty()){
			throw std::invalid_argument("json format eror");
		}

		for (Json::Value::const_iterator pos = root.begin(); pos != root.end(); ++pos){
			if (!pos->isArray() || pos->empty()){
				throw std::invalid_argument("json format eror");
			}else{
			    std::vector<uint64_t> vec;
			    for (Json::Value::const_iterator vpos = pos->begin(); vpos != pos->end(); ++vpos){
			    	vec.push_back(vpos->asUInt64());
			    }
			    mat.push_back(vec);
			}
		}
	}

};

#endif /* CHISQUAREKEYWORDEXTRACTOR_H_ */

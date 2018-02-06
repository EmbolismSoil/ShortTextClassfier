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
class ChiSquareKeyWordExtractor : private boost::noncopyable {
public:
	typedef bool CT;
	ChiSquareKeyWordExtractor(std::vector<std::pair<std::string, CT> > const& data)
	{
		_jieba = JieBaFactory::get_jieba("default");
		_do_init(data);
	}

	virtual std::vector<std::pair<std::string, double> > get_top_keywords(std::string const& sms, uint64_t k, bool order=false)
	{
		std::vector<std::string> words;
		_jieba->Cut(sms, words, true);

		std::vector<std::pair<std::string, double> > result;
		std::vector<std::string>::const_iterator pos = words.begin();

		for (; pos != words.end(); ++pos){
			uint64_t A = _contain_words[*pos][1];
			uint64_t B = _contain_words[*pos][0];
			uint64_t C = _not_contain_words[*pos][1];
			uint64_t D = _not_contain_words[*pos][0];

			double chi = _CHI(A, B, C, D);

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

		if (order){
			std::sort(result.begin(), result.end(), &ChiSquareKeyWordExtractor::_chi_pair_comparetor);
		}
		return result;
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

	double _CHI(uint64_t A, uint64_t B, uint64_t C, uint64_t D)
	{
		uint64_t N = A + B + C + D;
		uint64_t above = (N * (A*D-B*C));
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
			for (size_t y = 0; y < mat.size(); ++y){
				uint64_t x_sum = 0;
				uint64_t y_sum = 0;
				_get_sum(mat, x, y, x_sum, y_sum);
				double v = double(x_sum*y_sum) / double(dn); //计算理论值
				double error = v - double(mat[x][y]); //计算误差
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
			std::string content;
			content.swap(sms.first);
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
		for (; pos != data.begin(); ++pos){
			_update_mat(*pos);
		}

		boost::unordered_map<std::string, std::vector<std::vector<uint64_t> > >::iterator mpos = _mat.begin();
		for (; mpos != _mat.end(); ++mpos){
			std::vector<std::vector<uint64_t> > &mat = mpos->second;
			for (size_t cls = 0; cls < mat.size(); ++cls){
				mat[1][cls] = _cls_cnt[cls] - mat[0][cls];
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
			}

			if (w_mat[0].size() < cls){ //
				w_mat[0].resize(cls, 0);
			}

			w_mat[0][cls] += 1;
		}
	}

	static bool _chi_pair_comparetor(std::pair<std::string, double> const& lhs, std::pair<std::string, double> const& rhs)
	{
		return lhs.second < rhs.second;
	}
};

#endif /* CHISQUAREKEYWORDEXTRACTOR_H_ */

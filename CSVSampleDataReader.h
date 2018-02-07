/*
 * CSVSampleDataReader.h
 *
 *  Created on: 2018年2月5日
 *      Author: nullpointer
 */

#ifndef CSVSAMPLEDATAREADER_H_
#define CSVSAMPLEDATAREADER_H_

#include <vector>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>


class CSVSampleDataReader
{
public:
	CSVSampleDataReader(std::string const& path, std::string terminated = "\t"):
		_terminated(terminated),
		_f(path.c_str())
	{

	}

	std::vector<std::pair<std::string, int> > parse()
	{
		std::vector<std::pair<std::string, int> > result;
		std::string line;
		while(std::getline(_f, line)){
			boost::replace_all(line, "\r", "");
			std::vector<std::string> items;
			boost::split(items, line, boost::is_any_of(_terminated));
			if(items.size() != 2){
				continue;
			}

			std::string sms;
			std::string cls;
			sms.swap(items[0]);
			cls.swap(items[1]);

			boost::trim(sms);
			boost::trim(cls);
			int int_cls = 0;
			if (cls != "0"){
				int_cls = 1;
			}
			result.push_back(std::make_pair(sms, int_cls));
		}

		return result;
	}

private:
	std::string const _terminated;
	std::ifstream _f;
};


#endif /* CSVSAMPLEDATAREADER_H_ */

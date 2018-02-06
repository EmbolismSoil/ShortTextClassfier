/*
 * KeyWordExtractor.h
 *
 *  Created on: 2018年2月6日
 *      Author: nullpointer
 */

#ifndef KEYWORDEXTRACTOR_H_
#define KEYWORDEXTRACTOR_H_

#include <vector>
#include <utility>
#include <string>
#include <stdint.h>

class KeyWordExtractor
{
public:
	virtual ~KeyWordExtractor(){}
	virtual std::vector<std::pair<std::string, double> > get_top_keywords(std::string const& sms, uint64_t k, bool order=false) = 0;
};




#endif /* KEYWORDEXTRACTOR_H_ */

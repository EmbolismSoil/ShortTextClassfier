/*
 * WordVectorStorage.h
 *
 *  Created on: 2018年2月5日
 *      Author: nullpointer
 */

#ifndef WORDVECTORSTORAGE_H_
#define WORDVECTORSTORAGE_H_

#include <string>
#include <stdint.h>
#include <vector>

class WordVectorStorage{
public:
	virtual ~WordVectorStorage(){}
	virtual std::vector<int64_t> const get_word_vector(std::string const& sms) = 0;
};

#endif /* WORDVECTORSTORAGE_H_ */

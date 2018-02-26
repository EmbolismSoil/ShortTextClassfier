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
#include <wordvector.h>

template<class T>
class WordVectorStorage{
public:
	virtual ~WordVectorStorage(){}
    virtual WordVector<T> get_wvec(std::string const& sms) = 0;
};

#endif /* WORDVECTORSTORAGE_H_ */

/*
 * SmsClassifier.h
 *
 *  Created on: 2018年2月5日
 *      Author: nullpointer
 */

#ifndef SMSCLASSIFIER_H_
#define SMSCLASSIFIER_H_

#include <vector>
#include <string>
#include "KeyWordExtractor.h"
#include <boost/shared_ptr.hpp>
#include "WordVectorStorage.h"

template<class FT, class CT>
class SmsClassifier {
public:
	SmsClassifier(boost::shared_ptr<KeyWordExtractor> extractor,
					boost::shared_ptr<WordVectorStorage<FT> > wvec_stroage):
		_extractor(extractor),
		_wvec_stroage(wvec_stroage)
   {

   }

	virtual ~SmsClassifier();
	virtual void train(std::vector<std::pair<std::vector<FT>, CT> > const& ) = 0;
	virtual CT const predict(std::vector<FT> const&) = 0;

private:
	boost::shared_ptr<KeyWordExtractor> _extractor;
	boost::shared_ptr<WordVectorStorage<FT> > _wvec_stroage;
};

#endif /* SMSCLASSIFIER_H_ */

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


template<class FT, class CT>
class SmsClassifier {
public:
	SmsClassifier();
	virtual ~SmsClassifier();
	virtual CT const classify(std::vector<FT> const& features);
	virtual void train(std::vector<std::pair<std::string, CT> > const& data);
	virtual void train(std::string const& csv_path);
};

#endif /* SMSCLASSIFIER_H_ */

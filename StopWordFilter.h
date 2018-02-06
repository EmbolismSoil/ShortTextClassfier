#ifndef __STOP_WORD_FILTER_H__
#define __STOP_WORD_FILTER_H__

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/noncopyable.hpp>

class StopWordFilter: private boost::noncopyable
{
public:
	StopWordFilter(std::vector<std::string> const& stopwords):
		_stop_words(stopwords)
	{

	}

	std::string filter(std::string const& sms)
	{
	    if (sms.empty()){
	    	return sms;
	    }

	    std::string filterd_sms(sms);
	    std::vector<std::string>::const_iterator pos = _stop_words.begin();

	    for ( ; pos != _stop_words.end(); ++pos){
	    	boost::replace_all(filterd_sms, *pos, "");
	    }

	    return filterd_sms;
	}

	virtual ~StopWordFilter(){}

private:
	std::vector<std::string> const& _stop_words;
};

#endif

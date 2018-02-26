#ifndef WORDVECTOR_H
#define WORDVECTOR_H

#include <vector>
#include <stdexcept>
#include <exception>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <stdio.h>

template<class T>
class WordVector
{
public:
    WordVector(int dim);
    WordVector(std::vector<T> const& vec);
    WordVector operator + (WordVector const& rhs);
    WordVector& operator += (WordVector const& rhs);
    WordVector operator - (WordVector const& rhs);
    WordVector operator * (typename std::vector<T>::value_type num);
    typename std::vector<T>::value_type operator * (WordVector const& rhs);
    std::string const to_string();

private:
    std::vector<T> _vec;
    WordVector(){}
};


template<class T>
WordVector<T>::WordVector(int dim):
    _vec(dim, 0)
{
    if (dim <= 0)
    {
        throw std::invalid_argument("Dim error");
    }
}

template<class T>
WordVector<T>::WordVector(const std::vector<T> &vec):
    _vec(vec)
{

}

template<class T>
WordVector<T> WordVector<T>::operator+(const WordVector &rhs)
{
    if (rhs._vec.size() != _vec.size() || _vec.empty()){
        throw std::invalid_argument("Dim error");
    }

    WordVector<T> vec;
    for (typename std::vector<T>::size_type pos = 0; pos != _vec.size(); ++pos)
    {
        vec._vec.push_back(_vec[pos] + rhs._vec[pos]);
    }

    return vec;
}

template<class T>
WordVector<T> &WordVector<T>::operator +=(const WordVector &rhs)
{
    if (rhs._vec.size() != _vec.size() || _vec.empty()){
        throw std::invalid_argument("Dim error");
    }

    WordVector<T>& vec = *this;
    for (typename std::vector<T>::size_type pos = 0; pos != _vec.size(); ++pos)
    {
        vec._vec[pos] = (_vec[pos] + rhs._vec[pos]);
    }

    return vec;
}

template<class T>
WordVector<T> WordVector<T>::operator- (const WordVector &rhs)
{
    if (rhs._vec.size() != _vec.size() || _vec.empty()){
        throw std::invalid_argument("Dim error");
    }

    WordVector<T> vec;
    for (typename std::vector<T>::size_type pos = 0; pos != _vec.size(); ++pos)
    {
        vec._vec.push_back(_vec[pos] - rhs._vec[pos]);
    }

    return vec;
}

template<class T>
WordVector<T> WordVector<T>::operator *(typename std::vector<T>::value_type num)
{
    WordVector<T> vec;
    for (typename std::vector<T>::const_iterator pos = _vec.begin(); pos != _vec.end(); ++pos){
        vec._vec.push_back(*pos * num);
    }

    return vec;
}

template<class T>
typename std::vector<T>::value_type WordVector<T>::operator *(const WordVector &rhs)
{
    if (rhs._vec.size() != _vec.size() || _vec.empty()){
        throw std::invalid_argument("Dim error");
    }

    typename std::vector<T>::value_type dot = 0;
    for (typename std::vector<T>::size_type pos = 0; pos != _vec.size(); ++pos)
    {
        dot += _vec[pos] * rhs._vec[pos];
    }

    return dot;
}

template<class T>
const std::string WordVector<T>::to_string()
{
    std::stringstream ss;
    std::vector<std::string> s_vec;
    for (typename std::vector<T>::const_iterator pos = _vec.begin(); pos != _vec.end(); ++pos){
       ss << *pos;
       s_vec.push_back(ss.str());
       ss.str("");
    }

    std::string vec = boost::join(s_vec, ", ");
    boost::format fmt("[%s]");
    fmt % vec;
    return fmt.str();
}

#endif // WORDVECTOR_H

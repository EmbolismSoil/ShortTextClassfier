#ifndef SVMCLASSIFIER_H
#define SVMCLASSIFIER_H

#include <svm.h>
#include <vector>
#include <boost/shared_ptr.hpp>

class SVMClassifier
{
public:
    SVMClassifier();
    // [a, b, c, d, e, f, g, ...]
    double predict(std::vector<double> const& feat);
    void train(std::vector<std::vector<double> > const& feat, std::vector<double> & labels);
    void save_model(std::string const& path);

private:
    boost::shared_ptr<svm_model> _model;
    svm_parameter _paramter;
};

#endif // SVMCLASSIFIER_H

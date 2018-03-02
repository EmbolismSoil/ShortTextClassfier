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
    double predict(std::vector<svm_node> const& feat);
    void train(std::vector<svm_node> const& feat);

private:
    boost::shared_ptr<svm_model> _model;
};

#endif // SVMCLASSIFIER_H
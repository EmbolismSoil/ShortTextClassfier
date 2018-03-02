#include "svmclassifier.h"

SVMClassifier::SVMClassifier()
{

}

double SVMClassifier::predict(const std::vector<svm_node> &feat)
{
    return svm_predict(_model.get(), &(*feat.begin()));
}

#include "svmclassifier.h"
#include <stdexcept>

SVMClassifier::SVMClassifier()
{
    _paramter.svm_type = C_SVC;
    _paramter.kernel_type = RBF;
    _paramter.degree = 3;
    _paramter.coef0 = 0;
    _paramter.nu = 0.5;
    _paramter.cache_size = 100;
    _paramter.C = 1;
    _paramter.eps = 1e-3;
    _paramter.p = 0.1;
    _paramter.shrinking = 1;
    _paramter.probability = 0;
    _paramter.nr_weight = 0;
    _paramter.weight_label = NULL;
    _paramter.weight = NULL;
}

double SVMClassifier::predict(const std::vector<double> &feat)
{
    struct svm_node nodes[feat.size()];

    for (auto i = 0; i < feat.size(); ++i){
        nodes[i].index = i;
        nodes[i].value = feat[i];
    }

    return svm_predict(_model.get(), nodes);
}

void SVMClassifier::train(const std::vector<std::vector<double> > &feat,  std::vector<double> &labels)
{
    //doubel to svm_noeds
    if (feat.size() <= 0 || feat[0].size() <= 0){
        throw std::invalid_argument("feat dim error");
    }

    const std::vector<double> &tmp = feat[0];
    const std::vector<std::vector<double> >::size_type x(feat.size());
    const std::vector<double>::size_type y(tmp.size());

    auto data = new struct svm_node*[x];
    for (auto i = 0; i < x; ++i){
        data[i] = new struct svm_node[y];
    }

    for (auto i = 0; i < feat.size(); ++i){
        for (auto j = 0; j < feat[0].size(); ++j){
            struct svm_node node;
            node.index = j;
            node.value = feat[i][j];
            data[i][j] = node;
        }
    }

    struct svm_problem problem;
    problem.l = feat.size();
    problem.x = data;
    problem.y = &(*labels.begin());

    _model = boost::shared_ptr<svm_model>(svm_train(&problem, &_paramter));

    for (auto i = 0; i < x; ++i){
        delete data[i];
    }

    delete data;
}

void SVMClassifier::save_model(const std::string &path)
{
    svm_save_model(path.c_str(), _model.get());
}

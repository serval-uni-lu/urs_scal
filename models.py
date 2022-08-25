#!/usr/bin/env python3
import pandas as pd
from sklearn.svm import SVC
from sklearn.svm import SVR
from sklearn.svm import LinearSVC
from sklearn.linear_model import Ridge
from sklearn.model_selection import train_test_split
from sklearn.model_selection import LeaveOneOut
from sklearn.metrics import accuracy_score
from sklearn.metrics import r2_score
from sklearn import metrics
from sklearn.preprocessing import StandardScaler
from sklearn.preprocessing import Normalizer
from sklearn.preprocessing import PolynomialFeatures
from sklearn.pipeline import make_pipeline
from sklearn.linear_model import LinearRegression
from sklearn.cluster import KMeans
from sklearn import tree
from sklearn import ensemble
from sklearn import neighbors
from sklearn import linear_model
from sklearn import neural_network
from sklearn.model_selection import cross_val_score
import statsmodels.api as sm
from statsmodels.stats.outliers_influence import variance_inflation_factor
import numpy as np

import sys

# import matplotlib.pyplot as mpl
# import scipy.stats as stats

deff = pd.read_csv("data/deff.csv", skipinitialspace = True, index_col = 'file')
fpt = pd.read_csv("data/fpt.csv", skipinitialspace = True, index_col = 'file')
patoh = pd.read_csv("data/patoh_9.csv", skipinitialspace = True, index_col = 'file')
# d4 = pd.read_csv("data/d4_001_450.csv", skipinitialspace = True, index_col = 'file')
# d4 = pd.read_csv("d4_mod_001_450.csv", skipinitialspace = True, index_col = 'file')
eqv = pd.read_csv("data/eqv.csv", skipinitialspace = True, index_col = 'file')
z3 = pd.read_csv("data/z3.csv", skipinitialspace = True, index_col = 'file')
# ms = pd.read_csv("data/minisat.csv", skipinitialspace = True, index_col = 'file')
nb_cls = pd.read_csv("data/nb_clauses.csv", skipinitialspace = True, index_col = 'file')
# nb_cls = pd.read_csv("data/3sat.eqv.csv", skipinitialspace = True, index_col = 'file')
# sat3 = pd.read_csv("data/3sat.csv", skipinitialspace = True, index_col = 'file')
mis = pd.read_csv("data/mis.csv", skipinitialspace = True, index_col = 'file')
tw = pd.read_csv("data/tw.csv", skipinitialspace = True, index_col = 'file')


# z3 = bsat.rename(columns={'state' : 'state_z3', 'mem': 'mem_z3', 'time': 'time_z3'}, inplace = False)

spur = pd.read_csv("data/stat_sharpSAT_2022-08-11.csv", skipinitialspace = True, index_col = 'file')
ug3 = pd.read_csv("data/stat_unigen3_2022-08-16.csv", skipinitialspace = True, index_col = 'file')
d4 = pd.read_csv("data/stat_d4_2022-08-11.csv", skipinitialspace = True, index_col = 'file')
sharpSAT = pd.read_csv("data/stat_sharpSAT_2022-08-11.csv", skipinitialspace = True, index_col = 'file')

data = eqv
data = data.join(mis, rsuffix = '_mis', on = 'file')
data = data.join(tw, rsuffix = '_tw', on = 'file')
data = data.join(deff, rsuffix = '_deff', on = 'file')
data = data.join(fpt, rsuffix = '_fpt', on = 'file')
data = data.join(patoh, rsuffix = '_patoh', on = 'file')
data = data.join(nb_cls, rsuffix = '_nb_cls', on = 'file')
# data = data.join(sat3, rsuffix = '_3sat', on = 'file')
data = data.join(z3, rsuffix = '_z3', on = 'file')
# data = data.join(ms, rsuffix = '_ms', on = 'file')
# data = data.join(tw, rsuffix = '_tw', on = 'file')

# data = data[data.tw.notnull()]
data.dropna(inplace = True)
data.to_csv('m.csv')

def c_loo(X, Y, clf, t):
    loo = LeaveOneOut()
    ype = np.zeros(len(Y))

    for tr, te in loo.split(X):
        X_train, X_test = X[tr], X[te]
        y_train, y_test = Y[tr], Y[te]

        clf = clf.fit(X_train, y_train)
        ype[te] = clf.predict(X_test)[0]

    print("### " + t + "\n")
    print(metrics.classification_report(Y, ype, digits = 5))
    print("")

    tn, fp, fn, tp = metrics.confusion_matrix(Y, ype).ravel()
    print("TN: " + str(tn))
    print("TP: " + str(tp))
    print("FN: " + str(fn))
    print("FP: " + str(fp))
    print("")

    # return metrics.accuracy_score(Y, ype)
    return metrics.f1_score(Y, ype)

def class_loo(data, title, inputs, mt, mm):
    print("## " + title + "\n")
    res = {}

    # data = data[data.state != "mem"]
    nb = len(data)
    nbt = len(data[(data.time >= mt) | (data.mem >= mm)])

    print("nb elems: " + str(nb))
    print("nb to: " + str(nbt))
    print("cnst precision: " + str(max(nbt / nb, (nb - nbt) / nb))) 

    X = data[inputs].to_numpy()
    scaler = StandardScaler()
    scaler.fit(X)
    X_ = scaler.transform(X)
    Y = (data['time'].ge(mt).to_numpy()) | (data['mem'].ge(mm).to_numpy())

    res["cnst 0"] = metrics.f1_score(Y, np.zeros(len(Y)))
    res["cnst 1"] = metrics.f1_score(Y, np.zeros(len(Y)) + 1)

    res["DT"] = c_loo(X, Y, tree.DecisionTreeClassifier(), "DT")
    res["RF 10"] = c_loo(X, Y, ensemble.RandomForestClassifier(n_estimators=10), "RF 10")
    res["RF 100"] = c_loo(X, Y, ensemble.RandomForestClassifier(n_estimators=100), "RF 100")
    res["RF 1000"] = c_loo(X, Y, ensemble.RandomForestClassifier(n_estimators=1000), "RF 1000")
    # res["RF 10000"] = c_loo(X, Y, ensemble.RandomForestClassifier(n_estimators=10000), "RF 10000")
    # res["RF 100000"] = c_loo(X, Y, ensemble.RandomForestClassifier(n_estimators=100000), "RF 100000")
    # res["RF 1000000"] = c_loo(X, Y, ensemble.RandomForestClassifier(n_estimators=1000000), "RF 1000000")
    # res["ERF 10"] = c_loo(X, Y, ensemble.ExtraTreesClassifier(n_estimators=10), "ERF 10")
    # res["ERF 100"] = c_loo(X, Y, ensemble.ExtraTreesClassifier(n_estimators=100), "ERF 100")
    # res["ERF 1000"] = c_loo(X, Y, ensemble.ExtraTreesClassifier(n_estimators=1000), "ERF 1000")
    # res["ABC 10"] = c_loo(X, Y, ensemble.AdaBoostClassifier(n_estimators=10), "ABC 10")
    # res["ABC 100"] = c_loo(X, Y, ensemble.AdaBoostClassifier(n_estimators=100), "ABC 100")
    # res["ABC 1000"] = c_loo(X, Y, ensemble.AdaBoostClassifier(n_estimators=1000), "ABC 1000")
    # res["GBC 10"] = c_loo(X, Y, ensemble.GradientBoostingClassifier(n_estimators=10), "GBC 10")
    # res["GBC 100"] = c_loo(X, Y, ensemble.GradientBoostingClassifier(n_estimators=100), "GBC 100")
    # res["GBC 1000"] = c_loo(X, Y, ensemble.GradientBoostingClassifier(n_estimators=1000), "GBC 1000")
    # res["HGBC 1"] = c_loo(X, Y, ensemble.HistGradientBoostingClassifier(max_iter = 1_000), "HGBC 1")
    # res["HGBC 10"] = c_loo(X, Y, ensemble.HistGradientBoostingClassifier(max_iter = 10_000), "HGBC 10")
    # res["HGBC 100"] = c_loo(X, Y, ensemble.HistGradientBoostingClassifier(max_iter = 100_000), "HGBC 100")
    # res["HGBC 1000"] = c_loo(X, Y, ensemble.HistGradientBoostingClassifier(max_iter = 1_000_000), "HGBC 1000")
    # res["HGBC 10000"] = c_loo(X, Y, ensemble.HistGradientBoostingClassifier(max_iter = 10_000_000), "HGBC 10000")
    # res["KNC 1"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 1), "KNC 1")
    # res["KNC 2"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 2), "KNC 2")
    # res["KNC 3"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 3), "KNC 3")
    # res["KNC 4"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 4), "KNC 4")
    # res["KNC 5"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 5), "KNC 5")
    # res["KNC 6"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 6), "KNC 6")
    # res["KNC 7"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 7), "KNC 7")
    # res["KNC 8"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 8), "KNC 8")
    # res["KNC 9"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 9), "KNC 9")
    # res["KNC 10"] = c_loo(X, Y, neighbors.KNeighborsClassifier(n_neighbors = 10), "KNC 10")
    # res["SGDC"] = c_loo(X, Y, linear_model.SGDClassifier(max_iter = 1_000_000), "SGDC")
    # res["SVC l"] = c_loo(X, Y, SVC(kernel = 'linear', max_iter = 1_000_000, C = 100), "SVC l")
    # res["SVC l scaled"] = c_loo(X_, Y, SVC(kernel = 'linear', max_iter = 1_000_000, C = 100), "SVC l scaled")
    # res["SVC rbf"] = c_loo(X, Y, SVC(kernel = 'rbf', max_iter = 10_000_000, C = 100), "SVC rbf")
    # res["SVC rbf scaled"] = c_loo(X_, Y, SVC(kernel = 'rbf', max_iter = 10_000_000, C = 100), "SVC rbf scaled")
    # res["SVC poly 2"] = c_loo(X_, Y, SVC(kernel = 'poly', degree = 2, max_iter = 1_000_000, C = 100), "SVC poly 2")
    # res["SVC poly 3"] = c_loo(X_, Y, SVC(kernel = 'poly', degree = 3, max_iter = 1_000_000, C = 100), "SVC poly 3")
    # res["SVC poly 4"] = c_loo(X_, Y, SVC(kernel = 'poly', degree = 4, max_iter = 1_000_000, C = 100), "SVC poly 4")
    # res["SVC poly 5"] = c_loo(X_, Y, SVC(kernel = 'poly', degree = 5, max_iter = 1_000_000, C = 100), "SVC poly 5")
    # res["SVC poly 6"] = c_loo(X_, Y, SVC(kernel = 'poly', degree = 6, max_iter = 1_000_000, C = 100), "SVC poly 6")
    # res["SVC poly 7"] = c_loo(X_, Y, SVC(kernel = 'poly', degree = 7, max_iter = 1_000_000, C = 100), "SVC poly 7")
    # res["SVC poly 8"] = c_loo(X_, Y, SVC(kernel = 'poly', degree = 8, max_iter = 1_000_000, C = 100), "SVC poly 8")
    # res["SVC poly 9"] = c_loo(X_, Y, SVC(kernel = 'poly', degree = 9, max_iter = 1_000_000, C = 100), "SVC poly 9")
    # res["SVC poly 10"] = c_loo(X_, Y, SVC(kernel = 'poly', degree = 10, max_iter = 1_000_000, C = 100), "SVC poly 10")
    # res["SVC sigmoid"] = c_loo(X_, Y, SVC(kernel = 'sigmoid', max_iter = 1_000_000, C = 100), "SVC sigmoid")
    # res["MLP"] = c_loo(X, Y, neural_network.MLPClassifier(hidden_layer_sizes=(5, 2), solver='lbfgs', max_iter = 100000), "MLP")
    # res["MLP 100"] = c_loo(X_, Y, neural_network.MLPClassifier(hidden_layer_sizes=(100), solver='lbfgs', max_iter = 100000), "MLP 100")
    # res["MLP 100.100"] = c_loo(X_, Y, neural_network.MLPClassifier(hidden_layer_sizes=(100, 100), solver='lbfgs', max_iter = 100000), "MLP 100.100")
    # res["MLP 5.2"] = c_loo(X_, Y, neural_network.MLPClassifier(hidden_layer_sizes=(5, 2), solver='lbfgs', max_iter = 100000), "MLP 5.2")
    # res["MLP 6.5.4.3.2.1"] = c_loo(X_, Y, neural_network.MLPClassifier(hidden_layer_sizes=(6, 5, 4, 3, 2, 1), solver='lbfgs', max_iter = 100000), "MLP 6.5")
    # res["MLP 6.4.2"] = c_loo(X_, Y, neural_network.MLPClassifier(hidden_layer_sizes=(6, 4, 2), solver='lbfgs', max_iter = 100000), "MLP 6.4")
    # res["MLP scaled"] = c_loo(X_, Y, neural_network.MLPClassifier(hidden_layer_sizes=(5, 2), solver='lbfgs', max_iter = 100000), "MLP scaled")

    print(res)
    print("\n")
    return res

def show_VIF(data, keys):
    print("## VIF\n")
    X = data[keys].to_numpy()
    for i in range(0, len(keys)):
        print(keys[i] + ": " + str(variance_inflation_factor(X, i)))
    print("\n")

print("""+++
title = "SVM predictor"
date = "2022-08-16"
description = "SVM predictor"
katex = false
autonumbering = true
+++\n\n""")

ipk = ['#var', 'deff', '#mis', '#eqv']
samplers = {'\\spur': spur, '\\unigen': ug3, 'D4': d4, 'sharpSAT': sharpSAT}
show_VIF(data, ipk)
# ipk = ['#eqv']

mt = 1800
mm = 4*10**6

r = {}
for name in samplers:
    sampler = samplers[name]
    r[name] = class_loo(data.join(sampler, rsuffix = '_spur', on = 'file'), "Sampler time analysis " + name, ipk, mt, mm)

samplers = {'\\spur fm': spur, '\\unigen fm': ug3, 'D4 fm': d4, 'sharpSAT fm': sharpSAT}
data = data.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
print(data)

for s in samplers:
    r[s] = class_loo(data.join(samplers[s], rsuffix = '_sam', on = 'file'), "Sampler time analysis " + s, ipk, mt, mm)

print(pd.DataFrame(r) * 100)
pd.DataFrame(r).to_csv("models.csv")

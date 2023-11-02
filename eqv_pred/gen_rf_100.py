#!/usr/bin/env python3
import pandas as pd
import graphviz
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

spur = pd.read_csv("data/stat_spur_2022-08-11.csv", skipinitialspace = True, index_col = 'file')
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

def run_poly(data, title, key):
    print("## " + title + "\n")

    data = data[data.state == "done"]
    nb = len(data)

    print("nb elems: " + str(nb))

    X = data[['#var', '#clause', 'tw', '#mis', '#eqv']].to_numpy()
    # X = data[['tw', '#eqv']].to_numpy()
    Y = np.log2(data[key].to_numpy())
    # Y = data[key].to_numpy()

    loo = LeaveOneOut()
    print("nb splits: " + str(loo.get_n_splits(X)))
    ype = np.zeros(len(Y), dtype = float)

    for i in range(1, 5):
        transformer = PolynomialFeatures(degree=i, include_bias=False)
        transformer.fit(X)
        X_ = transformer.transform(X)
        clf = make_pipeline(StandardScaler(), LinearRegression())
        clf = LinearRegression()
        m = clf.fit(X_, Y)
        r_sq = m.score(X_, Y)

        print(str(i) + " rq: " + str(r_sq))

    print("\n")

def run_ols(data, title, inputs, key):
    print("## " + title + "\n")

    data = data[data.state == "done"]
    nb = len(data)

    print("nb elems: " + str(nb))

    X = data[inputs].to_numpy()
    X = sm.add_constant(X)
    # X = data[['tw', '#eqv']].to_numpy()
    Y = np.log2(data[key].to_numpy())
    # Y = data[key].to_numpy()

    model = sm.OLS(Y, X)
    results = model.fit()
    print(results.summary())
    print("")

fig = 0
def run_loo(data, title, inputs, key):
    global fig
    print("## " + title + "\n")

    data = data[data.state == "done"]
    nb = len(data)

    print("nb elems: " + str(nb))
    print("min Y: " + str(np.min(data[key].to_numpy())))
    print("max Y: " + str(np.max(data[key].to_numpy())))
    print("mean Y: " + str(np.mean(data[key].to_numpy())))

    X = data[inputs].to_numpy()
    # X = data[['tw', '#eqv']].to_numpy()
    Y = np.log2(data[key].to_numpy())
    Y = data[key].to_numpy()

    loo = LeaveOneOut()
    print("nb splits: " + str(loo.get_n_splits(X)) + "\n")
    ype = np.zeros(len(Y), dtype = float)

    # transformer = PolynomialFeatures(degree=3, include_bias=False)
    # transformer.fit(X)
    # X_ = transformer.transform(X)
    X_ = X

    ype = np.zeros(len(Y))

    for tr, te in loo.split(X_):
        X_train, X_test = X_[tr], X_[te]
        y_train, y_test = Y[tr], Y[te]
        # clf = make_pipeline(StandardScaler(), SVR(kernel = 'linear', C = 1000, degree = 3, max_iter = 1_000_000))
        # clf = SVR(kernel = 'rbf', C = 100, degree = 3, max_iter = 100_000)
        # clf = make_pipeline(StandardScaler(), LinearRegression(n_jobs = 4))
        # clf.fit(X_train, y_train)
        # r_sq = m.score(X_, Y)
        # clf = tree.DecisionTreeRegressor().fit(X_train, y_train)
        clf = ensemble.RandomForestRegressor().fit(X_train, y_train)
        ype[te] = clf.predict(X_test)[0]

    print("r2: " + str(r2_score(Y, ype)))
    # Y = data[key].to_numpy()
    # ype = np.exp2(ype)

    print("(after exp2) r2: " + str(r2_score(Y, ype)))
    print("max error: " + str(metrics.max_error(Y, ype)))
    print("mean ae: " + str(metrics.mean_absolute_error(Y, ype)))
    print("mape: " + str(metrics.mean_absolute_percentage_error(Y, ype)))
    print("median ae: " + str(metrics.median_absolute_error(Y, ype)))

    # mpl.figure(fig)
    # fig += 1
    # mpl.grid()

    # show_trend_line(data[3], data[1])
    # mpl.axline((0, 0), slope = 1, linewidth = 0.5)
    # mpl.scatter(Y, ype, marker = "x", linewidth = 1)
    # mpl.title("actual vs pred")
    # mpl.xlabel("actual")
    # mpl.ylabel("pred")
    # mpl.savefig(title + '.png', bbox_inches = 'tight', dpi = 600)

    # print(Y)
    # print(ype)

    print("\n")

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
    # return metrics.f1_score(Y, ype)
    return ype

def run_DT(data, title, inputs, mt, mm):
    print("## " + title + "\n")
    res = {}

    # data = data[data.state != "mem"]
    nb = len(data)
    nbt = len(data[(data.time >= mt) | (data.mem >= mm)])

    print("nb elems: " + str(nb))
    print("nb to: " + str(nbt))
    print("cnst precision: " + str(max(nbt / nb, (nb - nbt) / nb))) 

    # if nbt == 0:
        # return -1

    X = data[inputs].to_numpy()
    scaler = StandardScaler()
    scaler.fit(X)
    X_ = scaler.transform(X)
    # X = data[['tw', '#eqv']].to_numpy()
    Y = (data['time'].ge(mt).to_numpy()) | (data['mem'].ge(mm).to_numpy())
    # Y = data[key].to_numpy()

    # res["cnst 0"] = metrics.f1_score(Y, np.zeros(len(Y)))
    # res["cnst 1"] = metrics.f1_score(Y, np.zeros(len(Y)) + 1)

    res["DT"] = c_loo(X, Y, ensemble.RandomForestClassifier(n_estimators = 100, n_jobs = 3), "DT")
    # clf = ensemble.RandomForestClassifier(n_estimators = 1000, n_jobs = 3).fit(X, Y)
    # print(clf.feature_importances_)
    # tree.export_graphviz(clf, out_file="r.dot", class_names=True, feature_names = ["#var", "#cls", "#eqv"])

    return pd.DataFrame({'real': Y, 'pred': res["DT"]}, index = data.index)

def show_VIF(data, keys):
    print("## VIF\n")
    X = data[keys].to_numpy()
    for i in range(0, len(keys)):
        print(keys[i] + ": " + str(variance_inflation_factor(X, i)))
    print("\n")

ipk1 = ['#mis']
ipk2 = ['#eqv']
ipk3 = ['deff']
ipk4 = ['#var']
ipk5 = ['#clause']
ipk6 = ['#var', '#clause', '#literal', 'tw', 'lp', '#mis', '#eqv', 'time_z3', 'mem_z3']
ipk7 = ['#var', '#clause', '#literal', 'tw', 'lp', '#mis', '#eqv']
ipk8 = ['#var', '#clause', 'tw', 'lp', '#mis', '#eqv']
ipk9 = ['#var', 'tw', 'lp', '#mis', '#eqv']
ipk10 = ['#var', 'tw', 'deff', '#mis', '#eqv']
ipk11 = ['#var', 'deff', '#mis', '#eqv']
ipk12 = ['#var', 'deff', '#mis']
ipk13 = ['#var', 'deff', '#eqv']
ipk14 = ['#var', 'deff']
ipk15 = ['#var', '#clause', 'deff']
ipk16 = ['#var', '#clause', '#literal', 'deff']
# ipk = ['#mis']
# show_VIF(data, ipk)
# ipk = ['#eqv']

spur_fm = spur.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
ug3_fm = ug3.filter(regex = "(FeatureModel|FMEasy)", axis = 0)

spur_nfm = spur.drop(ug3.index, axis = 0)
ug3_nfm = ug3.drop(ug3.index, axis = 0)

# samplers = {'\\spur': spur, '\\unigen': ug3, '\\spur (FM)': spur_fm, '\\unigen (FM)': ug3_fm, '\\spur (non-FM)': spur_nfm, '\\unigen (non-FM)': ug3_nfm}
samplers = {'\\spur': spur, '\\unigen': ug3}
ipk = {  '\\#mis': ipk1
       , '\\#eqv': ipk2
       , '\\edeff': ipk3
       , '\\#v': ipk4
       , '\\#c': ipk5
       , 'all': ipk6
       , '\\#v, \\#c, \\#l, \\tw, \\deff, \\#mis, \\#eqv': ipk7
       , '\\#v, \\#c, \\tw, \\deff, \\#mis, \\#eqv': ipk8
       , '\\#v, \\tw, \\deff, \\#mis, \\#eqv': ipk9
       , '\\#v, \\tw, \\edeff, \\#mis, \\#eqv': ipk10
       , '\\#v, \\edeff, \\#mis, \\#eqv': ipk11
       , '\\#v + \\edeff + \\#mis': ipk12
       , '\\#v + \\edeff + \\#eqv': ipk13
       , '\\#v + \\edeff': ipk14
       , '\\#v + \\#c + \\edeff': ipk15
       , '\\#v + \\#c + \\#l + \\edeff': ipk16
       }

mt = 1800
mm = 4*10**6

# name = list(sys.argv)[1]
r = {}
# sampler = pd.read_csv(name, skipinitialspace = True, index_col = 'file')
# r[name] = run_DT(data.join(sampler, rsuffix = '_spur', on = 'file'), "Sampler time analysis", ipk, mt, mm)

for k in ipk:
    l = {}
    for s in samplers:
        l[s] = run_DT(data.join(samplers[s], rsuffix = '_sam', on = 'file'), "Sampler time analysis", ipk[k], mt, mm) * 1
    r[k] = l

print(pd.DataFrame(r).T)
pd.DataFrame(r).T.to_pickle("rf_100_n.pkl")

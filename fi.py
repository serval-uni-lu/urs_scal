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

# data = data[data.tw.notnull()]
data.dropna(inplace = True)
data.to_csv('m.csv')


def fi(data, title, inputs, mt, mm):
    print("## " + title + "\n")
    res = {}

    # data = data[data.state != "mem"]
    nb = len(data)
    nbt = len(data[(data.time >= mt) | (data.mem >= mm)])

    print("nb elems: " + str(nb))
    print("nb to: " + str(nbt))
    print("cnst precision: " + str(max(nbt / nb, (nb - nbt) / nb))) 

    X = data[inputs].to_numpy()
    Y = (data['time'].ge(mt).to_numpy()) | (data['mem'].ge(mm).to_numpy())

    clf = ensemble.RandomForestClassifier(n_estimators = 1000, n_jobs = 3).fit(X, Y)
    return clf.feature_importances_

def show_VIF(data, keys):
    print("## VIF\n")
    X = data[keys].to_numpy()
    for i in range(0, len(keys)):
        print(keys[i] + ": " + str(variance_inflation_factor(X, i)))
    print("\n")

ipk = ['#var', '#clause', '#literal', 'tw', 'lp', '#mis', '#eqv', 'time_z3', 'mem_z3']
show_VIF(data, ipk)

mt = 1800
mm = 4*10**6

samplers = {'\\spur': spur, '\\unigen': ug3, 'D4': d4, 'sharpSAT': sharpSAT}


r = {}

for s in samplers:
    r[s] = fi(data.join(samplers[s], rsuffix = '_sam', on = 'file'), "Sampler time analysis " + s, ipk, mt, mm) * 100

samplers = {'\\spur fm': spur, '\\unigen fm': ug3, 'D4 fm': d4, 'sharpSAT fm': sharpSAT}
data = data.filter(regex = "(FeatureModel|FMEasy)", axis = 0)

for s in samplers:
    r[s] = fi(data.join(samplers[s], rsuffix = '_sam', on = 'file'), "Sampler time analysis " + s, ipk, mt, mm) * 100

print(pd.DataFrame(r, index = ipk).T)
pd.DataFrame(r, index = ipk).T.to_csv("fi.csv")

#!/usr/bin/env python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as mpl

import scipy.stats as stats

deff = pd.read_csv("data/deff.csv", skipinitialspace = True, index_col = 'file')
fpt = pd.read_csv("data/fpt.csv", skipinitialspace = True, index_col = 'file')
patoh = pd.read_csv("data/patoh_9.csv", skipinitialspace = True, index_col = 'file')
# d4 = pd.read_csv("data/d4_001_450.csv", skipinitialspace = True, index_col = 'file')
# d4 = pd.read_csv("d4_mod_001_450.csv", skipinitialspace = True, index_col = 'file')
eqv = pd.read_csv("data/eqv.csv", skipinitialspace = True, index_col = 'file')
z3 = pd.read_csv("data/z3.csv", skipinitialspace = True, index_col = 'file')
ms = pd.read_csv("data/minisat.csv", skipinitialspace = True, index_col = 'file')
nb_cls = pd.read_csv("data/nb_clauses.csv", skipinitialspace = True, index_col = 'file')
# nb_cls = pd.read_csv("data/3sat.eqv.csv", skipinitialspace = True, index_col = 'file')
# sat3 = pd.read_csv("data/3sat.csv", skipinitialspace = True, index_col = 'file')
mis = pd.read_csv("data/mis.csv", skipinitialspace = True, index_col = 'file')
tw = pd.read_csv("data/tw.csv", skipinitialspace = True, index_col = 'file')
# mc = pd.read_csv("data/mc_log.csv", skipinitialspace = True, index_col = 'file')

# kus = pd.read_csv("data/log_kus_2022-07-06.csv", skipinitialspace = True, index_col = 'file')
unigen3 = pd.read_csv("data/stat_unigen3_2022-08-16.csv", skipinitialspace = True, index_col = 'file')
spur = pd.read_csv("data/stat_spur_2022-08-11.csv", skipinitialspace = True, index_col = 'file')
quicksampler = pd.read_csv("data/stat_quicksampler_2022-07-25.csv", skipinitialspace = True, index_col = 'file')
d4 = pd.read_csv("data/stat_d4_2022-08-11.csv", skipinitialspace = True, index_col = 'file')
sharpSAT = pd.read_csv("data/stat_sharpSAT_2022-08-11.csv", skipinitialspace = True, index_col = 'file')
cms = pd.read_csv("data/stat_cms_2022-08-11.csv", skipinitialspace = True, index_col = 'file')
bsat = pd.read_csv("data/stat_bsat_2022-08-16.csv", skipinitialspace = True, index_col = 'file')

# z3 = bsat.rename(columns={'state' : 'state_z3', 'mem': 'mem_z3', 'time': 'time_z3'}, inplace = False)


# kus = kus[kus['state'] == 'done']
# spur = spur[spur['state'] == 'done']
# unigen3 = unigen3[unigen3['state'] == 'done']
# quicksampler = quicksampler[quicksampler['state'] == 'done']

# data = eqv.join(d4, rsuffix = '_d4', on = 'file')
# data = eqv.join(kus, rsuffix = '_kus', on = 'file')
# data = eqv.join(spur, rsuffix = '_spur', on = 'file')
# data = eqv.join(unigen3, rsuffix = '_unigen3', on = 'file')
# data = eqv.join(quicksampler, rsuffix = '_quicksampler', on = 'file')
data = eqv
data = data.join(mis, rsuffix = '_mis', on = 'file')
data = data.join(tw, rsuffix = '_tw', on = 'file')
data = data.join(deff, rsuffix = '_deff', on = 'file')
data = data.join(fpt, rsuffix = '_fpt', on = 'file')
# data = data.join(patoh, rsuffix = '_patoh', on = 'file')
data = data.join(nb_cls, rsuffix = '_nb_cls', on = 'file')
# data = data.join(sat3, rsuffix = '_3sat', on = 'file')
data = data.join(z3, rsuffix = '_z3', on = 'file')
# data = data.join(mc, rsuffix = '_z3', on = 'file')
# data = data.join(ms, rsuffix = '_ms', on = 'file')
# data = data.join(tw, rsuffix = '_tw', on = 'file')

# data = deff.join(fpt, rsuffix = '_fpt', on = 'file').join(patoh, rsuffix = '_patoh', on = 'file')
# data = data.join(d4 , rsuffix = '_d4' , on = 'file').join(eqv  , rsuffix = '_eqv'  , on = 'file')
# data = data.join(z3 , rsuffix = '_z3' , on = 'file').join(nb_cls, rsuffix = '_cls', on = 'file')
# data = data.join(mis, rsuffix = '_mis', on = 'file').join(tw, rsuffix = '_tw', on = 'file')

# data = data[data['state_kus'] == 'done']
# data = data[data['state_spur'] == 'done']
# data = data[data['state_unigen3'] == 'done']
# data = data[data['state_quicksampler'] == 'done']


fm = data.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
# data = data.drop(fm.index, axis = 0)
# data = fm

data = data[data.tw.notnull()]
data.dropna(inplace = True)
data.to_csv('m.csv')

def print_corr(X, Y, nx, ny):
    print("correlation (" + nx + ", " + ny + ")")
    corr, p = stats.pearsonr(X, Y)
    print("    pearsonr: " + str(corr) + ", " + str(p))
    corr, p = stats.kendalltau(X, Y)
    print("    kendalltau: " + str(corr) + ", " + str(p))
    corr, p = stats.spearmanr(X, Y)
    print("    spearmanr: " + str(corr) + ", " + str(p))

def get_max_len(l):
    m = -1
    for i in l:
        m = max(m, len(i))
    return m

def normal_fmt(m_len, n):
    num_fmt = "{:" + str(m_len - 2) + ".3f}  "
    return num_fmt.format(n)

def em_fmt(m_len, n):
    num_fmt = "*{:5.3f}  "
    r = num_fmt.format(n)
    return ((m_len - len(r)) * " ") + r

def em2_fmt(m_len, n):
    num_fmt = "**{:5.3f} "
    r = num_fmt.format(n)
    return ((m_len - len(r)) * " ") + r


def correlation_matrix(data, names, highligh, f):
    nb_cols = len(data) + 1
    m_len = max(11, get_max_len(names) + 2)
    res = "" + (m_len * " ") + "&"
    for i in range(0, len(names) - 2):
        n = names[i]
        res += " " + n + ((m_len - len(n) - 1) * " ")
        if i != len(names) - 3:
            res += "&"
    res += "\\\\\n"

    for j in range(len(data) - 2, len(data)):
        res += "" + names[j] + ((m_len - len(names[j]) - 1) * " ") + "&"
        for i in range(0, len(data) - 2):
            corr, pv = f(data[j], data[i])
            corr *= 100
            if i < j:
                # if (names[i] in highligh and names[j] in highligh[names[i]]) or pv >= (10**(-3)):
                if (names[i] in highligh and names[j] in highligh[names[i]]) or pv >= 0.05:
                    res += em2_fmt(m_len, corr)
                elif (names[i] in highligh and names[j] in highligh[names[i]]) or pv >= 0.001:
                    res += em_fmt(m_len, corr)
                else:
                    res += normal_fmt(m_len, corr)

            elif i == j:
                res += "           " + ((m_len - 11) * " ")
            else:
                # res += normal_fmt(m_len, pv) + "|"
                res += "           " + ((m_len - 11) * " ")
            if i != len(data) - 3:
                res += "&"
        res += "\\\\\n"


    return res

highligh = {
    '#vars': {'#clauses', '#sclauses', '#eqv', '#nodes', 'd4 time', 'deff', 'patoh'}
    , '#clauses': {'#nodes', 'd4 time', 'deff'}
    , '#sclauses': {'#nodes', 'd4 time'}
    , '#eqv': {'#nodes', 'd4 time', 'log2(#nod)', 'log2(#edg)'}
    , 'nodes': {'#edges', 'd4 time', 'deff', 'sdeff', 'patoh'}
    , 'd4 time': {'deff', 'sdeff', 'patoh'}
}


def gen_tables(data, title):
    # data = data[data.state.notnull()]
    orig = data

    print("## " + title + "\n")

    data = orig
    data = data[data['state'] == "done"]
    d_list = [data['#var'], data['#clause'], data['#literal'], data.tw, data['lp'], data['#mis'], data['#eqv'], data['time_z3'], data['mem_z3'], data['time'], data['mem']]
    n_list = ['#v', '#c', '#l', 'tw', 'deff', '#mis', '#eqv', 'time_z3', 'mem_z3', 'time', 'mem']

    r = correlation_matrix(d_list, n_list, highligh, stats.kendalltau)
    print("### Kendall Correlation (all)\n")
    print(str(len(data)) + " elems\n")
    print(r)

    data = orig
    data = data[data['state'] == "done"]
    data = data.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
    d_list = [data['#var'], data['#clause'], data['#literal'], data.tw, data['lp'], data['#mis'], data['#eqv'], data['time_z3'], data['mem_z3'], data['time'], data['mem']]
    n_list = ['#v', '#c', '#l', 'tw', 'deff', '#mis', '#eqv', 'time_z3', 'mem_z3', 'time', 'mem']

    r = correlation_matrix(d_list, n_list, highligh, stats.kendalltau)
    print("### Kendall Correlation (FM)\n")
    print(str(len(data)) + " elems\n")
    print(r)

    data = orig
    data = data[data['state'] == "done"]
    fm = data.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
    data = data.drop(fm.index, axis = 0)

    d_list = [data['#var'], data['#clause'], data['#literal'], data.tw, data['lp'], data['#mis'], data['#eqv'], data['time_z3'], data['mem_z3'], data['time'], data['mem']]
    n_list = ['#v', '#c', '#l', 'tw', 'deff', '#mis', '#eqv', 'time_z3', 'mem_z3', 'time', 'mem']

    r = correlation_matrix(d_list, n_list, highligh, stats.kendalltau)
    print("### Kendall Correlation (no FM)\n")
    print(str(len(data)) + " elems\n")
    print(r)

    data = orig
    d_list = [data['#var'], data['#clause'], data['#literal'], data.tw, data['lp'], data['#mis'], data['#eqv'], data['time_z3'], data['mem_z3'], data['state'] != "done"]
    n_list = ['#v', '#c', '#l', 'tw', 'deff', '#mis', '#eqv', 'time_z3', 'mem_z3', 'done']

    r = correlation_matrix(d_list, n_list, highligh, stats.pointbiserialr)
    print("### Point biserial r (done?)\n")
    print(str(len(data)) + " elems\n")
    print(r)





print("""+++
title = "Correlation of formula metrics with sampling metrics"
date = "2022-07-20"
description = "Correlation of formula metrics with sampling metrics"
katex = false
autonumbering = true
+++\n\n""")

# data = eqv.join(d4, rsuffix = '_d4', on = 'file')
# data = eqv.join(kus, rsuffix = '_kus', on = 'file')
# data = eqv.join(spur, rsuffix = '_spur', on = 'file')
# data = eqv.join(unigen3, rsuffix = '_unigen3', on = 'file')
# data = eqv.join(quicksampler, rsuffix = '_quicksampler', on = 'file')

# gen_tables(data.join(kus, rsuffix = '_kus', on = 'file'), "KUS time analysis")
gen_tables(data.join(spur, rsuffix = '_spur', on = 'file'), "SPUR time analysis")
gen_tables(data.join(unigen3, rsuffix = '_unigen3', on = 'file'), "UniGen3 time analysis")
# gen_tables(data.join(cms, rsuffix = '_cms', on = 'file'), "CMSgen time analysis")
# gen_tables(data.join(quicksampler, rsuffix = '_quicksampler', on = 'file'), "Quicksampler time analysis")
gen_tables(data.join(bsat, rsuffix = '_bsat', on = 'file'), "BSAT time analysis")

gen_tables(data.join(d4, rsuffix = '_d4', on = 'file'), "D4 time analysis")
gen_tables(data.join(sharpSAT, rsuffix = '_sharpSAT', on = 'file'), "sharpSAT time analysis")

z3 = z3.rename(columns={'state_z3' : 'state', 'mem_z3': 'mem', 'time_z3': 'time'}, inplace = False)
ms = ms.rename(columns={'state_ms' : 'state', 'mem_ms': 'mem', 'time_ms': 'time'}, inplace = False)
gen_tables(data.join(z3, rsuffix = '_z3', on = 'file'), "Z3")
gen_tables(data.join(ms, rsuffix = '_ms', on = 'file'), "MiniSAT")

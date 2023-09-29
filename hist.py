#!/usr/bin/env python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as mpl

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

# data = data[data.tw.notnull()]
data.dropna(inplace = True)
data.to_csv('m.csv')

spur = data.join(spur, on = 'file')
ug3 = data.join(ug3, on = 'file')
d4 = data.join(d4, on = 'file')
sharpSAT = data.join(sharpSAT, on = 'file')

spur = spur[spur.state == 'done']
ug3 = ug3[ug3.state == 'done']
d4 = d4[d4.state == 'done']
sharpSAT = sharpSAT[sharpSAT.state == 'done']

# fm = data.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
# spur = spur.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
# ug3 = ug3.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
fm = data


print(data['#eqv'].min())
print(data['#eqv'].max())
# ax = data['#eqv'].hist(bins = 100)
# fig = ax.get_figure()
# fig.savefig('hist.png')

mpl.figure(0)
mpl.grid()

nb_bins = 50

l = [spur['#mis'], ug3['#mis'], fm['#mis']]
lab = ['spur', 'ug3', 'fm']

mpl.hist(l, bins = nb_bins, label = lab)


# mpl.hist(fm['#eqv'], bins = nb_bins, label = 'fm')

mpl.legend()
mpl.savefig("hist.png", dpi = 600, bbox_inches = 'tight')


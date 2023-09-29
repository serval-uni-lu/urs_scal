#!/usr/bin/env python3
import pandas as pd
import numpy as np

spur = pd.read_csv("data/stat_spur_2022-08-11.csv", skipinitialspace = True, index_col = 'file')
ug3 = pd.read_csv("data/stat_unigen3_2022-08-16.csv", skipinitialspace = True, index_col = 'file')
d4 = pd.read_csv("data/stat_d4_2022-08-11.csv", skipinitialspace = True, index_col = 'file')
sharpSAT = pd.read_csv("data/stat_sharpSAT_2022-08-11.csv", skipinitialspace = True, index_col = 'file')

def pp(sampler, title, mt = 1800, mm = 4 * 10**6):
    print("--------------------------------------------------")
    print(title)
    fm = sampler.filter(regex = "(FeatureModel|FMEasy)", axis = 0)

    print("total nb: " + str(len(sampler)))
    print("FM nb: " + str(len(fm)))

    lim = sampler[sampler.time <= mt]
    lim = lim[lim.mem <= mm]

    print("(all) success: " + str(len(lim)))

    lim = fm[fm.time <= mt]
    lim = lim[lim.mem <= mm]
    print("(FM) success: " + str(len(lim)))

pp(spur, "SPUR")
pp(ug3, "UG3")
pp(d4, "D4")
pp(sharpSAT, "sharpSAT")

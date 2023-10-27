#!/usr/bin/env python3
import pandas as pd
from sklearn import metrics


df = pd.read_pickle("rf_100.pkl")

spur = df['\\spur']
ug3 = df['\\unigen']

res = {'f1': {}, 'roc': {}}

nb_fm = 1000000
nb = 100000
nb_nfm = 100000

for i in df.index:
    sp = spur[i]
    ug = ug3[i]

    nb = min(len(sp), min(nb, len(ug)))

    sp_fm = sp.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
    ug_fm = ug.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
    nb_fm = min(len(sp_fm), min(nb_fm, len(ug_fm)))

    sp_nfm = sp.drop(sp_fm.index, axis = 0)
    ug_nfm = ug.drop(ug_fm.index, axis = 0)
    nb_nfm = min(len(sp_nfm), min(nb_nfm, len(ug_nfm)))

    l = {}
    l['\\spur (all)'] = metrics.recall_score(sp['real'], sp['pred'])
    l['\\spur (FM)'] = metrics.recall_score(sp_fm['real'], sp_fm['pred'])
    l['\\spur (non-FM)'] = metrics.recall_score(sp_nfm['real'], sp_nfm['pred'])

    l['\\unigen (all)'] = metrics.recall_score(ug['real'], ug['pred'])
    l['\\unigen (FM)'] = metrics.recall_score(ug_fm['real'], ug_fm['pred'])
    l['\\unigen (non-FM)'] = metrics.recall_score(ug_nfm['real'], ug_nfm['pred'])
    res['f1'][i] = l

    l = {}
    l['\\spur (all)'] = metrics.precision_score(sp['real'], sp['pred'])
    l['\\spur (FM)'] = metrics.precision_score(sp_fm['real'], sp_fm['pred'])
    l['\\spur (non-FM)'] = metrics.precision_score(sp_nfm['real'], sp_nfm['pred'])

    l['\\unigen (all)'] = metrics.precision_score(ug['real'], ug['pred'])
    l['\\unigen (FM)'] = metrics.precision_score(ug_fm['real'], ug_fm['pred'])
    l['\\unigen (non-FM)'] = metrics.precision_score(ug_nfm['real'], ug_nfm['pred'])
    res['roc'][i] = l


print("recall")
print(pd.DataFrame(res['f1']).T * 100)
print("\nprecision")
print(pd.DataFrame(res['roc']).T * 100)

print(f"nb:{nb}, nb_fm: {nb_fm}, nb_nfm: {nb_nfm}")

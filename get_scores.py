#!/usr/bin/env python3
import pandas as pd
from sklearn import metrics


df = pd.read_pickle("rf_100.pkl")

spur = df['\\spur']
ug3 = df['\\unigen']

res = {'f1': {}, 'roc': {}}

for i in df.index:
    sp = spur[i]
    ug = ug3[i]

    sp_fm = sp.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
    ug_fm = ug.filter(regex = "(FeatureModel|FMEasy)", axis = 0)

    sp_nfm = sp.drop(sp_fm.index, axis = 0)
    ug_nfm = ug.drop(ug_fm.index, axis = 0)

    l = {}
    l['\\spur (all)'] = metrics.f1_score(sp['real'], sp['pred'])
    l['\\spur (FM)'] = metrics.f1_score(sp_fm['real'], sp_fm['pred'])
    l['\\spur (non-FM)'] = metrics.f1_score(sp_nfm['real'], sp_nfm['pred'])

    l['\\unigen (all)'] = metrics.f1_score(ug['real'], ug['pred'])
    l['\\unigen (FM)'] = metrics.f1_score(ug_fm['real'], ug_fm['pred'])
    l['\\unigen (non-FM)'] = metrics.f1_score(ug_nfm['real'], ug_nfm['pred'])
    res['f1'][i] = l

    l = {}
    l['\\spur (all)'] = metrics.roc_auc_score(sp['real'], sp['pred'])
    l['\\spur (FM)'] = metrics.roc_auc_score(sp_fm['real'], sp_fm['pred'])
    l['\\spur (non-FM)'] = metrics.roc_auc_score(sp_nfm['real'], sp_nfm['pred'])

    l['\\unigen (all)'] = metrics.roc_auc_score(ug['real'], ug['pred'])
    l['\\unigen (FM)'] = metrics.roc_auc_score(ug_fm['real'], ug_fm['pred'])
    l['\\unigen (non-FM)'] = metrics.roc_auc_score(ug_nfm['real'], ug_nfm['pred'])
    res['roc'][i] = l


print("F1:")
print(pd.DataFrame(res['f1']).T * 100)
print("\nROC AUCs:")
print(pd.DataFrame(res['roc']).T * 100)

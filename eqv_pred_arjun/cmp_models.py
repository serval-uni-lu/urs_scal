#!/usr/bin/env python3
import pandas as pd
from sklearn import metrics


df = pd.read_pickle("models.pkl")

sp = df['\\spur'][0]
ug = df['\\unigen'][0]

res = {'f1': {}, 'roc': {}}

sp_fm = sp.filter(regex = "(FeatureModel|FMEasy)", axis = 0)
ug_fm = ug.filter(regex = "(FeatureModel|FMEasy)", axis = 0)

sp_nfm = sp.drop(sp_fm.index, axis = 0)
ug_nfm = ug.drop(ug_fm.index, axis = 0)

for i in ["DT", "RF 10", "RF 100", "RF 1000"]:
    if i not in sp:
        continue
    l = {}
    l['\\spur (all)'] = metrics.f1_score(sp['real'], sp[i])
    l['\\spur (FM)'] = metrics.f1_score(sp_fm['real'], sp_fm[i])
    l['\\spur (non-FM)'] = metrics.f1_score(sp_nfm['real'], sp_nfm[i])

    l['\\unigen (all)'] = metrics.f1_score(ug['real'], ug[i])
    l['\\unigen (FM)'] = metrics.f1_score(ug_fm['real'], ug_fm[i])
    l['\\unigen (non-FM)'] = metrics.f1_score(ug_nfm['real'], ug_nfm[i])
    res['f1'][i] = l

    l = {}
    l['\\spur (all)'] = metrics.roc_auc_score(sp['real'], sp[i])
    l['\\spur (FM)'] = metrics.roc_auc_score(sp_fm['real'], sp_fm[i])
    l['\\spur (non-FM)'] = metrics.roc_auc_score(sp_nfm['real'], sp_nfm[i])

    l['\\unigen (all)'] = metrics.roc_auc_score(ug['real'], ug[i])
    l['\\unigen (FM)'] = metrics.roc_auc_score(ug_fm['real'], ug_fm[i])
    l['\\unigen (non-FM)'] = metrics.roc_auc_score(ug_nfm['real'], ug_nfm[i])
    res['roc'][i] = l

print("F1:")
print((pd.DataFrame(res['f1']).T * 100).round(3).to_string())
print("\nROC AUCs:")
print((pd.DataFrame(res['roc']).T * 100).round(3).to_string())

# print(f"nb:{nb}, nb_fm: {nb_fm}, nb_nfm: {nb_nfm}")

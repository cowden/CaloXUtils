
import os
import re
import numpy as np
import pandas as pd


nBins = 8
dE = 500/nBins
bins = np.arange(0, 500+dE, dE)

#
# estiamte resolution
def resolution(X, cuts=[3,97]):
    cut_points = np.percentile(X,cuts)
    return np.std(X[(X > cut_points[0])&(X <= cut_points[1])])

#
# aggregate resolution
def aggregate_resolution(df,bins):
    # convert beam energy
    df['eGen'] /= 1000

    # apply zero suppresion
    df.loc[df['uncalib_pred']<0,'uncalib_pred'] = 0.

    # estimate the bin centers
    centers = np.diff(bins)/2. + bins[:-1]

    # digitize the bins
    df['bin'] = np.digitize(df['eGen'], bins) - 1
    df['calib_response'] = df['calib_pred']/df['eGen']
    df['uncalib_response'] = df['uncalib_pred']/df['eGen']

    # group by bin
    gdf = df.loc[df['eGen']>0,['bin','calib_response','uncalib_response','calib_pred','uncalib_pred','calib_esum','uncalib_esum']].groupby('bin').agg(['mean',resolution])
    gdf['E'] = centers
    gdf['invE'] = 1./centers
    gdf['invsqrtE'] = np.sqrt(1./centers)

    # normalize resolution by mean energy
    gdf['calib_res'] = gdf[('calib_response','resolution')]/gdf[('calib_response','mean')]
    gdf['uncalib_res'] = gdf[('uncalib_response','resolution')]/gdf[('uncalib_response','mean')]

    return gdf

#
# get scan parameter from file name
def get_scan_param_from_file(name):
    if name[-5:] != '.pckl':
        return 0, 0

    return ([float(par) for par in re.split('_',name[:-5])[3:]])

#
# collect parameters from scan directory
def collect_parameters_from_dir(directory):
    parameter_list = []
    for data in os.listdir(directory):
        params = get_scan_param_from_file(data)

        parameter_list.append(tuple(params))

    return parameter_list

#
# apply a callback method to scan directory
def apply_callback_scan(directory,callback):
    returns = []
    for data in os.listdir(directory):
        ret = callback(os.path.join(directory,data))
        returns.append(ret)

    return returns

#
# get file name from parameters
def get_file_from_params(param, directory='/data/me/calox/pi-scan3', base_name='calox_pi-_500GeV'):
  file_name = os.path.join(directory, base_name + '_{:.2f}_{:.1f}_{:.2f}.pckl'.format(*param))
  return file_name

#
# apply a callback to a lit of parameters
def apply_callback_params(params, callback,directory='/data/me/calox/pi-scan3',base_name='calox_pi-_500GeV'):
    returns = []
    for par in params:
        file_name = get_file_from_params(par, directory, base_name)
        ret = callback(file_name)
        returns.append(ret)

    return returns

#
# plot response

#
# plot resolution

#
# plot residual

 


import os
import re
import numpy as np
import pandas as pd
import statsmodels.api as sm
import lmfit
import matplotlib as mpl
import pylab as plt


nBins = 8
dE = 125/nBins
bins = np.arange(0, 125+dE, dE)


#
# model response
def mod_response(df, col, egen='eGen'):
    mod = sm.GLM(df[col], sm.add_constant(df[egen])).fit()
    return mod

#
# get residual
def get_residuals(df, col, mod, egen='eGen'):
    return ((df[col]-mod.params['const'])/mod.params[egen] - df[egen])/df[egen]

#
# response
def get_response(df):
    dfc = df.loc[df['eGen'] < 125000].copy()

    # convert beam energy
    dfc['eGen'] /= 1000
    dfc['calib_esum'] /= 1000
    dfc['uncalib_esum'] /= 1000

    # apply zero suppresion
    dfc.loc[dfc['uncalib_pred']<0,'uncalib_pred'] = 0.
    dfc = dfc.loc[dfc['eGen'] < 125]
    uncalib_mod = mod_response(dfc, 'uncalib_pred')
    dfc['uncalib_resid'] = get_residuals(dfc, 'uncalib_pred', uncalib_mod)
    calib_mod = mod_response(dfc, 'calib_pred')
    dfc['calib_resid'] = get_residuals(dfc, 'calib_pred', calib_mod)

    calib_esum_mod = mod_response(dfc, 'calib_esum')
    dfc['calib_esum_resid'] = get_residuals(dfc, 'calib_esum', calib_esum_mod)
    uncalib_esum_mod = mod_response(dfc, 'uncalib_esum')
    dfc['uncalib_esum_resid'] = get_residuals(dfc, 'uncalib_esum', uncalib_esum_mod)

    return dfc


#
# estiamte resolution
def resolution(X, cuts=[3,97]):
    cut_points = np.percentile(X,cuts)
    return np.std(X[(X > cut_points[0])&(X <= cut_points[1])])


#
# aggregate resolution
def aggregate_resolution(df,bins):

    dfc = get_response(df)

    # estimate the bin centers
    centers = np.diff(bins)/2. + bins[:-1]

    # digitize the bins
    dfc['bin'] = np.digitize(dfc['eGen'], bins) - 1

    # group by bin
    gdf = dfc.groupby('bin')

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
# resolution parameterization
def refit_func(x, const, stoch, noise):
    return np.sqrt(const**2 + stoch**2*x**2 + noise**2*x**4)


#
# fit data and return fitted model
def fit_resolution(x, y):

    mod = lmfit.Model(refit_func)

    fpars = mod.make_params(const=0.05, stoch=0.25, noise=10.)
    fpars['const'].min = 0
    fpars['stoch'].min = 0
    fpars['noise'].min = 0

    res = mod.fit(y, fpars, x=x)

    return res

#
# resolution callback
def resolution_callback(fn):
    df = pd.read_pickle(fn)

    gdf = aggregate_resolution(df, bins)

    mod = lmfit.Model(refit_func)

    resdf = pd.DataFrame({'uncalib_resolution': gdf['uncalib_resid'].apply(lambda x: resolution(x)),
        'calib_resolution': gdf['calib_resid'].apply(lambda x: resolution(x)),
        'eGen': gdf['eGen'].mean(),
        'x': 1./np.sqrt(gdf['eGen'].mean()) }
        )

    res = fit_resolution(resdf['x'], resdf['uncalib_resolution'])

    return (res, resdf)


#
# do the resolution analysis
class ResolutionAnalysis:
    
    def __init__(self, data='/data/me/calox/pi-scan3'):
        self._data = data

        self._params = collect_parameters_from_dir(data)
        self._resfits = apply_callback_params(self._params, resolution_callback)

        self._aic = [mod[0].aic for mod in self._resfits]
        self._fit_pars = [(mod[0].params['const'].value, mod[0].params['stoch'].value, mod[0].params['noise'].value) for mod in self._resfits]
        self._reses = [r[1] for r in self._resfits]

        self._df = pd.DataFrame( {
            'calib': [p[0] for p in self._params],
            'noise': [p[1] for p in self._params],
            'bias':  [p[2] for p in self._params],
            'aic':   self._aic,
            'const': [fp[0] for fp in self._fit_pars],
            'stoch': [fp[1] for fp in self._fit_pars],
            'inoise': [fp[2] for fp in self._fit_pars]
        })

        self._ideal_mod = fit_resolution(self._reses[0]['x'], self._reses[0]['calib_resolution'])


    @property
    def result(self):
        return self._df


    def plotResolutions(self):

        fig = plt.figure(figsize=(11,8))

        colors = ['m', 'b', 'r' ,'darkorange', 'c']
        styles = ['-' ,'--', ':']
        shapes = ['o', 's' ,'X' ,'^' ,'v' ,'p' ,'d']

        calibs = np.unique(self.result['calib'])
        noises = np.unique(self.result['noise'])
        biases = np.unique(self.result['bias'])

        color_dict = dict(zip(calibs, colors))
        style_dict = dict(zip(biases, styles))
        shape_dict = dict(zip(noises, shapes))

        for i, p in enumerate(self._params):
           vals = self._reses[i][['x', 'uncalib_resolution']].values
           plt.plot(vals[:,0], vals[:,1],
               c=color_dict[p[0]],
               marker=shape_dict[p[1]],
               ls=style_dict[p[2]])


        vals = self._reses[0][['x','calib_resolution']].values
        plt.plot(vals[:,0], vals[:,1],
            lw=3, marker='3', c='k', label='Ideal Response')

        plt.xlabel(r'$\frac{1}{\sqrt{E_{Beam}\;[{\rm GeV}]}}$')
        plt.ylabel(r'$\frac{\sigma_{\hat{E}}}{\mu_{\hat{E}}}$')
        plt.title('CNN Resolution')

        leg1 = plt.legend(loc="upper left", bbox_to_anchor=(1., 1.))
        
        calib_handles = [mpl.lines.Line2D([], [], color=v) for k,v in color_dict.items()]
        calib_leg = plt.legend(handles=calib_handles,
            labels=color_dict.keys(),
            title='Calibration',
            bbox_to_anchor=(1., 0.93))
        plt.gca().add_artist(calib_leg)
        plt.gca().add_artist(leg1)

        noise_handles = [mpl.lines.Line2D([], [], marker=v, lw=0, c='#3A3A30') for k,v in shape_dict.items()]
        noise_leg = plt.legend(handles=noise_handles,
            labels=shape_dict.keys(),
            title='Noise',
            bbox_to_anchor=(1., 0.68))
        plt.gca().add_artist(noise_leg)

        bias_handles = [mpl.lines.Line2D([], [], ls=v, c='#3A3A3A') for k,v in style_dict.items()]
        bias_leg = plt.legend(handles=bias_handles,
            labels=style_dict.keys(),
            title='Bias',
            bbox_to_anchor=(1., 0.29))
        plt.gca().add_artist(bias_leg)


        plt.tight_layout(rect=[0., 0., 0.8, 1.])


    # plot the fit parameters
    def plotFitParams(self):
        fig = plt.figure(figsize=(16,8))
        gs = plt.GridSpec(3, 3, wspace=0, hspace=0)

        # plot constant vs calibration
        ax0 = plt.subplot(gs[0])
        gdf = self._df.groupby('calib').agg(['mean','std'])
        gdf.reset_index(inplace=True)
        ax0.errorbar(gdf['calib'], gdf[('const','mean')], yerr=gdf[('const','std')], lw=0., marker='o', elinewidth=1)
        ax0.set_ylabel('Constant Term')
        plt.setp(ax0.get_xticklabels(), visible=False)

        # plot constant vs noise
        ax1 = plt.subplot(gs[1], sharey=ax0)
        gdf = self._df.groupby('noise').agg(['mean','std'])
        gdf.reset_index(inplace=True)
        ax1.errorbar(gdf['noise'], gdf[('const','mean')], yerr=gdf[('const','std')], lw=0., marker='o', elinewidth=1)
        plt.setp(ax1.get_yticklabels(), visible=False)
        plt.setp(ax1.get_xticklabels(), visible=False)

        # plot constant vs bias
        ax2 = plt.subplot(gs[2], sharey=ax0)
        gdf = self._df.groupby('bias').agg(['mean','std'])
        gdf.reset_index(inplace=True)
        ax2.errorbar(gdf['bias'], gdf[('const','mean')], yerr=gdf[('const','std')], lw=0., marker='o', elinewidth=1)
        plt.setp(ax2.get_yticklabels(), visible=False)
        plt.setp(ax2.get_xticklabels(), visible=False)

        # plot stoch vs calib
        ax3 = plt.subplot(gs[3], sharex=ax0)
        gdf = self._df.groupby('calib').agg(['mean','std'])
        gdf.reset_index(inplace=True)
        ax3.errorbar(gdf['calib'], gdf[('stoch','mean')], yerr=gdf[('stoch','std')], lw=0., marker='o', elinewidth=1)
        ax3.set_ylabel('Stochastic Term')
        plt.setp(ax3.get_xticklabels(), visible=False)

        # plot stoch vs noise
        ax4 = plt.subplot(gs[4], sharex=ax1, sharey=ax3)
        gdf = self._df.groupby('noise').agg(['mean','std'])
        gdf.reset_index(inplace=True)
        ax4.errorbar(gdf['noise'], gdf[('stoch','mean')], yerr=gdf[('stoch','std')], lw=0., marker='o', elinewidth=1)
        plt.setp(ax4.get_xticklabels(), visible=False)
        plt.setp(ax4.get_yticklabels(), visible=False)

        # plot stoch vs bias
        ax5 = plt.subplot(gs[5], sharex=ax2, sharey=ax3)
        gdf = self._df.groupby('bias').agg(['mean','std'])
        gdf.reset_index(inplace=True)
        ax5.errorbar(gdf['bias'], gdf[('stoch','mean')], yerr=gdf[('stoch','std')], lw=0., marker='o', elinewidth=1)
        plt.setp(ax5.get_xticklabels(), visible=False)
        plt.setp(ax5.get_yticklabels(), visible=False)

        # plot noise vs calib
        ax6 = plt.subplot(gs[6], sharex=ax0)
        gdf = self._df.groupby('calib').agg(['mean','std'])
        gdf.reset_index(inplace=True)
        ax6.errorbar(gdf['calib'], gdf[('inoise','mean')], yerr=gdf[('inoise','std')], lw=0., marker='o', elinewidth=1)
        ax6.set_xlabel('Calibration Scale')
        ax6.set_ylabel('Noise Term')

        # plot noise vs noise
        ax7 = plt.subplot(gs[7], sharex=ax1, sharey=ax6)
        gdf = self._df.groupby('noise').agg(['mean','std'])
        gdf.reset_index(inplace=True)
        ax7.errorbar(gdf['noise'], gdf[('inoise','mean')], yerr=gdf[('inoise','std')], lw=0., marker='o', elinewidth=1)
        plt.setp(ax7.get_yticklabels(), visible=False)
        ax7.set_xlabel('Noise Scale [MeV]')

        # plot noise vs bias
        ax8 = plt.subplot(gs[8], sharex=ax2, sharey=ax6)
        gdf = self._df.groupby('bias').agg(['mean','std'])
        gdf.reset_index(inplace=True)
        ax8.errorbar(gdf['bias'], gdf[('inoise','mean')], yerr=gdf[('inoise','std')], lw=0., marker='o', elinewidth=1)
        ax8.set_xlabel('Bias Scale [MeV]')
        plt.setp(ax8.get_yticklabels(), visible=False)


    def plotIndResolution(self, indx):
        ax = plt.gca()

        vals = self._reses[indx]
        ax.plot(vals['x'], vals['uncalib_resolution'], marker='s')
        
        x = np.linspace(0.05, 0.35, 100)
        fpars = self._resfits[indx][0].params
        ax.plot(x, refit_func(x, fpars['const'].value, fpars['stoch'].value, fpars['noise'].value))

    def plotIdealResolution(self):
        ax = plt.gca()

        vals = self._reses[0]
        ax.plot(vals['x'], vals['calib_resolution'], marker='o')

        x = np.linspace(0.05, 0.35, 100)
        fpars = self._ideal_mod.params
        ax.plot(x, refit_func(x, fpars['const'].value, fpars['stoch'].value, fpars['noise'].value))

#
# plot response

#
# plot resolution

#
# plot residual

 

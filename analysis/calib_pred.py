#!/usr/bin/env python
# coding: utf-8


import sys
import pandas as pd
import numpy as np
import tensorflow as tf
import tables
import scipy.stats as stats
import pdb
import getopt

#####################################################
# Configure some stuff for Tensorflow
config = tf.compat.v1.ConfigProto()
config.gpu_options.allow_growth = True
tf.compat.v1.keras.backend.set_session(tf.compat.v1.Session(config=config))

gpus = tf.config.experimental.list_physical_devices('GPU')
if gpus:
    try: 
      tf.config.experimental.set_virtual_device_configuration(gpus[0],
        [tf.config.experimental.VirtualDeviceConfiguration(memory_limit=4096)])
      logical_gpus = tf.config.experimental.list_logical_devices('GPU')
      print(len(gpus), "Physical GPUs", len(logical_gpus), "Logical GPUs")
    except RuntimeError as e:
      print(e)
#####################################################

np.random.seed(42)

# set the beta distribution parameters
alpha = 2
beta = 2
calib_scale = 0.3
noise_scale = 1.
bias = 0.

#
# select a random set of panels to cover the detector module
# from the copula output
def select_random_panel_set(calib_copula, detector_shape):
    """
    Select a random set of panels from the copula predictions
    and slice 'n dice to cover the detector.

    Parameters
    ----------
    calib_copula - random draw form vine copula that describes
                   the pixel correlation structure.

    detector_shape - a tuple that describes the shape of the detector.

    Returns
    -------
    An array the shape of the detector of a draw from a copula.
    """
    depth = detector_shape[0]
    width = detector_shape[1]
    height = detector_shape[2]

    # check width and height are the same.
    assert width == height

    panel_width = int(np.sqrt(calib_copula.shape[1]))

    num_panels_side = int(np.ceil(width/panel_width))
    slice_off = int((num_panels_side * panel_width - width) / num_panels_side)

    # create empty array
    assembly = np.zeros(detector_shape)

    # loop over depth
    for d_i in range(depth):
        for p_i in range(num_panels_side):
            for p_j in range(num_panels_side):
                # get a random panel
                indx = np.random.choice(calib_copula.shape[2])
                drw = np.random.choice(calib_copula.shape[0])

                panel = calib_copula[drw,:,indx].reshape((panel_width,panel_width))
               
                w_min = p_i * (panel_width - slice_off)
                w_max = (p_i + 1) * (panel_width - slice_off)
                h_min = p_j * (panel_width - slice_off)
                h_max = (p_j + 1) * (panel_width - slice_off)
                assembly[d_i,w_min:w_max,h_min:h_max] = panel[:panel_width-slice_off,:panel_width-slice_off]


    return assembly


def draw_uncalibration(copula_assembly, alpha, beta, scale):
    """
    Draw a random uncalibration factor sample.

    Parameters
    ----------
    copula_assembly (array) - copula output in the shape of the detector module
                              (the output of select_random_panel_set).
    alpha (float) - parameter of beta distribution
    beta (float) - parameter of beta distribution
    scale (float) - resolution of the calibration (i.e. all elements are with \pm scale
        of 1.).

    Returns
    -------
    An array of 1 draw of uncalibration factors for the detector module.
    """
    uncalib_consts = stats.beta.ppf(copula_assembly.flatten(), a=alpha, b=beta).reshape(copula_assembly.shape)
    uncalib_consts = scale*(2.*np.expand_dims(uncalib_consts,len(copula_assembly.shape))-1.) + 1.

    return uncalib_consts

def draw_noise(copula_assembly, alpha, beta, scale):
    """
    Draw a random noise sample.

    Parameters
    ----------
    copula_assembly (array) - copula output in the shape of the detector module
                              (the output of select_random_panel_set).
    alpha (float) - parameter of beta distribution
    beta (float) - parameter of beta distribution
    scale (float) - scale the noise

    Returns
    -------
    An array of 1 draw of noise for the detector module.
    """
    noise = stats.beta.ppf(copula_assembly.flatten(), a=alpha, b=beta).reshape(copula_assembly.shape)
    noise = scale*(2.*noise - 1.)
    
    return np.expand_dims(noise, len(noise.shape))

def get_batch_to_score(X, batch, batch_size):
    """
    Get a batch from input data.

    Parameters
    ----------
    X (array) - input data
    batch (int) - batch number
    batch_size (int) - size of the batch

    Returns
    -------
    A slice of the input data corresponding to the batch.
    """
    return X[batch*batch_size:(batch+1)*batch_size]

def score_batch(data_dict, model, X, y, batch, batch_size, calib_copula, alpha, beta, noise_scale, bias):
    """
    Score a batch of data.

    Parameters
    ----------
    data_dict (dict) - model scores are returned in this dictionary
    model - the model
    X (array) - input data
    y (array) - target
    batch (int) - the batch number
    batch_size (int) - batch size
    calib_copula (array) - copula draws
    alpha (float) - beta distribution parameter
    beta (float) - beta distribution parameter
    noise_scale (float) - scale of noise distribution
    """
    # draw noise
    noise = np.zeros((batch_size,*X.shape[1:]))
    for i in range(batch_size):
        assembly_calibs = select_random_panel_set(calib_copula, X.shape[1:-1])
        noise[i] = draw_noise(assembly_calibs, alpha, beta, noise_scale)
    
    X_batch = get_batch_to_score(X, batch, batch_size)
    Esum_batch = X_batch.sum(axis=(1,2,3)).ravel()

    X_uncalib_batch = np.zeros(X_batch.shape)
    for i in range(batch_size):
        X_uncalib_batch[i] = X_batch[i] * uncalib_consts + noise[i] + bias
        #X_uncalib_batch[i] = noise[i]

    # apply zero suppression
    X_uncalib_batch[np.nonzero(X_uncalib_batch < 0.6)] = 0.

    Esum_uncalib_batch = X_uncalib_batch.sum(axis=(1,2,3)).ravel()

    pred = model.predict([X_uncalib_batch, Esum_uncalib_batch]).ravel()
    calib_pred = model.predict([X_batch, Esum_batch]).ravel()

    data_dict['uncalib_pred'] += list(pred)
    data_dict['calib_pred'] += list(calib_pred)
    data_dict['eGen'] += list(get_batch_to_score(y, batch, batch_size))
    data_dict['uncalib_esum'] += list(Esum_uncalib_batch)
    data_dict['calib_esum'] += list(Esum_batch)


def print_help():
    """Print a help message."""
    msg = """calib_pred.py [options].
    \t-s, --seed\tset the random seed
    \t-m, --model\tspecify the CNN model file.
    \t-c, --copula\tspecify the copula draw data file.
    \t-x, --data\tspecify the location of the input data.
    \t-a, --alpha\tnoise/calibration distribution parameter
    \t-b, --beta\tnoise/calibration distribution parameter
    \t-S, --noise-scale\tnoise distribution scale
    \t-B, --bias\tbias offset
    \t-o, --output\toutput file name
    \t-h, --help\tprint this help message."""
    print(msg)

#======================================
#  Main method if this is run from CLI
if __name__ == '__main__':

    mod_file = 'tf_Si_5ns_0p6MeV_corr_6var_v2_e25_0p00726.h5'
    cop_file = 'out.h5'
    dfile = 'sample.npz'
    output = 'cnn_uncalib_output.pkl'
    batch_size = 5

    #
    # parse command line options
    # random seed
    # model file
    # copula file
    # X data
    # alpha
    # beta
    # noise scale
    # output location
    shortargs = 's:m:c:x:a:b:S:C:o:B:hT:'
    longargs = ['seed=', 
        'model=',
        'copula=',
        'data=',
        'alpha=',
        'beta=',
        'noise-scale=',
        'calib-scale=',
        'output=',
        'bias=',
        'help',
        'batch-size=',]
    opts, args = getopt.getopt(sys.argv[1:], shortargs, longargs)
    for o, a in opts:
        if o in ('-s', '--seed'):
            np.random.seed(int(a))
        elif o in ('-m', '--model'):
            mod_file = a
        elif o in ('-c','--copula'):
            cop_file = a
        elif o in ('-x','--data'):
            dfile = a
        elif o in ('-a','--alpha'):
            alpha = float(a)
        elif o in ('-b','--beta'):
            beta = float(a)
        elif o in ('-S','--noise-scale'):
            noise_scale = float(a)
        elif o in ('-C','--calib-scale'):
            calib_scale = float(a)
        elif o in ('-o','--output'):
            output = a
        elif o in ('-B','--bias'):
            bias = float(a)
        elif o in ('-h','--help'):
            print_help()
            sys.exit(0)
        elif o in ('-T','--batch-size'):
            batch_size = int(a)
        else:
            print('{} is not a valid option'.format(o))
            print_help()
            sys.exit(1)


    model = tf.keras.models.load_model(mod_file)
    print(model.summary())

    #
    # load the miscalibration copula output
    calib_copula = tables.open_file(cop_file,'r').get_node('/calib/calib').read()


    data = np.load(dfile,allow_pickle=True)
    X=data['input']
    y=data['egen']

    data_dict = {'eGen': [],
            'calib_pred': [],
            'uncalib_pred': [],
            'calib_esum': [],
            'uncalib_esum': []}

    #
    # select a single miscalibration draw for the complete module
    assembly_calibs = select_random_panel_set(calib_copula, X.shape[1:-1])
    uncalib_consts = draw_uncalibration(assembly_calibs, alpha, beta, calib_scale)


    for l in range (int(len(y)/batch_size)):
        score_batch(data_dict, model, X, y, l, batch_size, calib_copula, alpha, beta, noise_scale, bias)


    df = pd.DataFrame(data_dict)
    df.to_pickle(output)


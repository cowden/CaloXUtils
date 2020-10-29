"""Prepare the input for the CNN from G4 Simulation."""

import pandas as pd
import numpy as np
from ROOT import TFile
import getopt
import sys


def load_extract_from_root(root_file):
    """
    Extract simulation data from ROOT file.

    Parameters
    ----------
    root_file (str) - the root file name (output of CaloX_G4).

    Returns
    -------
    Pandas dataframe
    """
    # open the file
    myfile = TFile(root_file)

    # retrieve the ntuple of interest
    mychain = myfile.Get('tree')
    entries = mychain.GetEntriesFast()

    df = pd.DataFrame()


    for jentry in range(entries):
        ientry = mychain.LoadTree(jentry)

        if ientry % 1000 == 0:
            print(ientry)
        nb = mychain.GetEntry(jentry)
        if nb<=0:
            continue


        allHits1 = dict(zip(mychain.T1HitID,mychain.T1HitEdep))
        Hits1 = dict()

        for (key, value) in allHits1.items():
            if value > 0.6:
               Hits1[key] = value

        allHits4 = dict(zip(mychain.T4HitID,mychain.T4HitEdep))
        Hits4 = dict()

        for (key, value) in allHits4.items():
            if value > 0.6:
               Hits4[key] = value


        allHits5 = dict(zip(mychain.T5HitID,mychain.T5HitEdep))
        Hits5 = dict()

        for (key, value) in allHits5.items():
            if value > 0.6:
               Hits5[key] = value


        allHits10 = dict(zip(mychain.T10HitID,mychain.T10HitEdep))
        Hits10 = dict()

        for (key, value) in allHits10.items():
            if value > 0.6:
               Hits10[key] = value

        df = df.append({
             "eGen": mychain.GenE[0],
             "Hits1": Hits1,
             "Hits4": Hits4,
             "Hits5": Hits5,
             "Hits10": Hits10,
             "SumHAD" : mychain.T5HitEsum[0] -mychain.T5emHitEsum[0] ,
             "SumEM" : mychain.T5emHitEsum[0]
             }, ignore_index=True)

    # close the root file
    myfile.Close()

    print(df.info())
    return df

# =========================================
#

def create_data_array(dd,shape=(75,50,50)):
    """
    Create an array of data from a row of a dataframe.

    Reshape the row of data into the shape of the detector.

    Parameters
    ----------
    dd (dict) - a row of a dataframe corresponding to an event in the detector
    shape (tuple: default (75,50,50)) - the shape of the detector (in pixels).

    Returns
    -------
    An array of the given shape (np.ndarray).
    """
    var = np.zeros(shape,dtype=np.float32)

    for (key, value) in dd.items():
        X=int(key/1000000)
        Y=int(int(key/1000)%1000)
        Z=int(key%1000)
        if X>-1 and Y>-1 and Z>-1 and  X<shape[2] and Y<shape[1] and Z<shape[0]: var[Z][Y][X]=value
    return var


def numpify_data(df,shape=(75,50,50)): 
    """
    Transform and reshape dataframe.

    Parameters
    ----------
    df (pd.DataFrame) - input dataframe

    Returns
    -------
    np.ndarray in shape of detector
    """
    X = np.asarray(df[['Hits5']].apply(lambda x: create_data_array(*x), axis=1).tolist()).reshape(len(df), 75, 50, 50, 1)

    y=np.asarray(df[['SumHAD','SumEM']])

    ene = np.asarray(df['eGen'])

    return X, y, ene

def print_help():
    """Print a help message."""
    msg = """prep_sim_for_cnn.py [options].
    \t-i, --input-file\tinput file name
    \t-o, --output-file\toutput file name
    \t-h, --help\tprint this help message"""
    print(msg)

#===============================
# main program
if __name__ == '__main__':
    input_file = ''
    output_file = ''

    # parse command line arguments
    # input file
    # output file
    shortargs = 'i:o:h'
    longargs = ['input-file=',
        'output-file=',
        'help']
    opts, args = getopt.getopt(sys.argv[1:], shortargs, longargs)
    for o, a in opts:
        if o in ('-i','--input-file'):
            input_file = a
        elif o in ('-o','--output-file'):
            output_file = a
        elif o in ('-h','--help'):
            print_help()
            sys.exit(0)
        else:
            print('{} is not a valid option'.format(o))
            print_help()
            sys.exit(1)

    df = load_extract_from_root(input_file)

    X, y, ene = numpify_data(df)

    np.savez_compressed(output_file, input=X, output=y,egen=ene)

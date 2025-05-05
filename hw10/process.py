import matplotlib.pyplot as plt
import numpy as np

def fft_py(sig):

    t = sig[0]
    Fs = len(sig[0])/sig[0][-1] # sample rate

    Ts = 1.0/Fs; # sampling interval
    ts = np.arange(0,t[-1],Ts) # time vector
    y = sig[1] # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    return frq, abs(Y)

def maf_py(sig, avg_window):
    res = []
    for i in range (avg_window, len(sig[0])):
        res.append([sig[0][i], sum(sig[1][i-avg_window:i])/avg_window])
    return np.array(res).transpose()

def iir_py(sig, a):
    res = [[0,0]]
    for i in range (len(sig[0])):
        res.append([sig[0][i], a*res[-1][1] + (1-a)*sig[1][i]])
    return np.array(res).transpose()

def fir_py(sig):
    h = [
        0.000000000000000000,
        0.000040262106974371,
        0.000170024258399472,
        0.000406690199474799,
        0.000772918706884466,
        0.001296076521368535,
        0.002007023183673962,
        0.002938268639323423,
        0.004121602274626644,
        0.005585344389067255,
        0.007351413495736279,
        0.009432431456742390,
        0.011829100502165908,
        0.014528080108850198,
        0.017500567401953101,
        0.020701743512651884,
        0.024071192879055254,
        0.027534336698251928,
        0.031004850406466316,
        0.034387963495681552,
        0.037584473605140306,
        0.040495250787391433,
        0.043025966563228178,
        0.045091759205407367,
        0.046621543646666895,
        0.047561692004237828,
        0.047878847901159989,
        0.047561692004237828,
        0.046621543646666902,
        0.045091759205407367,
        0.043025966563228192,
        0.040495250787391440,
        0.037584473605140313,
        0.034387963495681552,
        0.031004850406466326,
        0.027534336698251945,
        0.024071192879055264,
        0.020701743512651888,
        0.017500567401953118,
        0.014528080108850200,
        0.011829100502165906,
        0.009432431456742402,
        0.007351413495736279,
        0.005585344389067264,
        0.004121602274626658,
        0.002938268639323424,
        0.002007023183673965,
        0.001296076521368534,
        0.000772918706884464,
        0.000406690199474800,
        0.000170024258399472,
        0.000040262106974370,
        0.000000000000000000,
    ]

    res = []
    for i in range (len(h), len(sig[0])):
        res.append([sig[0][i], np.dot(sig[1][i-len(h):i], h)])
    return np.array(res).transpose()

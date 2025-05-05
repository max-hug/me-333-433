import matplotlib.pyplot as plt # for plotting
import numpy as np # for sine function
import csv
import process

def load_csv(file_name):
    data = []
    with open(file_name) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            data.append([float(row[0]), float(row[1])])
    return np.array(data).transpose()

samp_rate = lambda sig: len(sig[0])/sig[0][-1]

sigA = load_csv('sigA.csv')
sigB = load_csv('sigB.csv')
sigC = load_csv('sigC.csv')
sigD = load_csv('sigD.csv')

sigs = [sigA, sigB, sigC, sigD]
names = ['Signal A', 'Signal B', 'Signal C', 'Signal D']

def main():

    #-----------ffts------------------------------------------------------
    index = 0
    f, axs = plt.subplots(2,len(sigs))
    f.set_figwidth(16)
    f.set_figheight(8)
    for sig, name in zip(sigs, names):
        frq, res = process.fft_py(sig)

        axs[0][index].plot(sig[0],sig[1],'b-*')
        axs[0][index].set_title('{}'.format(name))

        axs[1][index].loglog(frq,res,'b-*')
        axs[1][index].set_xlabel('freq [Hz]')
        axs[1][index].set_ylabel('Intensity')
        axs[1][index].set_title('FFT of {}'.format(name))
        index += 1
    plt.subplots_adjust(wspace=0.25, hspace=0.25)  # Adjust spacing
    plt.savefig('ffts.png', dpi=200)

    #-----------maf------------------------------------------------------
    window_size = 60
    index = 0
    f, axs = plt.subplots(2,2)
    f.set_figwidth(12)
    f.set_figheight(8)
    axs = axs.ravel()
    for sig, name in zip(sigs, names):
        res = process.maf_py(sig, window_size)

        axs[index].plot(sig[0],sig[1],'b-*')
        axs[index].plot(res[0],res[1],'r-*')
        axs[index].set_xlabel('time [s]')
        axs[index].set_ylabel('Signal')
        axs[index].set_title('MAF of {}, window = {}'.format(name, window_size))
        index += 1
    plt.subplots_adjust(wspace=0.35, hspace=0.35)  # Adjust spacing
    plt.savefig('mafs.png', dpi=200)

    #-----------iir------------------------------------------------------
    a = 0.96
    index = 0
    f, axs = plt.subplots(2,2)
    f.set_figwidth(12)
    f.set_figheight(8)
    axs = axs.ravel()
    for sig, name in zip(sigs, names):
        res = process.iir_py(sig, a)

        axs[index].plot(sig[0],sig[1],'b-*')
        axs[index].plot(res[0],res[1],'r-*')
        axs[index].set_xlabel('time [s]')
        axs[index].set_ylabel('Signal')
        axs[index].set_title('IIR of {}, A = {:.2f}, B = {:.2f}'.format(name, a, 1-a))
        index += 1
    plt.subplots_adjust(wspace=0.35, hspace=0.35)  # Adjust spacing
    plt.savefig('iirs.png', dpi=200)

    #-----------fir------------------------------------------------------
    index = 0
    f, axs = plt.subplots(2,2)
    f.set_figwidth(12)
    f.set_figheight(8)
    axs = axs.ravel()
    for sig, name in zip(sigs, names):
        res = process.fir_py(sig)

        axs[index].plot(sig[0],sig[1],'b-*')
        axs[index].plot(res[0],res[1],'r-*')
        axs[index].set_xlabel('time [s]')
        axs[index].set_ylabel('Signal')
        axs[index].set_title('{}, Blackman Filter with Cutoff \n100Hz and transition 900Hz'.format(name))
        index += 1
    plt.subplots_adjust(wspace=0.35, hspace=0.35)  # Adjust spacing
    plt.savefig('firs.png', dpi=200)


    plt.show()

main()

#-----------ffts------------------------------------------------------
# index = 0
# f, axs = plt.subplots(1,len(sigs))
# for sig, name in zip(sigs, names):
#     frq, res = process.fft_py(sig)
#     maf_frq, maf_res = process.fft_py(process.maf_py(sig, 30))

#     axs[index].loglog(frq,res,'b-*')
#     axs[index].set_xlabel('freq [Hz]')
#     axs[index].set_ylabel('Intensity')
#     axs[index].set_title('FFT of {}'.format(name))

#     axs[index].loglog(maf_frq,maf_res,'r-*')
#     axs[index].set_xlabel('freq [Hz]')
#     axs[index].set_ylabel('Intensity')
#     axs[index].set_title('FFT of MAF {}'.format(name))
#     index += 1
# plt.subplots_adjust(wspace=0.25, hspace=0.25)  # Adjust spacing
# plt.show()
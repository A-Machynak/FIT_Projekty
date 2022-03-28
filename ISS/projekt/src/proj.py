""" 
ISS Project
"""

import numpy as np
from scipy.io import wavfile
from scipy import signal
import matplotlib.pyplot as plt

def read_file(file_name):
    # 4.1
    fs, data = wavfile.read(file_name)
    print(
        f'Sample rate: {fs} Samples: {data.size} Length: {data.size/fs}[s] Min: {data.min()} Max: {data.max()}'
    )
    return fs, data


def plot_wav(fs, data, out_file_name):
    t = np.arange(data.size) / fs

    plt.figure(figsize=(8, 4))
    plt.plot(t, data)
    plt.gca().set_xlabel('$t[s]$')
    plt.gca().set_title('Zvukový signál')

    plt.savefig(f"plots/{out_file_name}")


def plot_fft(fs, data, out_file_name):
    N = data.size

    plt.figure(figsize=(8, 4))
    x = np.linspace(0.0, fs / 2, N // 2)
    #print(f'{x[0]} {x[1]} {x[2]} {N} {N // 2}')

    fft = np.sqrt(data.real**2 + data.imag**2)

    plt.plot(x, fft[:N // 2])
    plt.gca().set_xlabel('$f[Hz]$')
    plt.gca().set_title('Výstup DFT')

    plt.savefig(f"plots/{out_file_name}")


def plot_spectro(fs, data, out_file_name):
    f, t, spect = signal.spectrogram(data, fs, nperseg=1024, noverlap=512)

    plt.figure(figsize=(8, 4))
    plt.pcolormesh(t, f, 10 * np.log10(spect**2))
    plt.gca().set_xlabel('$t[s]$')
    plt.gca().set_ylabel('$f[Hz]$')
    plt.gca().set_title('Spektrogram')

    plt.savefig(f"plots/{out_file_name}")


def plot_poles_zeros(zeros, poles, out_file_name):
    plt.figure(figsize=(6, 6))

    # jednotkova kruznice
    ang = np.linspace(0, 2 * np.pi, 100)
    plt.plot(np.cos(ang), np.sin(ang))

    # nuly, poly
    plt.scatter(np.real(zeros),
                np.imag(zeros),
                marker='o',
                facecolors='none',
                edgecolors='r',
                label='Nuly')
    plt.scatter(np.real(poles),
                np.imag(poles),
                marker='x',
                color='g',
                label='Póly')
    plt.gca().set_xlabel('Realná složka $\mathbb{R}\{$z$\}$')
    plt.gca().set_ylabel('Imaginarní složka $\mathbb{I}\{$z$\}$')
    plt.grid(alpha=0.5, linestyle='--')

    plt.savefig(f"plots/{out_file_name}")


def plot_filter_freqz(b, a, fs, out_file_name):
    w, H = signal.freqz(b, a)
    _, ax = plt.subplots(1, 2, figsize=(12, 4))

    ax[0].plot(w / 2 / np.pi * fs, np.abs(H))
    ax[0].set_xlabel('Frekvence [Hz]')
    ax[0].set_title('Modul frekvenční charakteristiky $|H(e^{j\omega})|$')

    ax[1].plot(w / 2 / np.pi * fs, np.angle(H))
    ax[1].set_xlabel('Frekvence [Hz]')
    ax[1].set_title(
        'Argument frekvenční charakteristiky $\mathrm{arg}\ H(e^{j\omega})$')

    for ax1 in ax:
        ax1.grid(alpha=0.5, linestyle='--')

    plt.savefig(f"plots/{out_file_name}")


def plot_impulse_response(b, a, out_file_name):
    # impulsni odezva
    N_imp = 32
    imp = [1, *np.zeros(N_imp - 1)]  # jednotkovy impuls
    h = signal.lfilter(b, a, imp)

    plt.figure(figsize=(5, 3))
    plt.stem(np.arange(N_imp), h, basefmt=' ')
    plt.gca().set_xlabel('$n$')
    plt.gca().set_title('Impulsní odezva $h[n]$')

    plt.grid(alpha=0.5, linestyle='--')

    plt.savefig(f"plots/{out_file_name}")


def normalize(data):
    # 4.2
    new_data = data.copy()
    m = np.mean(new_data, axis=0)
    new_data -= int(m)
    abs_max = np.maximum(np.abs(data.min()), data.max())
    print(f'Mean value: {m} Absolute max: {abs_max}')

    new_data = new_data / abs_max
    print(f'(Normalized) New max: {new_data.max()} New min: {new_data.min()}')
    return new_data


def get_n_max(data, n):
    # Get 'n' highest values from array
    arr_max = []
    for a in range(0, n):
        arr_max.append([0.0, 0.0])

    for a in range(0, data.size):
        calc_val = np.sqrt(data[a].real**2 + data[a].imag**2)
        #print(f'{calc_val} = {data[a].real} + {data[a].imag}; {15.65557729941 * a}')

        for i in range(0, n):
            # Found new higher value?
            if calc_val > arr_max[i][0]:
                # Shift all the other ones - pop last one
                arr_max[i + 1:n] = arr_max[i:n - 1]
                arr_max[i] = [calc_val, a]
                break

    return arr_max


def print_highest_vals(data, n):
    arr = get_n_max(data, n)

    for i in range(0, len(arr)):
        val = (arr[i][1] * _fs) / len(_split_data[0])
        if val < _fs / 2:
            print(f'{arr[i][0]} : {arr[i][1]} : {val} Hz')


def split_512(data):
    new_data = []
    i = 0

    for i in range(0, data.size - 1024, 512):
        new_data.append(data[i:i + 1024])

    # the last missing part of our signal (can't divide it by 1024)
    i += 512
    if i < data.size:
        new_data.append(data[i:data.size])

    #print(f'[0]: {data[0]} [0][0]: {new_data[0][0]} [512]: {data[512]} [1][0]: {new_data[1][0]}')
    #print(f'last: {data[data.size-1]} ?= {new_data[len(new_data)-1][len(new_data[len(new_data)-1])-1]}')

    return new_data


def my_dft(data):
    N = data.size
    # make the dft matrix
    a, b = np.meshgrid(np.arange(0, N), np.arange(0, N))

    # e^ -2*PI*J/N
    w = np.exp(-2 * np.pi * 1J / N)
    matrix = np.power(w, a * b) / np.sqrt(N)

    # insert the missing '1's
    #matrix[0].fill(1)
    #matrix[:, 0].fill(1)
    # or not...hmmm... wat

    result = data.dot(matrix)

    return result


def gen_cosine_waves(fs, data, F):
    AMPL = 500

    t = data.size / fs

    samples = np.arange(t * fs) / fs

    cos = [
        np.int16(np.cos(2 * np.pi * F[0] * samples) * AMPL),
        np.int16(np.cos(2 * np.pi * F[1] * samples) * AMPL),
        np.int16(np.cos(2 * np.pi * F[2] * samples) * AMPL),
        np.int16(np.cos(2 * np.pi * F[3] * samples) * AMPL)
    ]

    return cos


def z_filter_zeros(fs, F):
    omegak = []
    nk = []

    for i in range(0, len(F)):
        omegak.append(2 * np.pi * (F[i] / fs))
        nk.append(np.exp(1J * omegak[i]))

    nk = [*nk, *np.conj(nk)]
    print(f'NK: {nk}')
    return nk


def z_filter(nk):
    coeff = np.poly(nk)
    print(f'Coeff: {coeff}')

    return coeff


_fs, _data_orig = read_file('../audio/xmachy02.wav')
#plot_wav(fs, data, 'plot_4_1.png')

_abs_max = np.maximum(np.abs(_data_orig.min()), _data_orig.max())
_data = normalize(_data_orig)
#plot_wav(fs, data, 'plot_4_2.png')

_split_data = split_512(_data)

_chosen_frame = _split_data[22]

# FFT
#_fft_result = my_dft(_chosen_frame)
#_fft_result = np.fft.fft(_chosen_frame)
#plot_fft(_fs, _fft_result, 'fft.png')

# Audio in spectrogram
#plot_spectro(_fs, _data, 'in_spectro.png')

#print_highest_vals(_fft_result, 10)
# 687.5 1359.375 2046.875 2734.375
# ~ [682.8125, 1365.625, 2048.4375, 2731.25]
_F = [682.8125, 682.8125 * 2, 682.8125 * 3, 682.8125 * 4]
""" 
# Generate 4 cos on freq F
_coss = gen_cosine_waves(_fs, _data, _F)
_4cos = _coss[0] + _coss[1] + _coss[2] + _coss[3]
wavfile.write('../audio/4cos.wav', _fs, _4cos)
plot_spectro(_fs, _4cos, '4cos_spectro.png')
"""

_filter_zeros = z_filter_zeros(_fs, _F)
#plot_poles_zeros(_filter_zeros, [(0.0, 0.0J)], 'z_filter_zp.png')

_filter_b = z_filter(_filter_zeros)
#_filter_a = [1.0]
#plot_filter_freqz(_filter_b, _filter_a, _fs, 'z_filter_freqz.png')
#plot_impulse_response(_filter_b, _filter_a, 'z_filter_IR.png')

_out = np.int16(np.convolve(_data, _filter_b) * _abs_max)
#plot_wav(_fs, _data, 'test.png')
plot_wav(_fs, _out, 'filtered.png')
#_out = np.int16(signal.lfilter(_filter_b, _filter_a, _data) * 32768)
#y = np.array([np.convolve(xi, _filter_b, mode='valid') for xi in _data])

plot_spectro(_fs, _out, 'out_spectro.png')

wavfile.write('../audio/clean_z.wav', _fs, _out)

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.pyplot as plt
from scipy.signal import butter, lfilter, freqz
import mylib as m


def butter_lowpass(cutoff, fs, order=5):
    """
    Design a Butterworth lowpass filter

    Parameters:
    cutoff (float): Cutoff frequency (Hz)
    fs (float): Sampling frequency (Hz)
    order (int): Filter order

    Returns:
    b, a (ndarray, ndarray): Numerator and denominator polynomials of the IIR filter
    """
    nyq = 0.5 * fs  # Nyquist frequency
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype="low", analog=False)
    return b, a


def lowpass_filter(data, cutoff, fs, order=5):
    """
    Apply lowpass filter to signal

    Parameters:
    data (array): Input signal
    cutoff (float): Cutoff frequency (Hz)
    fs (float): Sampling frequency (Hz)
    order (int): Filter order

    Returns:
    y (array): Filtered signal
    """
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y


# Example usage
if __name__ == "__main__":
    # Generate a test signal
    fs = 1000  # Sampling frequency (Hz)
    t = np.arange(0, 1, 1 / fs)  # Time vector

    # Signal components: 10Hz (our target) and 50Hz (noise)
    signal = np.sin(2 * np.pi * 10 * t) + 0.5 * np.sin(2 * np.pi * 50 * t)

    # Filter parameters
    cutoff = 30  # Cutoff frequency (Hz)
    order = 6  # Filter order
    m.plot_fft_magnitude(signal, fs)

    # Apply lowpass filter
    filtered_signal = lowpass_filter(signal, cutoff, fs, order)
    m.plot_fft_magnitude(filtered_signal, fs)

    # Plot results
    plt.figure(figsize=(12, 6))

    # Plot frequency response
    plt.subplot(2, 1, 1)
    b, a = butter_lowpass(cutoff, fs, order)
    w, h = freqz(b, a, worN=8000)
    plt.plot(0.5 * fs * w / np.pi, np.abs(h), "b")
    plt.plot(cutoff, 0.5 * np.sqrt(2), "ko")
    plt.axvline(cutoff, color="k")
    plt.xlim(0, 0.5 * fs)
    plt.title("Lowpass Filter Frequency Response")
    plt.xlabel("Frequency [Hz]")
    plt.ylabel("Gain")
    plt.grid()

    # Plot time domain signals
    plt.subplot(2, 1, 2)
    plt.plot(t, signal, "b-", label="Original Signal")
    plt.plot(t, filtered_signal, "g-", linewidth=2, label="Filtered Signal")
    plt.title("Original and Filtered Signals")
    plt.xlabel("Time [s]")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.grid()

    plt.tight_layout()
    plt.show()


# coding: utf-8

# # Important Util Methods

# In[289]:


import numpy as np
from scipy.signal import butter, lfilter

def butter_bandpass(lowcut, highcut, fs, order=4):
    nyq = 0.5 * fs #Nyquist frequeny is half the sampling frequency
    low = lowcut / nyq 
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def bandpass_filter(data, lowcut, highcut, fs, order=4):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

def butter_lowpass(cutoff, fs, order=4):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low')
    return b, a

def lowpass_filter(data, cutoff, fs, order=4):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = lfilter(b, a, data)
    return y

def calHeartRates(peak_list, fs):
    heart_rates_list = list()
    count = 0
    while (count < (len(peak_list))-1):
        peak_dist = (peak_list[count+1] - peak_list[count]) #Calculate interval between peaks in terms of number of samples
        heart_rates_list.append(60.0 * fs/peak_dist) #Append to list
        count += 1
    return heart_rates_list;

def plotSignal(sample_length, data, peaks, ybeat):
    plt.figure(figsize=(8,4))
    plt.title("Detected peaks in signal")
    plt.xlim(0, sample_length)
    plt.plot(data, alpha=0.5, color='blue') #Plot semi-transparent heart rate
    plt.scatter(peaks, ybeat, color='red') #Plot detected peaks
    plt.show()
        
def calBPM(heart_rates_list):
    bpm = np.mean(heart_rates_list)
    print("Average Heart Beat is: %.2f" %bpm) #Round off to 2 decimal and print
    
def calRespirationRates(dataset, peak_list, fs):
    interval_list = list()
    count = 0
    while (count < (len(peak_list))-1):
        #Calculate interval between peaks in terms of number of samples
        time_interval = (dataset.time[peak_list[count+1]] - dataset.time[peak_list[count]])
        interval_list.append(60/time_interval) #Convert to time interval and append to list
        count += 1
    return interval_list;

def calAvgRespirationRate(respiration_rates_list):
    resp_rate = np.mean(respiration_rates_list)
    print("Average Respiration rate is: %.01f" %resp_rate) #Round off to 1 decimal and print
    
def getAC_DC_ValuesForSignal(data):
    ## Calculate min and max peak indices and values
    min_peaks = argrelextrema(data, np.less)
    max_peaks = argrelextrema(data, np.greater)

    min_peak_values = [data[x] for x in min_peaks[0]] #Get the y-value of all min peaks for interpolation
    max_peak_values = [data[index] for index in max_peaks[0]] #Get the y-value of all max peaks to calculate amplitude

    ## Interpolate
    x = min_peaks[0];
    y = min_peak_values;
    f = interpolate.interp1d(x, y, fill_value="extrapolate")

    interpolated_min_peak_values = f(max_peaks[0])
    AC_values = (max_peak_values - interpolated_min_peak_values)
    DC_values = interpolated_min_peak_values
    return (AC_values, DC_values)

    
def calSPO2(ratioAverage): 
    return -45.060*ratioAverage* ratioAverage + 30.354 *ratioAverage + 94.845

def calAvgSPO2(ratio_avg_list):
    SPO2_values = [calSPO2(ratio_avg) for ratio_avg in ratio_avg_list]
    print("Average SPO2 is:", np.mean(SPO2_values))  


# # FFT Transform

# In[101]:


dataset = pd.read_csv("C:/Users/k2khanna/sensor_data.csv")
levelled_sp = dataset.IR - np.mean(dataset.IR)
fft_signal = np.fft.fft(levelled_sp)
timestep = 0.02
freq = np.fft.fftfreq(dataset.shape[0], d=timestep)
plt.figure(figsize=(8,4))
plt.plot(abs(freq), abs(fft_signal))
plt.title("frequency v/s signal")
plt.xlim(0, 10)
plt.show()


# # Heart Rate Calculation

# In[290]:


from scipy.signal import argrelextrema
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import math

dataset = pd.read_csv("C:/Users/k2khanna/sensor_data.csv")
Fs = 50;
T = 0.02;
mean_IR = np.mean(dataset.IR)
dataset.IR = dataset.IR - mean_IR
dataset.IR = bandpass_filter(dataset.IR, 0.5 ,2, Fs) #Lower cutoff freq: 0.5, Higher cutoff freq: 2
peak_list = argrelextrema(dataset.IR.values, np.greater)[0]

ybeat = [dataset.IR[x] for x in peak_list] #Get the y-value of all peaks for plotting purposes
plotSignal(len(dataset), dataset.IR, peak_list, ybeat)
    


# In[291]:


heart_rates = calHeartRate(peaklist, 50)
calBPM(heart_rates)
print("Heart Rates obtained:")
for rate in heart_rates:
    print("%.2f" %rate) #Round off to 1 decimal and print


# # Respiration Rate Calculation

# In[292]:


from scipy.signal import argrelextrema
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import math

dataset = pd.read_csv("C:/Users/k2khanna/sensor_data.csv")
Fs = 50;
T = 0.02;
mean_IR = np.mean(dataset.IR)
dataset.IR = dataset.IR - mean_IR
dataset.IR = bandpass_filter(dataset.IR, 0.165, 0.3, Fs)
peak_list = argrelextrema(dataset.IR.values, np.greater)[0]

ybeat = [dataset.IR[x] for x in peak_list] #Get the y-value of all peaks for plotting purposes
plotSignal(len(dataset), dataset.IR, peak_list, ybeat)
respiration_rates = calRespirationRate(dataset, peak_list, Fs)
calAvgRespirationRate(respiration_rates)
for rate in respiration_rates:
    print("%.01f" %rate) #Round off to 1 decimal and print


# # SPO2 Calculation

# In[267]:


from scipy.signal import argrelextrema
from scipy import interpolate
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import math

dataset = pd.read_csv("C:/Users/k2khanna/sensor_data.csv")
Fs = 50;
T = 0.02;
IR_array = dataset.IR.values
RED_array = dataset.RED.values

IR_array = lowpass_filter(IR_array, 2, Fs)[100:]
RED_array = lowpass_filter(RED_array, 2, Fs)[100:]

plt.plot(IR_array)
plt.show()
plt.plot(RED_array)
plt.show()



# In[278]:


## IR Calculations
AC_DC_values_IR = getAC_DC_ValuesForSignal(IR_array);

IR_AC_val = AC_DC_values_IR[0]
IR_DC_val = AC_DC_values_IR[1]


# In[279]:


## RED Calculation
AC_DC_values_RED = getAC_DC_ValuesForSignal(RED_array);

RED_AC_val = AC_DC_values_RED[0]
RED_DC_val = AC_DC_values_RED[1]


# In[293]:


ratio_avg_list = list()
for i in range(0, len(IR_AC_val)):
    ratioAverage = (RED_AC_val[i] * IR_DC_val[i])/(IR_AC_val[i] * RED_DC_val[i])
    ratio_avg_list.append(ratioAverage)
    
SPO2_values = [calSPO2(ratio_avg) for ratio_avg in ratio_avg_list]
## Calculate the average SPO2 values  
calAvgSPO2(ratio_avg_list)

for val in SPO2_values:
    print("%.3f" %val) #Round off to 2 decimal and print


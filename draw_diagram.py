import matplotlib.pyplot as plt
import numpy as np
import math as m

def round_significant(num, digits=1):
    if num == 0:
        return 0
    digit_num = 10 ** (digits - 1 - m.floor(m.log10(abs(num))))
    return round(num * digit_num) / digit_num

iter_num = 100
version_num = 0
delta = [0, 0, 0, 0]

file_names = ['data/plot_Naive.png',
              'data/plot_Array.png',
              'data/plot_256.png',
              'data/plot_256_full_pipeline.png']
graph_names = ['Наивная реализация',
               'Реализация на массивах',
               'Реализация с интринсиками размера 256 бит',
               'Реализация с интринсиками размера 256 бит с полным конвейером']

with open('Output.txt', 'r') as f:
   data = [int(line.strip()) for line in f]

for version_num in range (0, 4):
    plt.hist(data [version_num * iter_num:(version_num + 1) * iter_num], bins = iter_num, color = 'blue', edgecolor = 'black')
    plt.grid(axis='y', linestyle='--')
    plt.title(graph_names [version_num])
    plt.xlabel('Задержка (такты)')
    plt.ylabel('Количество повторений')
    plt.savefig(file_names [version_num], dpi = 300)
    plt.clf ()

    avg = 0
    for i in range (version_num * iter_num, (version_num + 1) * iter_num):
        avg += data [i]
    avg /= 100

    delta [version_num] = 0
    for i in range (version_num * iter_num, (version_num + 1) * iter_num):
        delta [version_num] += (avg - data [i])**2
    delta [version_num] /= iter_num
    delta [version_num] = m.sqrt (delta [version_num])

    print ('Погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))

# plt.hist(data [100:200], bins = 100, color = 'blue', edgecolor = 'black')
# plt.grid(axis='y', linestyle='--')
# plt.title('Latency distribution (Array)')
# plt.xlabel('Latency')
# plt.ylabel('Quantity')
# plt.savefig('data/plot_Array.png', dpi = 300)
# plt.clf ()
#
# avg = 0
# for i in range (100, 200):
#    avg += data [i]
# avg /= 100
#
# delta = 0
# for i in range (100, 200):
#    delta += (avg - data [i])**2
# delta /= 100
# delta = m.sqrt (delta)
#
# print ('Погрешность версии на массивах равна ', delta)
#
# plt.hist(data [200:300], bins = 100, color = 'blue', edgecolor = 'black')
# plt.grid(axis='y', linestyle='--')
# plt.title('Latency distribution (256 intrinsics)')
# plt.xlabel('Latency')
# plt.ylabel('Quantity')
# plt.savefig('data/plot_256.png', dpi = 300)
# plt.clf ()
#
# avg = 0
# for i in range (200, 300):
#    avg += data [i]
# avg /= 100
#
# delta = 0
# for i in range (200, 300):
#    delta += (avg - data [i])**2
# delta /= 100
# delta = m.sqrt (delta)
#
# print ('Погрешность версии с интринсиками равна ', delta)
#
# plt.hist(data [300:], bins = 100, color = 'blue', edgecolor = 'black')
# plt.grid(axis='y', linestyle='--')
# plt.title('Latency distribution (256 intrinsics in full pipeline)')
# plt.xlabel('Latency')
# plt.ylabel('Quantity')
# plt.savefig('data/plot_256_full_pipeline.png', dpi = 300)
# plt.clf ()
#
# avg = 0
# for i in range (300, 400):
#    avg += data [i]
# avg /= 100
#
# delta = 0
# for i in range (300, 400):
#    delta += (avg - data [i])**2
# delta /= 100
# delta = m.sqrt (delta)
#
# print ('Погрешность версии с интринсиками и загруженным конвейером равна ', delta)

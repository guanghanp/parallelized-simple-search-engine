import glob,os, re
import matplotlib
import matplotlib.pyplot as plt
DIR_NAME = '/global/u2/x/xizhang1/parallelized-simple-search-engine/crawler/'
memo = {}
plt.rcParams['font.family'] = 'DeJavu Serif'
plt.rcParams['font.serif'] = ['Times New Roman']
pages = [26, 956, 3806, 7285]

openmp_v1_memo = []
openmp_v2_memo = []
serial_memo = []
path_v1 = DIR_NAME + 'openmp_v1.out'
path_v2 = DIR_NAME + 'openmp_v2.out'
serial_path = DIR_NAME + 'serial_crawler.out'

for line in open(path_v1):
    if line.startswith("Simulation Time ="):
        depth = re.findall(r"depth of ([\d]+) crawling", line)
        time = re.findall(r"Simulation Time = ([\d]+\.[\d]+) seconds", line)
        openmp_v1_memo.append(float(time[0]))

for line in open(path_v2):
    if line.startswith("Simulation Time ="):
        depth = re.findall(r"depth of ([\d]+) crawling", line)
        time = re.findall(r"Simulation Time = ([\d]+\.[\d]+) seconds", line)
        openmp_v2_memo.append(float(time[0]))

for line in open(serial_path):
    if line.startswith("Simulation Time ="):
        depth = re.findall(r"depth of ([\d]+) crawling", line)
        time = re.findall(r"Simulation Time = ([\d]+\.[\d]+) seconds", line)
        serial_memo.append(float(time[0]))

# plot the result
# for different problem size
plt.xlabel("Log Pages Count")
plt.ylabel("Log Runtime")
plt.grid()

plt.loglog(pages, openmp_v1_memo, marker="o", label="openmp_v1")
plt.loglog(pages, openmp_v2_memo, marker="o", label="openmp_v2")
plt.loglog(pages, serial_memo, marker="o", label="serial")
plt.legend()
plt.title("Runtime Comparison among Parallel and Serial Implementations of Crawling")
plt.savefig("Parallel Efficiency")

import glob,os, re
import matplotlib
import matplotlib.pyplot as plt
DIR_NAME = '/global/u2/x/xizhang1/parallelized-simple-search-engine/crawler/'
memo = {}
plt.rcParams['font.family'] = 'DeJavu Serif'
plt.rcParams['font.serif'] = ['Times New Roman']
mapToPages = { 1: 26, 2: 956, 3: 3806, 4: 7285}
for pages_num in [26, 956, 3806, 7285]:
    memo[pages_num] = {}
for p in [1, 4, 16, 36, 68]:
    path = DIR_NAME + 'openmpv2-' + str(p) + '-core-' 
    for outfile in glob.glob(path + "*" + ".out"):
        for line in open(outfile):
            if line.startswith("Simulation Time ="):
                depth = re.findall(r"depth of ([\d]+) crawling", line)
                time = re.findall(r"Simulation Time = ([\d]+\.[\d]+) seconds", line)
                pages_num = mapToPages[int(depth[0])]
                memo[pages_num][p] = float(time[0]) 


# for different problem size
plt.xlabel("Log Pages Count")
plt.ylabel("Log Runtime")
plt.grid()
for n in [26, 956, 3806, 7285]:
    ps = [1, 4, 16, 36, 68]
    time = [memo[n][p] for p in ps]
    plt.loglog(ps, time, marker="o", label=str(n) + " pages")
    Strong_eff = memo[n][1] / (memo[n][68] * 68) * 100
    plt.text(1, memo[n][1], str( "{:.3f}".format(Strong_eff)) + "%")
    plt.legend()
plt.title("Strong Scaling Efficiency for Multiple Crawling Depth in Log scale")
plt.savefig("Strong_Scaling_Efficiency_crawling_v1")
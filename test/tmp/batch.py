import os
from multiprocessing import Process

path = "./constP4-524/"
filelist = os.listdir(path)

arr=[]

for file in filelist:
    if  file[-2:]  =='gz':
        print(path+file)
        def task():
          # os.system("count_select "+path+file +" \"c_ackland == 2\" ./result/constP")
          os.system("output "+path+file +" \"c_voro[1] c_steinhart[1] c_steinhart[2] c_steinhart[3] c_steinhart[4]\" ")
        p = Process(target=task)
        arr.append(p)
        p.start()
for p in arr:
  p.join()

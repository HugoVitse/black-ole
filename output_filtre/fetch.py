import os

for i in range(102,120):
    nb = str(i).zfill(3) 
    command = f"wget http://192.168.1.60/Users/Hugo/blackhole/black-ole-main/black-ole-main/output/frame_{nb}.ppm" 
    os.system(command)

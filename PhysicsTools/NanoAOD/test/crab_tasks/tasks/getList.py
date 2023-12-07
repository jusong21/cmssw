#import os
#
#path = './'
#file_list = os.listdir(path)
#file_list_py = [file for file  in file_list if file.endwith('.py')]


import os

path = "./"
files = os.listdir(path)

with open('list.txt', 'w') as f:
    for file in files:
        if 'crab' in file:
            f.write(file)
            f.write('\n')

#for (root, directories, files) in os.walk(dir_path):
#    for file in files:
#        if '.py' in file:
#            file_path = os.path.join(root, file)
#            print(file_path)

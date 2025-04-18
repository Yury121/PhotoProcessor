# This procedure read data from database
# Create directory "images" with person images and creates
# diferent arrays fo analisis
# Create adjacent matrix

import sqlite3
import numpy as np
import os
import math
#from PIL import Image

# ------------------------------------------------------------
# TOOLS
#

def ClearFolder(fpath):
  for filename in os.listdir(fpath):
    file_path = os.path.join(fpath, filename)
    try: 
      if os.path.isfile(file_path):
        os.remove(file_path)
        #print(f"Deleted: {filename}")
#      else:
#        if os.path.isdir(file_path): 
#          remove_all_files(file_path)  # Recursively call for subdirectories
    except Exception as e:
      print(f"Error deleting file/folder: {e}")  
# ------------------------------------------------------------
def StoreTextList(m_list, fname) : #store simple list 
  with open(fname, "w") as file:
    for item in m_list:
        file.write(item + "\n")


# ------------------------------------------------------------
def ReadTextList(fname):
  with open(fname, "r") as file:
    loaded_list = [line.strip() for line in file]  
    #print(loaded_list)
    return loaded_list
  return []
# ------------------------------------------------------------
def IdAddCalc(src, num, second):
  # src = array[128, float32); num = argsort(source) turnicate 128, second = array[256, float32]
  # np.dot(src,src) = 1
  sec = np.zeros((len(src)), dtype = np.float32)
  for i in range(128):
    sec[i] = second[num[i]]
  nn = np.dot(sec, sec)
  if nn > 0 :
    return(np.dot(src,sec)/nn)
  return(0.)


# ------------------------------------------------------------
#     MAIN WORK
# ------------------------------------------------------------
print(' --------------------- MAIN WORK ---------------------------------')
ClearFolder('./images/')

# select data from database and create


db_file = "__vb.db3" #"vb.db3"
conn = sqlite3.connect(db_file)
cursor = conn.cursor()
sql = "SELECT ID, IDMAIN, NORM, COSIN, IMAGE FROM FACESET WHERE COSIN NOTNULL  ORDER BY ID"
cursor.execute(sql)
rows = cursor.fetchall()
threshold1 = 0.7648 # define threshold
threshold = 0.648 #0.70 # define threshold

# Extract COSIN values
cosines = []
idd1 = []
idMain = []
norm =[]
simg = []
n = 0
for row in rows:
    idd1.append(row[0])
    idMain.append(row[1])
    norm.append(row[2])
    cosines.append(np.frombuffer(row[3], dtype=np.float32))
    path = './images/'+str(row[0])+'.jpg'
    simg.append( str(row[0])+'.jpg')
 
    #print(path)
    with open(path, 'wb') as file:
        file.write(row[4]);
    n = n+1
conn.close()
print('end read database')
#

arr = np.array(cosines)
anorm = np.array(norm)
idd = np.array(idd1, dtype=int)
StoreTextList(simg, './images/imgnames.txt')
np.save('./images/norma.npy', anorm)
np.save('./images/cosine.npy', arr)
np.save('./images/idd.npy', idd)
#print(idd)

ss = np.zeros((anorm.shape[0], anorm.shape[0]+2), dtype=int)
uniq = np.zeros((len(simg)),dtype=bool) #bitwise map
print('start calculation')
mt = np.zeros((n,n),dtype=float)
#s = input('Press ENTER to continue')
adj0 = []

#n = 10

srcmin = np.zeros((128), dtype = np.float32)
srcmax = np.zeros((128), dtype = np.float32)

for i in range(n):
    mt[i,i] = 1.
    ss[i,0] = idd[i]
    ss[i,1] = 1
    num = np.argsort(abs(arr[i]) ).astype(np.int32)
    nummin = num[0:128]#[128:]
    nummax = num[128:256]
    print(i, '->', simg[i])
    for ii in range(128):
      srcmax[ii] = arr[i,nummax[ii]]
      srcmin[ii] = arr[i,nummin[ii]]
    nn_max = math.sqrt(np.dot(srcmax,srcmax))
    nn_min = math.sqrt(np.dot(srcmin,srcmin))
    #print('norm = ', nn)
    if nn_max> 0 :
      #print('src befor \r\n', src) 
      srcmax = srcmax/nn_max
    else:
      print('srcmax norm is zero ', src)
      srcmax = np.zeros((len(arr[0])), dtype = np.float32)
    if nn_min> 0 :
      #print('src befor \r\n', src) 
      srcmin = srcmin/nn_min
    else:
      print('srcmax norm is zero ', src)
      srcmin = np.zeros((len(arr[0])), dtype = np.float32)
    #print('src after \r\n', src, '\r\n norm', np.dot(src,src))
              
    #print(i, ' ', norm[i])
    for j in range(i+1,n):
      tt_min = 0
      tt_max = 0
      #print(i, ' ', norm[i], ' ', norm[j])
      if (anorm[i] > 0.) & (anorm[j]> 0.) :
        #print(i, ' ', norm[i], ' ', norm[j])
        mt[i,j] = np.dot(arr[i],arr[j])/(anorm[i]*anorm[j])
        mt[j,i] = mt[i,j]
        
      if (mt[i,j] > threshold1 ) & (mt[i,j] < threshold):
        tt_min = IdAddCalc(srcmin, nummin, arr[j])
        tt_max = IdAddCalc(srcmax, nummax, arr[j])
        print('mt[', i,',',j,'] = ', mt[i,j],'; additinal: min = ', tt_min, '; max = ', tt_max,  '; (max+min)/2 =', (tt_max + tt_min)/2)
        if tt_max > tt_min:
          mt[i,j] = mt[i,j] + tt_min
      if mt[i,j] > threshold: 
        #print(idd[i], '~',idd[j], ' cos = ', mt[i,j], '; additinal ', tt_min)
        tmp = (arr[i]/anorm[i]-arr[j]/anorm[j])
        aa = np.max(tmp)-np.min(tmp)
        bb = np.var(tmp)
        print(idd[i], '~',idd[j], ' cos = ', mt[i,j], '; max-min ', aa, '; var/aa = ', bb/aa )
        adj0.append([i,j])
        ss[i,1] = ss[i,1]+1
        ss[i,ss[i,1]] = idd[j]
        uniq[i] = True
        uniq[j] = True
print('MT = \r\n',mt)

adj = np.array(adj0, dtype= int)
#np.sort(adj)
np.save('./images/adjacent.npy', adj)
np.save('./images/compare.npy', mt)

notrec = []
#to tests
#aa = ReadTextList('./images/imgnames.txt')
#print(aa)
print('images: ',len(uniq), ' not in eqvivqlents ',len(simg) - np.count_nonzero(uniq))
for p in range (len(uniq)):
  if uniq[p]==False:
    notrec.append(simg[p])
print('not recognased ', len(notrec), ' :\r\n',notrec)

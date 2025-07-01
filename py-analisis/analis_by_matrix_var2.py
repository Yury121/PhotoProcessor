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
  sec = np.zeros((64), dtype = np.float32)#np.zeros((len(src)), dtype = np.float32)
  for i in range(64): #(128):
    sec[i] = second[num[i]]
  nn = np.dot(sec, sec)
  if nn > 0 :
    return(np.dot(src[0:64],sec)/nn)
  return(0.)


# ------------------------------------------------------------
#     MAIN WORK VARIANT FLIP
# this variant calculate two matrix and generate output with max parameters
# ------------------------------------------------------------
# try calculate using face direction
print(' --------------------- MAIN WORK ---------------------------------')
ClearFolder('./images/')

# select data from database and create


db_file = "__vb.db3" #"vb.db3"
conn = sqlite3.connect(db_file)
cursor = conn.cursor()
sql = "SELECT ID, IDMAIN, NORM, COSIN, IMAGE, WIDTH, HEIGHT FROM FACESET WHERE COSIN NOTNULL  ORDER BY ID"
cursor.execute(sql)
rows = cursor.fetchall()
threshold = 0.7071 # define threshold
threshold1 = 0.64 #0.70645 #0.70 # define threshold

# Extract COSIN values
cosines = []
idd1 = []
idMain = []
norm =[]
simg = []
imscale =[]
n = 0
for row in rows:
    idd1.append(row[0])
    idMain.append(row[1])
    norm.append(row[2])
    cosines.append(np.frombuffer(row[3], dtype=np.float32))
    path = './images/'+str(row[0])+'.jpg'
    simg.append( str(row[0])+'.jpg')
    imscale.append(math.sqrt(row[5]/row[6]))
    #print(path)
    with open(path, 'wb') as file:
        file.write(row[4]);
    n = n+1
conn.close()
print('end read database')
#
print(imscale)
arr = np.array(cosines)
anorm = np.array(norm)
imscale = np.array(imscale)
idd = np.array(idd1, dtype=int)
StoreTextList(simg, './images/imgnames.txt')
np.save('./images/norma.npy', anorm)
np.save('./images/cosine.npy', arr)
np.save('./images/idd.npy', idd)
#print(idd)

ss = np.zeros((anorm.shape[0], anorm.shape[0]+2), dtype=int)
uniq = np.zeros((len(simg)),dtype=bool) #bitwise map
print('start calculation')
#mt_posit = np.zeros((n,n),dtype=float)
#mt_negat = np.zeros((n,n),dtype=float)

#n = 10

mt = np.zeros((n,n),dtype=float)
#s = input('Press ENTER to continue')
adj0 = []
sxx = np.zeros((n,2), dtype = float)
for i in range(n):
  sxx[i,0] = np.sum(arr[i])
  sxx[i,1] = math.sqrt(1. - sxx[i,0]*sxx[i,0]/256)
  
#print("Sxx = ",sxx)

size = 32

srcmin = np.zeros((size), dtype = np.float32)
srcmax = np.zeros((size), dtype = np.float32)
Rxy = np.zeros((n,n),dtype=float)

for i in range(n):
    mt[i,i] = 1.
    ss[i,0] = idd[i]
    ss[i,1] = 1
    print(i, '->', simg[i])
    Rxy[i,i] = 1.
              
    for j in range(i+1,n):
      if (anorm[i] > 0.) & (anorm[j]> 0.) :
        Rxy[i,j] = (np.dot(arr[i],arr[j]) - sxx[i,0]*sxx[j,0]/256)/(sxx[i,1]*sxx[j,1])
        Rxy[j,i] = Rxy[i,j]
        
        nmin = np.argsort(abs(arr[i]))
        srcmin = np.take(arr[i],nmin[0:size])#/anorm[i]
        nn1 = math.sqrt(np.sum(srcmin*srcmin))
        srcmin = srcmin/nn1
        srcmax = np.take(arr[i],nmin[256-size : 256])#/anorm[i]
        nn1 = math.sqrt(np.sum(srcmax*srcmax))
        srcmax = srcmax/nn1
        mt[i,j] = np.dot(arr[i],arr[j])#/(anorm[i]*anorm[j])

        #mt[i,j] = abs( (np.dot(arr[i],arr[j]) - np.sum(arr[i])*np.sum(arr[j]/256.)) /(anorm[i]*anorm[j]))
        #print(i, ' ', norm[i], ' ', norm[j])
        #mt_posit[i,j] = abs(np.dot(arr[i],arr[j])/(anorm[i]*anorm[j]))
        #mt_negat[i,j] = abs(np.dot(arr[i],np.flip(arr[j]))/(anorm[i]*anorm[j]))
#        if mt_posit[i,j] > mt_negat[i,j] :
#          mt[i,j] = mt_posit[i,j]
#        else :
#          mt[i,j] = mt_negat[i,j]
        
        mt[j,i] = mt[i,j]
      rel = 0        
      if (mt[i,j] > threshold1 ) : #  & (mt[i,j] < threshold): #
        aa = np.take(arr[j], nmin[256 - size:256])
        mm1 = np.dot(srcmax,aa)/math.sqrt(np.sum(aa*aa))
        aa = np.take(arr[j], nmin[0:size])
        mm2 = np.dot(srcmin, aa)/math.sqrt(np.sum(aa*aa))
        rel = (mm1 + mm2)/2 # mt[i,j])/2
        print(idd[i], '?',idd[j],'; mt[', i,',',j,'] = ', mt[i,j],'; Rxy[', i,',',j,'] = ', Rxy[i,j], '; max(', size,') = ', mm1, '; min(', size,') = ', mm2, 'rel =', rel)
      if (mt[i,j] > threshold) | (rel > 0.5) | (Rxy[i,j] > threshold):
        #tmp = (arr[i]/anorm[i]-arr[j]/anorm[j])
        #aa = np.max(tmp)-np.min(tmp)
        #bb = np.sum(tmp)
        print(idd[i], '~',idd[j], ' cos = ', mt[i,j],'; Rxy[', i,',',j,'] = ', Rxy[i,j], ';' )
        adj0.append([i,j])
        ss[i,1] = ss[i,1]+1
        ss[i,ss[i,1]] = idd[j]
        uniq[i] = True
        uniq[j] = True
#print('MT = \r\n',mt)
#print('Rxy = \r\n', Rxy)

adj = np.array(adj0, dtype= int)
#np.sort(adj)
np.save('./images/adjacent.npy', adj)
np.save('./images/compare.npy', mt)
np.save('./images/list.npy', ss)

notrec = []
#to tests
#aa = ReadTextList('./images/imgnames.txt')
#print(aa)
print('images: ',len(uniq), ' not in eqvivqlents ',len(simg) - np.count_nonzero(uniq))
for p in range (len(uniq)):
  if uniq[p]==False:
    notrec.append(simg[p])
print('not recognased ', len(notrec), ' :\r\n',notrec)

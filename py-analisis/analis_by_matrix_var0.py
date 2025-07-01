# This procedure read data from database
# Create directory "images" with person images and creates
# diferent arrays fo analisis
# Create adjacent matrix
# --- simple variant ---- only cosine metric


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

# ------------------------------------------------------------
#     MAIN WORK VARIANT FLIP
# this variant calculate two matrix and generate output with max parameters
# ------------------------------------------------------------
# try calculate using face direction
print(' --------------------- MAIN WORK ---------------------------------')
ClearFolder('./images/')

# select data from database and create
#in this variant 

db_file = "__vb.db3" #"vb.db3"
conn = sqlite3.connect(db_file)
cursor = conn.cursor()
sql = "SELECT ID, IDMAIN, NORM, COSIN, IMAGE, WIDTH, HEIGHT FROM FACESET WHERE COSIN NOTNULL  ORDER BY ID"
cursor.execute(sql)
rows = cursor.fetchall()
threshold = 0.7071 # define threshold
threshold1 = 0.64 #0.70645 #0.70 # define threshold

# ---- STEP 1 Extract COSIN values from database
cosines = []
idd1 = []
idMain = []
norm =[]
simg = []
imscale =[]
n = 0
for row in rows:
    k = math.sqrt(row[5]/row[6])
    if k < 0.75:
      print('id = ',row[0],' ignored by width/height')
      continue
    idd1.append(row[0])
    idMain.append(row[1])
    norm.append(row[2])
    cosines.append(np.frombuffer(row[3], dtype=np.float32))
    path = './images/'+str(row[0]).zfill(4)+'.jpg'
    simg.append( str(row[0]).zfill(4)+'.jpg')
    imscale.append(k)
    #print(path)
    with open(path, 'wb') as file:
        file.write(row[4]);
    n = n+1
    if (n == 500):     # !!!! debug less n
      break
conn.close()
print('end read database')

# --------------  STEP 2 Saving resalts and prepere data -----------------------------
#print(imscale)
# It is assumed that the data has been normalized
arr = np.array(cosines)
#anorm = np.array(norm)
imscale = np.array(imscale)
idd = np.array(idd1, dtype=int)
StoreTextList(simg, './images/imgnames.txt')
#np.save('./images/norma.npy', anorm)
np.save('./images/cosine.npy', arr)
np.save('./images/idd.npy', idd)
#print(idd)

#n = 10 #for debug 

ss = np.zeros((n, n+2), dtype=int) #array of idenifical images
uniq = np.zeros((len(simg)),dtype=bool) #bitwise map
print('start calculation')


mt = np.zeros((n,n),dtype=float)
#s = input('Press ENTER to continue')
adj0 = []
# ------- STEP 3 --------- Calculation  ----------------
for i in range(n):
    mt[i,i] = 1.
    ss[i,0] = i #idd[i]
    ss[i,1] = 1
    print(i, '->', simg[i])
    for j in range(i+1,n):
      mt[i,j] = abs(np.dot(arr[i],arr[j]))
      mt[j,i] = mt[i,j]
      if (mt[i,j] > threshold ) : #  & (mt[i,j] < threshold): #
        print(idd[i], '~',idd[j], ' cos = ', mt[i,j],';' )
        adj0.append([i,j])
        ss[i,1] = ss[i,1]+1
        ss[i,ss[i,1]] = j # idd[j]
        uniq[i] = True
        uniq[j] = True

# ------ STEP 4  ------- Saving result and printing -----------


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
tup = 0 # count tuples
for i in range(len(ss)):
  if ss[i,1] <= 1:
    continue
  sli = str(i) +'! ' + str(ss[i,1]) + ' images: [ ' + str(idd[ss[i,0]]).zfill(4) + '.jpg'
  p = 1
  tup = tup +1
  for k in range(len(ss[i])-2):
    if p > ss[i,1]:
      break
    if ss[i, k+2] != 0:
      sli = sli + ', ' + str(idd[ss[i, k+2]]).zfill(4) + '.jpg'
  print(sli, ' ]')

print('--- First step ', n-len(notrec), ' imeges in ', tup, ' tuples was found  ----\r\n')
      
    
print('not recognased ', len(notrec), ' :\r\n',notrec)

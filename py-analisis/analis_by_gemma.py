import sqlite3
import numpy as np
import os
#from PIL import Image

def SearchIntInRow(row, num):
  #row : row[0] -- first element, not used
  #      row[1] - max elements number
  #      row[2:row[1]] - elements for search
  left = 2
  right = row[1]+1
  oldmid = 1
  if row[1] > 1 :
    if row[left] == num :
      print("found ", num, " in ", row)
      return True
    while left < right :
      mid = (left + right) // 2
      if mid == oldmid :
        print("!!!!mid == oldmid, the same value", mid, "row = \r\n")
        return False
      if row[mid] == num :
        print("found ", num, " in ", row)
        return True
      if row[mid] < num :
        right = mid
      else:
        left = mid+1
  return False




def create_text_file(filename, row):
  """Создает файл с текстом.
  Args:
      filename (str): Название файла.
      row (np.array): строки для записи в файл.
  """
  sstr = '<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">'
  sstr = sstr + '<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>'
  sstr = sstr +'<title>Comparation</title></head><body>'
  sstr = sstr + '<img src=\"./' + str(row[0]) +'.jpg\" title =\"'+str(row[0]) + '.jpg\" border="1" /><br />'
  print('\r\n',sstr)
  #print(filename)
  try:
    with open(filename, 'w') as f:
      f.write(sstr)
      for p in range(2, row[1]+1) :
           f.write('<img src=\"./' + str(row[p]) +'.jpg\"  title =\"'+str(row[p]) + '.jpg\"  border="1" /><br />')
      f.write('</body></html>')
    print(f"Файл '{filename}' создан успешно.\r\n")
  except IOError:
      print(f"Ошибка при создании файла '{filename}'.\r\n")


db_file = "vb.db3"
conn = sqlite3.connect(db_file)
cursor = conn.cursor()
sql = "SELECT ID, IDMAIN, NORM, COSIN, IMAGE FROM FACESET WHERE COSIN NOTNULL  ORDER BY IDMAIN"
cursor.execute(sql)
rows = cursor.fetchall()

# Extract COSIN values
cosines = []
idd1 = []
idMain = []
norm =[]
n = 0
for row in rows:
    idd1.append(row[0])
    idMain.append(row[1])
    norm.append(row[2])
    cosines.append(np.frombuffer(row[3], dtype=np.float32))
    path = './images/'+str(row[0])+'.jpg'
 #   sf.append[path]
    print(path)
    with open(path, 'wb') as file:
        file.write(row[4]);
    n = n+1
conn.close()
print('end read database')
print(idd1)
arr = np.array(cosines)
anorm = np.array(norm)
idd = np.array(idd1, dtype=int)
np.save('./images/norma.npy', anorm)
np.save('./images/cosine.npy', arr)
np.save('./images/idd.npy', idd)
print(idd)

ss = np.zeros((anorm.shape[0], anorm.shape[0]+2), dtype=int)
print('start calculation')
mt = np.zeros((n,n),dtype=float)
s = input('Press ENTER to continue')
#n = 10
for i in range(n):
    mt[i,i] = 1.
    ss[i,0] = idd[i]
    ss[i,1] = 1
    #print(i, ' ', norm[i])
    for j in range(i+1,n):
      #print(i, ' ', norm[i], ' ', norm[j])
      if (anorm[i] > 0.) & (anorm[j]> 0.) :
        #print(i, ' ', norm[i], ' ', norm[j])
        mt[i,j] = np.dot(arr[i],arr[j])/(anorm[i]*anorm[j])
        mt[j,i] = mt[i,j]
      if abs(mt[i,j]) > 0.7:
        print(idd[i], '~',idd[j])
        ss[i,1] = ss[i,1]+1
        ss[i,ss[i,1]] = idd[j]
"""        
      # search in prev row
      for p in range(i) :
        if SearchIntInRow(ss[p], id[j]) :
          if abs(mt[p, j]) > 0.7 :
            ss[i,1] = ss[i,1]+1
            ss[i,ss[i,1]] = id[j]
          break
"""          
            
                          
    #anorm[i] = 0.
print('MT = \r\n',mt)
np.save('./images/compare.npy', mt)

#print(ss)
#s = input('Press ENTER to continue')
k = 0;

for i in range (n):
    k = ss[i,1]
    print(k)
    if k > 1  :
        k = 1 + ss[i,1]
        path = './images/'+str(ss[i,0])+'.html'
        print(ss[i, 0: k+2])
        create_text_file(path, ss[i]);
        #print('\r\n',path, '  ind = ', ss[i, 1])
        for p in range(2, k) :
            path = './images/'+str(ss[i,p])+'.jpg'
            #print(path, "  p = ", p, '  ind = ', ss[i, 1], '   img = ', ss[i,p] )
        #s = input('Press ENTER to continue')
 
print(ss)


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
      print("found ", num, " in ", row[0:row[1]+1])
      return True
    while left < right :
      mid = (left + right) // 2
      if mid == oldmid :
        print("!!!!mid == oldmid, the same value", mid, "row = \r\n")
        return False
      if row[mid] == num :
        print("found ", num, " in ", row[0:row[1]+1])
        return True
      if row[mid] < num :
        right = mid
      else:
        left = mid+1
  return False




def create_text_file(filename, row, idd):
  """Создает файл с текстом.
  Args:
      filename (str): Название файла.
      row (np.array): строки для записи в файл.
  """
  sstr = '<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">'
  sstr = sstr + '<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>'
  sstr = sstr +'<title>Comparation</title></head><body>'
  sstr = sstr + '<img src=\"./' + str(idd[row[0]]) +'.jpg\" title =\"'+str(idd[row[0]]) + '.jpg\" border="1" /><br />'
  print('\r\n',sstr)
  #print(filename)
  try:
    with open(filename, 'w') as f:
      f.write(sstr)
      for p in range(2, row[1]+1) :
           f.write('<img src=\"./' + str(idd[row[p]]) +'.jpg\"  title =\"'+str(idd[row[p]]) + '.jpg\"  border="1" /><br />')
      f.write('</body></html>')
    print(f"Файл '{filename}' создан успешно.\r\n")
  except IOError:
      print(f"Ошибка при создании файла '{filename}'.\r\n")

"""
db_file = "vb.db3"
conn = sqlite3.connect(db_file)
cursor = conn.cursor()
sql = "SELECT ID, IDMAIN, NORM, COSIN, IMAGE FROM FACESET WHERE COSIN NOTNULL  ORDER BY IDMAIN"
cursor.execute(sql)
rows = cursor.fetchall()

# Extract COSIN values
cosines = []
id = []
idMain = []
norm =[]
n = 0
for row in rows:
    id.append(row[0])
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
arr = np.array(cosines)
anorm = np.array(norm)
np.save('./images/norma.np', anorm)
np.save('./images/cosine.np', arr)
"""

#arr = np.load('./images/cosine.np')
#anorm = np.load('./images/norma.np')



print('start calculation')

mt = np.load('./images/compare.npy')
aa = mt.shape
ss = np.zeros((aa[0], aa[1]+2), dtype=int)
idd = np.load('./images/idd.npy')
print('MT = \r\n',mt)

#s = input('Press ENTER to continue')
n = aa[0]
n = 100
for i in range(n):
    mt[i,i] = 1.
    ss[i,0] = i
    ss[i,1] = 1
    #print(i, ' ', norm[i])
    for j in range(i+1,n):
      b = 0

      # search in prev row
      for p in range(i) :
        if SearchIntInRow(ss[p], j) :
          b = 1
          break;
 #bred       
        if abs(mt[p, ss[p, j]) > 0.7 :
            ss[i,1] = ss[i,1]+1
            ss[i,ss[p,1]] = j
            print('added ',idd[p], '~',idd[j])
            break
          
        
        
        
      if abs(mt[i,j]) > 0.7:
        # check i  befor
      if SearchIntInRow(ss[p], j) :
            
            

            
            ss[i,1] = ss[i,1]+1
            ss[i,ss[p,1]] = j
            print('added ',idd[p], '~',idd[j])
            break





        print(idd[i], '~',idd[j])
        ss[i,1] = ss[i,1]+1
        ss[i,ss[i,1]] = j

            
                          
    #anorm[i] = 0.
print('MT = \r\n',mt)


#print(ss)
#s = input('Press ENTER to continue')
k = 0;

for i in range (n):
    k = ss[i,1]
    print(k)
    if k > 1  :
        k = 1 + ss[i,1]
        path = './images/'+str(idd[ss[i,0]])+'.html'
        print(ss[i, 0: k+2])
        create_text_file(path, ss[i], idd);
        #print('\r\n',path, '  ind = ', ss[i, 1])
        for p in range(2, k) :
            path = './images/'+str(idd[ss[i,p]])+'.jpg'
            #print(path, "  p = ", p, '  ind = ', ss[i, 1], '   img = ', ss[i,p] )
        #s = input('Press ENTER to continue')
 
print(ss)


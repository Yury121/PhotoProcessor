# diferent arrays fo analisis
# Create adjacent matrix

import sqlite3
import numpy as np
import os

# ------------------------------------------------------------
#     MAIN WORK
# ------------------------------------------------------------

# select data from database and create


db_file = "__vb.db3" #"vb.db3"
conn = sqlite3.connect(db_file)
cursor = conn.cursor()
sql = "SELECT ID, NORM, COSIN FROM FACESET WHERE COSIN NOT NULL  ORDER BY ID"
cursor.execute(sql)
rows = cursor.fetchall()
threshold = 0.7 # define threshold

# Extract COSIN values
cosines = []
idd1 = []
idMain = []
norm =[]
simg = []
n = 0
for row in rows:
  idd1.append(row[0])
  norm.append(row[1])
  cosines.append(np.frombuffer(row[2], dtype=np.float32))
  n = n+1

arr = np.array(cosines)
anorm = np.array(norm)
idd = np.array(idd1, dtype=int)

cursor.close()
conn.close()
print(db_file)


conn = sqlite3.connect(db_file)
cursor = conn.cursor()

for i in range(len(idd)):
  if anorm[i] > 0 :
    tmp = np.divide(arr[i],anorm[i], dtype = np.float64).flatten()
    #tmp = arr[i]/anorm[i]
    nn = np.dot(tmp, tmp)
    print(idd1[i], ') ',anorm[i], ' -> ', nn)
    cursor.execute("UPDATE FACESET SET NORM = ?, COSIN = ? WHERE ID = ?", (nn, bytes(tmp.astype(np.float32)), idd1[i]))
    print('updated ', idd[i])
    
  else :
    print('norm = ', norm, ' kof -> ', arr[i])

# cursor.execute("UPDATE FACESET SET NORM=?, COSIN=? WHERE ID=?", (norm, cosins[i], idd[i])

conn.commit()
cursor.close()
conn.close()
print('end read database')
"""
except sqlite3.Error as e:
  print(f"Ошибка при работе с базой данных: {e}")
finally:
  if conn:
    cursor.close()
    conn.close()
"""

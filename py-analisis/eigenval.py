import numpy as np
import random as rr
import cmath
import sqlite3

n = 9
np.random.seed()
a = np.zeros((n,n),dtype=float)
for i in range(n):
    for j in range(n):
        a[i,j] = np.random.rand()
print("Matrix A =",a,"\n")
eigenvals, eigenvecs = np.linalg.eig(a)
print("eigenvals = ",eigenvals, "\n", abs(eigenvals), "\n")
print("eigenvecs = ",eigenvecs, "\n", abs(eigenvecs),"\n")
print("\n\nDatabase test \n")

# Establish a connection to the SQLite database (creates it if it doesn't exist)
conn = sqlite3.connect('database1.db')  # Replace with your desired database name

# Create the "Names" table (create if it doesn't exist)
cursor = conn.cursor()
cursor.execute('''CREATE TABLE IF NOT EXISTS
                  Names (Id INTEGER PRIMARY KEY, name TEXT);''')

# Create the "Values" table 
cursor.execute('''CREATE TABLE IF NOT EXISTS
Values1 (IdMain INTEGER,value REAL);''')

# Insert sample data into the "Names" table
for i in range(5):
    k = np.random.randint(1024) 
    cursor.execute("INSERT INTO Names (name) VALUES (?)", ("Name_" + str(k+1),))  # Dynamically generate names 

conn.commit()

# Insert random values into the "Values" table based on the existing "Names" table
for i in range(5):
    cursor.execute("INSERT INTO Values1 (IdMain, value) VALUES (?, ?)", (cursor.lastrowid, i + 1)) 
conn.commit() 


cursor.close()
conn.close()

conn = sqlite3.connect('database1.db')
cursor = conn.cursor() 
cursor.execute('''SELECT  Names.Id AS Name_ID, name,
Values1.value FROM Names JOIN Values1 ON Names.Id = Values1.IdMain ORDER BY name ;
''') # SQL query for the combined data
results = cursor.fetchall() 

for row in results:
    print("IdMain: ", row[0], " name: ", row[1], " value: ", row[2])
    #print(results)

cursor.execute('''SELECT name, n.id, v.value 
                 FROM Names AS n
                 JOIN Values1 AS v ON n.id = v.idmain  ORDER BY name''')

# Вывод результатов
print("\n\n\n",cursor.fetchall())


conn.close()


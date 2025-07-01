import numpy as np
import os

# TOOLS
# ------------------------------------------------------------
def ReadTextList(fname):
  with open(fname, "r") as file:
    loaded_list = [line.strip() for line in file]  
    #print(loaded_list)
    return loaded_list
  return []
# ------------------------------------------------------------
# row - set of indexes
# idd - list of strings
def create_text_file(filename, row, idd):
  """Создает файл с текстом.
  Args:
      filename (str): Название файла.
      row (np.array): строки для записи в файл.
  """
  if os.path.isfile(filename):
      os.remove(filename)
  sstr = '<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">'
  sstr = sstr + '<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>'
  sstr = sstr +'<title>Comparation</title></head><body>'
  #sstr = sstr + '<img src=\".' + idd[row[0]] +'\" title =\"'+str(idd[row[0]]) + '.jpg\" border="1" /><br />'
  try:
    with open(filename, 'w') as f:
      f.write(sstr)
      for p in row :
          name = idd[p]
          f.write('<img src=\"./' + name +'\"  title =\"'+ name + '\"  border="1" />') #<br />
      f.write('</body></html>')
    #print(f"Файл '{filename}' создан успешно.\r\n")
  except IOError:
      print(f"Ошибка при создании файла '{filename}'.\r\n")

# ------------------------------------------------------------
def AddToRow(test, item): # test list of rows item is pair item[0] - exist, item[1] - new
    k = 0
    for p in range(len(test)) :
        k = k +1
        if item[0] in test[p] :
            test[p].append(item[1])
            test[p].sort()
            #print(item, ' was added to row ', p, 'test[p]=', test[p])
            break
# ------------------------------------------------------------

img = ReadTextList('./images/imgnames.txt')
adj = np.load('./images/adjacent.npy')
mt = np.load('./images/compare.npy')
stored = np.load('./images/list.npy')
print('img size = ', len(img), '; adj size = ', len(adj))

test = [] #list eqvivalence objects
uniq = np.zeros((len(img)),dtype=bool) #bitwise map

s = 0 # count pair
count = 0 # count rows
same =[]
ind = 0
for pp in adj :
    item = [int(pp[0]),int(pp[1])]
    #print(' -----  procesed ind ', ind,' item = ', pp, '---------')
    ind = ind +1
    ladd = []
    if (uniq[item[0]] == True) & (uniq[item[1]] == True) :
        #print('!!! step ', s+1, '; items exists: ', item)
        kk = 0
        ii = -1
        jj = -1
        # we need check to this items in the same existing class
        for i in range(len(test)):
            if kk == 2 : 
                break
            if item[0] in test[i]:
                #print(item[0], ' in test[', i, '] = ', test[i])
                kk = kk + 1
                ii = i
            if item[1] in test[i]:
                #print(item[1], ' in test[', i, '] = ', test[i])
                kk = kk + 1
                jj = i
        if ii != jj : #items from different classes
            tmp = [ii, jj]
            if jj < ii : # must to rotate indexes 
                tmp = [jj, ii]
            if  tmp not in same :
                same.append(tmp) #arrays must be concatinated
                print(tmp, ' must to concatinate; object size =', len(test), ' tuple ->', tmp)
#        break #debug

    if uniq[item[0]] == False : #test new item
        if uniq[item[1]] == True :
          #item 0 must be added in some row from test
          #print('false, true', item)
          ladd = [item[1], item[0]]
          AddToRow(test, ladd)
        else :
          #new row
          ladd = [item[0], item[1]]
          test.append(ladd)
          #print('Append tuple ', len(test), ' tuple ', ladd)
          count = count +1
    else:
      ladd = [item[0], item[1]]
      if uniq[item[1]] == False :
        #print('true, false', item)
        AddToRow(test, ladd)
    #item processed
    uniq[item[0]] = True
    uniq[item[1]] = True
    s = s+1     
#    if s > 25 : #debug
#        break   #debug

print ('processed ', s, ' pairs and ', count, ' rows calculated')
print('tuples: ', len(test), '\\r\nTEST =')
for i in range(len(test)):
  print(test[i])
#print(len(test), '\r\n TEST = ', test, '\r\n-----------')
same.sort(reverse = True)
print('Same = \r\n',same, '\r\n----------')
conc =[]
cuni = np.zeros((len(img)),dtype=int) #bitwise map
for p in range(len(test)):
    for i in test[p]:
#        print(i)
        cuni[i] = cuni[i]+1
print('From ',len(cuni), ' images ',np.count_nonzero(cuni), ' was idintificated')
for i in range(len(img)):
  if (cuni[i] == 0):
    conc.append(i)
    

'''    
k = 0
for p in test:
    name =  './images/'+str(k) + '.html'
    #create_text_file(name, p, img)
    k = k + 1
print (str(k)+' files was creted')
'''  


mask = np.zeros((len(test)),dtype=np.int32) - 1
for pp in same:
  if mask[pp[1]] == -1:
    test[pp[0]] = test[pp[0]] + test[pp[1]]
    mask[pp[1]] = pp[0]
  else:
    test[pp[0]] = test[pp[0]] + test[mask[pp[1]]]
    mask[pp[1]] = pp[0]

print(test)
mout = []
for pp in range(len(test)):
  if mask[pp] == -1:
    tmp = np.unique(np.array(test[pp], dtype = int)).tolist()
    mout.append(tmp)
                                     
print('Mout has ', len(mout), ' elements. MOUT = ')
k = 0
for p in mout:
  print(k,'->',img[p[0]], ' (',len(p),') ', p, '\r\n', np.take(img, p), '\r\n')
  name =  './images/'+str(k).zfill(3) + '.html'
  create_text_file(name, p, img)
  k = k + 1
print (str(k)+' files was creted')

create_text_file('./images/not_ident.html', conc, img)                 
                

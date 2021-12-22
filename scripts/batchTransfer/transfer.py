import json
import sys
import os
import time

file1 = open('transfer-input.txt', 'r')
Lines = file1.readlines()


outputFileName = 'transfer-output.txt'

for line in Lines:
  print(line)
  jsoned = json.loads(line)
  command = "cleos -u " + sys.argv[1] + " transfer " + jsoned["fromAccount"] +  " " +  jsoned["toAccount"] + " \"" + jsoned["amount"] + "\" \"\" -p " + jsoned["fromAccount"]
  print(command)
  with open(outputFileName, 'a') as the_file:
    the_file.write('\n' + command + '\n')
  os.system(command + ' >> ' + outputFileName + ' 2>&1')
  time.sleep(1)
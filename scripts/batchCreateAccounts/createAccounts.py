import json
import sys
import os
import time

file1 = open('createAccounts-input.txt', 'r')
Lines = file1.readlines()

outputFileName = 'createAccounts-output.txt'

for line in Lines:
  print(line)
  jsoned = json.loads(line)
  command = "cleos -u " + sys.argv[1] + " system newaccount " + jsoned["creator"] +  " " +  jsoned["createAccount"] + " \"" + jsoned["ownerPublicKey"] + "\" \"" + jsoned["activePublicKey"] + "\" --stake-net \"0.0100 SYS\" --stake-cpu \"0.0100 SYS\" --buy-ram-kbytes 4 -p " + jsoned["creator"]
  print(command)
  with open(outputFileName, 'a') as the_file:
    the_file.write('\n' + command + '\n')
  os.system(command + ' >> ' + outputFileName + ' 2>&1')
  time.sleep(1)

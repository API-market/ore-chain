import json
import sys
import os
import time

file1 = open('vesting-remove-input.txt', 'r')
Lines = file1.readlines()

outputFileName = 'vesting-remove-output.txt'

for line in Lines:
  print(line)
  jsoned = json.loads(line)
  command = "cleos -u " + sys.argv[1] + " push action eosio.token rmvestacct '[\"" + jsoned["account"] +  "\", " +  str(jsoned["index"]) + " ]' -p eosio.token"
  with open(outputFileName, 'a') as the_file:
    the_file.write('\n' + command + '\n')
  os.system(command + ' >> ' + outputFileName + ' 2>&1')
  time.sleep(1)
import json
import sys
import os
import time

file1 = open('vesting-input.txt', 'r')
Lines = file1.readlines()

outputFileName = 'vesting-output.txt'

for line in Lines:
  print(line)
  jsoned = json.loads(line)
  command = "cleos -u " + sys.argv[1] + " push action eosio.token addvestacct '[\"" + jsoned["account"] +  "\", \"" +  jsoned["quantity"] + "\", \"" + jsoned["start"]+ "\", \"" + jsoned["end"] + "\"]' -p eosio.token"
  print(command)
  with open(outputFileName, 'a') as the_file:
    the_file.write('\n' + command + '\n')
  os.system(command + ' >> ' + outputFileName + ' 2>&1')
  time.sleep(1)
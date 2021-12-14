import json
import sys
import os
import time

file1 = open('vesting-input.txt', 'r')
Lines = file1.readlines()

#url=" -u https://ore.openrights.exchange "
url=" "

outputFileName = 'vesting-output.txt'

for line in Lines:
  print(line)
  jsoned = json.loads(line)
  command = "cleos" + url + "push action eosio.token addvestacct '[\"" + jsoned["account"] +  "\", \"" +  jsoned["quantity"] + "\", \"" + jsoned["start"]+ "\", \"" + jsoned["end"] + "\"]' -p eosio.token"
  with open(outputFileName, 'a') as the_file:
    the_file.write('\n' + command + '\n')
  os.system(command + ' >> ' + outputFileName + ' 2>&1')
  time.sleep(1)
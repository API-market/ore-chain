import json
import sys
import os
import time

file1 = open('createAccounts-input.txt', 'r')
Lines = file1.readlines()

url=""

newAccountOwnerPublicKey = "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn"
newAccountActivePublicKey = "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn"

outputFileName = 'createAccounts-output.txt'

for line in Lines:
  print(line)
  jsoned = json.loads(line)
  command = "cleos " + url + "system newaccount " + jsoned["creator"] +  " " +  jsoned["createAccount"] + " \"" + newAccountOwnerPublicKey + "\" \"" + newAccountActivePublicKey + "\" --stake-net \"0.0100 SYS\" --stake-cpu \"0.0100 SYS\" --buy-ram-kbytes 4 -p " + jsoned["creator"]
  with open(outputFileName, 'a') as the_file:
    the_file.write('\n' + command + '\n')
  os.system(command + ' >> ' + outputFileName + ' 2>&1')
  time.sleep(1)

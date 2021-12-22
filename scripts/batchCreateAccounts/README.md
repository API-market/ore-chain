# Batch Account Create

## Prerequisites to install
- python 3
- cleos
- keosd

## Preperation
- Unlock the eosio wallet that contains the privateKey of active permission of funding account (or fromAccount(s))

## Usage
Create Accounts:

    - Populate createAccounts-input.txt (see example)
    - Provide command line: url, newAccountOwnerPublicKey, newAccountActivePublicKey
    - python3 createAccounts.py "https://ore.openrights.exchange" EOS5... EOS5..


## Installation

    npm install
    npm run compile

## Testing on fork

    npm run test -- --network hardhat

## Usage

Distribute tokens:

    npm run hardhat -- --network target distribute --filename accounts.json

## Steps to run monthly distribution

- populate key for sender account - ORE Foundation (0xA2910d9b2Bd0Bdc1DfCCDDAd532680b167Df1894)
    const PRIVATE_KEY = '...'

- Build ORE Contract artifacts (using custom config file)

    ```npm run compile-ethmain```
- populate accounts.json with amounts to transfer
    - copy from accounts-mainnet-strategic-round-2-2021-11-13.json 
    - update changes from spreadsheet
    - copy contents of new accounts.json to spreadsheet to compare amounts about to be send match exactly with spreadsheet rows

- update GAS_PRICE to current gas price plus a buffer - e.g. if current price is 110 Gwei, set to 130 (130E9)
    const GAS_PRICE = 130E9
- run transfer script

    ```npm run hardhat -- --config hardhat.mainnet.config.js --network target distribute --filename accounts.json```
- confirm each transfer in block explorer - etherscan.io 
- copy output from terminal to google docs log 
# ORE Vesting Scripts

## Deploy Vesting
This script requires ore-token contract to be built. For that under ore-token/ directory run:
```shell
## Requires eosio.cdt v1.6
./build.sh
```
Then to deploy vesting enabled eosio.token contract under vesting directory:
```shell
## Requires an unlocked wallet that containts active@eosio.token key
./deployVesting.sh
```
## Initialize vesting schedules
- Create `vesting-input.txt` in the format of example file.
- Run:
```shell
## Requires an unlocked wallet that containts active@eosio.token key
python3 addvesting.py
```

## Calculate Vesting
- Run:
```shell
ts-node calculateVesting.ts accountname "https://ore.openrights.exchange"
```

## Get Vesting Balances
This script gathers all vesting accounts and outputs two csv files:

- total_csv 
    - list of all accounts and total tokens claimed and locked  
- detailed_vesting.csv
    - list of all vesting schedules: account, claimed, locked, start time, and end time

### Initial Setup
```
virtualenv -p `which python3` ~/envs/vesting
source ~/envs/vesting/bin/activate
pip install -r requirements.txt
```

### Run script
```
source ~/envs/vesting/bin/activate
python getVestingBalances.py
```
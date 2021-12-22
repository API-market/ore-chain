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
python3 addvesting.py "https://ore.openrights.exchange"
```

## Calculate Vesting
- Run:
```shell
ts-node calculateVesting.ts accountname "https://ore.openrights.exchange"
```
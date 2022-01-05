# Build the contracts

## Prerequisites

- eosio.cdt 1.6.x

## Manually

```bash
# assumes you are in the ore-chain/ore-contracts directory
mkdir build && cd build
cmake .. 
make -j4
```
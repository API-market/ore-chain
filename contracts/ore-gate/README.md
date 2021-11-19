# ORE Gate Contract

The ORE Gate contract (ore2eth.ore) governs the rate at which tokens can be sent across a bridge to Ethereum enforcing date sensitive locking (queue).

## How to test

```
./test.sh

# to see queue table
cleos get table ore2eth.ore ore2eth.ore queue
```
## Features
- Default queue schedule is 24 months period starting from` 13 October 2021`.
- **(Only Admin)** If account is added to priority list by ``addpriority()`` queue schedule is calculated based on 12 months period starting from the same date.
```
cleos push action ore2eth.ore addpriority '["testacount11"]' -p ore2eth.ore
```
- ``queue()`` is automatically done by ORE Token transfer to ``ore2eth.ore`` (gate) contract.
```
cleos transfer testacount11 ore2eth.ore "100.0000 ORE" ""
```
- To send ORE Tokens to Ethereum Chain, ``sendtoeth()`` needs to be called manually by specifying ``quantity`` and ``destination address`` which is the Ethereum address you want to receive ORE Tokens.
```
cleos push action ore2eth.ore sendtoeth '["testacount11", "10.0000 ORE", "ETHADDR..."]' -p testacount11
```
- If the specified quantity is more than the unlocked amount (calculated by chain state's epoch time) ``sendtoeth()`` will throw with the error message specifies the maximum allowed amount for that time.
```
assertion failure with message: Quantity can't be more than unlocked amount: 5.1709 ORE
```
- To reclaim queued ORE Tokens call ``reclaim()`` by specifying quantity. __Beware:__ If you already sent some ORE Tokens to Ethereum you can only reclaim with ``quantity`` that exceeding the amount needs to be locked for the sent amount.
```
cleos push action ore2eth.ore reclaim '["testacount11", "90.0000 ORE"]' -p testacount11
```

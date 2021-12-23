# Get Token Balances
This script uses DFuse GraphQL endpoint to retrieve all account balances in descending order for a given symbol.

## Preperation
- Create `.env` file using example.
## Usage
    # Uses eosio.token contract by default
    ts-node tokenBalances.ts ORE

    ## OR to specify a contract
    ts-node tokenBalances.ts SYM tokencontract

## Sample Output
{"count":1,"account":"account11114","balance":"10000000.6349 ORE"}
{"count":2,"account":"account11112","balance":"7000000.0000 ORE"}
{"count":3,"account":"account11115","balance":"222789.0000 ORE"}
{"count":4,"account":"account11121","balance":"33333.0000 ORE"}
{"count":5,"account":"account11133","balance":"10.0000 ORE"}
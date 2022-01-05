# ORE Specific Changes

## Eosio.System 
Adds features to account creation incentivization

* **upgraderam()** action added. This can only be run via the eosio active permission. This action will be called when a threshold of chain ram usage is triggered 
    1. **max_ram_size** will be increased by 20%.
    2. **setprice()** action will be called from system.ore (reduces cost of ORE accounts)
    

 * **setram()** action is modified. "ram may only be increased" assertion is removed temporarily. Currently max_ram_size is the default value (64 GiB) and we will reduce it to our initial ram capacity according to our token economics planning (10 GiB).
 
 * **setabi()** action is modified. Now it requires the active permission of system.ore in addition to the account sets contract.
 * **claim()** action is added. Allows ORE stakers to claim rewards from u.pay reward pool
    1. Similar to claimrewards() action for producers.
    2. Callable once in every month.

## Eosio.Token
In addition to standard eosio.token contract, provides stake & unstake actions, allowing accounts to stake ORE token to create new accounts and get rewards from inflation. Additionally there are vesting tables and actions to allow certain accounts to have tokens that unlock over a period of time. 

* **stake()** & **unstake()** actions added. Only callable by system.ore permission.
    1. Called when creating new accounts or changing accounts’ tier (resources).
* Staker will be rewarded by calling **claim()** action which will distribute the staking rewards from u.pay account.

* **chngstaker()** action added. Only callable by current staker. A shortcut for a staker to transfer the stakership of an account.

* **addvestacct** & **rmvestacct** actions added. Only callable by `eosio.token` these actions will add or remove accounts from the `vesting` table. Accounts in the `vesting` table can only transfer locked tokens in a cadence of `( (current_time - start_time) / (end_time - start_time) ) * total_locked_tokens`.
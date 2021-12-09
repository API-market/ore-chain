# ORE Specific Changes

## Eosio.Token
In addition to standard eosio.token contract, provides stake & unstake actions, allowing accounts to stake ORE token to create new accounts and get rewards from inflation. Additionally there are vesting tables and actions to allow certain accounts to have tokens that unlock over a period of time. 

* **stake()** & **unstake()** actions added. Only callable by system.ore permission.
    1. Called when creating new accounts or changing accounts’ tier (resources).
* Staker will be rewarded by calling **claim()** action which will distribute the staking rewards from u.pay account.

* **chngstaker()** action added. Only callable by current staker. A shortcut for a staker to transfer the stakership of an account.

* **addvestacct** & **rmvestacct** actions added. Only callable by `eosio.token` these actions will add or remove accounts from the `vesting` table. Accounts in the `vesting` table can only transfer locked tokens in a cadence of `( (current_time - start_time) / (end_time - start_time) ) * total_locked_tokens`.


cleos system newaccount eosio ptokens EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-net "10.0000 SYS" --stake-cpu "10.0000 SYS" --buy-ram-kbytes 5192
cleos system newaccount eosio xeth.ptokens EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-net "10.0000 SYS" --stake-cpu "10.0000 SYS" --buy-ram-kbytes 5192
cleos system newaccount eosio ore2eth.ore EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-net "100.0000 SYS" --stake-cpu "100.0000 SYS" --buy-ram-kbytes 10192
cleos system newaccount eosio testacount11 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV --stake-net "10.0000 SYS" --stake-cpu "10.0000 SYS" --buy-ram-kbytes 5192

sleep 2s

cleos set contract ore2eth.ore ./ oregate.wasm oregate.abi -p ore2eth.ore -p system.ore
cleos set account permission ore2eth.ore active '{"threshold": 1,"keys": [{"key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV","weight": 1}],"accounts": [{"permission":{"actor":"ore2eth.ore","permission":"eosio.code"},"weight":1}]}' owner -p ore2eth.ore@owner

sleep 2s

sleep 2s
cleos transfer eosio testacount11 "100.0000 ORE" ""
cleos transfer testacount11 ore2eth.ore "100.0000 ORE" ""

sleep 2s
cleos push action ore2eth.ore sendtoeth '["testacount11", "10.0000 ORE", "5GJS5..."]' -p testacount11

sleep 2s
cleos push action ore2eth.ore addpriority '["testacount11"]' -p ore2eth.ore
# cleos push action ore2eth.ore rmvpriority '["eosio"]' -p ore2eth.ore

cleos get table ore2eth.ore ore2eth.ore queue
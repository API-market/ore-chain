cleos="$1"
ore_contracts="$2"

$cleos set contract  system.ore $ore_contracts/build/ore.system/ ore.system.wasm ore.system.abi
sleep 2s
$cleos set account permission lock.ore active '{"threshold": 1,"keys": [{"key": "EOS4tdVaZUkQe7dmqRz7gqX85z3nDbAMnqKAMXcmrDKG31RYE1Mb1","weight": 1}],"accounts": [{"permission":{"actor":"eosio","permission":"eosio.code"},"weight":1}, {"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner -p lock.ore@owner
sleep 2s
$cleos set account permission system.ore active '{"threshold": 1,"keys": [{"key": "EOS6H2tjbrS6zm8d3tX6yeHorEoihP23Ny9c9wFRHGfJp4vtD42rn","weight": 1}],"accounts": [{"permission":{"actor":"eosio","permission":"eosio.code"},"weight":1}, {"permission":{"actor":"system.ore","permission":"eosio.code"},"weight":1}]}' owner -p system.ore@owner
sleep 2s
$cleos push action system.ore migrate '[]' -p system.ore
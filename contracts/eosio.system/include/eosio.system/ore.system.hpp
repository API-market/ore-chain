#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

namespace oresystem {
   //  struct [[eosio::table("pricetable"), eosio::contract("system.ore")]] prices {
   //    eosio::name pricename;
   //    eosio::asset price;
   //    uint64_t primary_key() const { return pricename.value; }
   //    EOSLIB_SERIALIZE(prices, (pricename)(price))
   // };
   // typedef eosio::multi_index<"pricetable"_n, prices> _pricetable;

   TABLE prices
   {
      eosio::name pricename;
      eosio::asset price;
      uint64_t primary_key() const { return pricename.value; }
      EOSLIB_SERIALIZE(prices, (pricename)(price))
   };
   typedef eosio::multi_index<"pricetable"_n, prices> pricetable;
}

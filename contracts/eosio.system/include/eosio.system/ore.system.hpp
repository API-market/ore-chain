#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

namespace oresystem {
   TABLE prices
   {
      eosio::name pricename;
      eosio::asset price;
      uint64_t primary_key() const { return pricename.value; }
      EOSLIB_SERIALIZE(prices, (pricename)(price))
   };
   typedef eosio::multi_index<"pricetable"_n, prices> pricetable;
}

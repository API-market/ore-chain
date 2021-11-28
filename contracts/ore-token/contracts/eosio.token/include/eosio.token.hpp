#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>

#include <string>

namespace eosiosystem {
   class system_contract;
}

namespace eosio {

   using std::string;
   /**
    * eosio.token contract defines the structures and actions that allow users to create, issue, and manage
    * tokens on eosio based blockchains.
    */
   class [[eosio::contract("eosio.token")]] token : public contract {
      public:
         using contract::contract;

//*** Added GBT

          static constexpr symbol ore_symbol     = symbol(symbol_code("ORE"), 4);
          static constexpr name ore_lock{"lock.ore"_n};
          static constexpr name ore_system{"system.ore"_n};

           struct [[eosio::table]] reserve {
              asset          staked;
              time_point     last_claimed;
              
              uint64_t primary_key()const { return staked.symbol.code().raw(); }
           };

           struct [[eosio::table]] staking_stats {
              asset    ore_staked;

              uint64_t primary_key()const { return ore_staked.symbol.code().raw(); }
           };

           struct [[eosio::table]] stake_info {
              asset    amount;
              name     staker;

              uint64_t primary_key()const { return amount.symbol.code().raw(); }
           };

         struct vesting_info {
            eosio::asset   claimed;
            eosio::asset   locked;
            time_point     start_time;
            time_point     end_time;
         };

         /*
         * 
         */
         struct [[eosio::table, eosio::contract("eosio.token")]] vesting_acct_info {
            eosio::name                account;
            std::vector<vesting_info>  vesting;
            eosio::asset               total_claimed;
            eosio::asset               total_locked;

            uint64_t primary_key() const { return account.value; }
         };

          typedef eosio::multi_index< "reserves"_n, reserve > reserves;
          typedef eosio::multi_index< "stakestats"_n, staking_stats > stakestats;
          typedef eosio::multi_index< "stakeinfo"_n, stake_info > stakeinfo;
          typedef eosio::multi_index< "vesting"_n, vesting_acct_info> vestinginfo;
//***

         /**
          * Allows `issuer` account to create a token in supply of `maximum_supply`. If validation is successful a new entry in statstable for token symbol scope gets created.
          *
          * @param issuer - the account that creates the token,
          * @param maximum_supply - the maximum supply set for the token created.
          *
          * @pre Token symbol has to be valid,
          * @pre Token symbol must not be already created,
          * @pre maximum_supply has to be smaller than the maximum supply allowed by the system: 1^62 - 1.
          * @pre Maximum supply must be positive;
          */
         [[eosio::action]]
         void create( const name&   issuer,
                      const asset&  maximum_supply);
         /**
          *  This action issues to `to` account a `quantity` of tokens.
          *
          * @param to - the account to issue tokens to, it must be the same as the issuer,
          * @param quntity - the amount of tokens to be issued,
          * @memo - the memo string that accompanies the token issue transaction.
          */
         [[eosio::action]]
         void issue( const name& to, const asset& quantity, const string& memo );

         /**
          * The opposite for create action, if all validations succeed,
          * it debits the statstable.supply amount.
          *
          * @param quantity - the quantity of tokens to retire,
          * @param memo - the memo string to accompany the transaction.
          */
         [[eosio::action]]
         void retire( const asset& quantity, const string& memo );


         /**
          * Allows `from` account to transfer to `to` account the `quantity` tokens.
          * One account is debited and the other is credited with quantity tokens.
          *
          * @param from - the account to transfer from,
          * @param to - the account to be transferred to,
          * @param quantity - the quantity of tokens to be transferred,
          * @param memo - the memo string to accompany the transaction.
          */
         [[eosio::action]]
         void transfer( const name&    from,
                        const name&    to,
                        const asset&   quantity,
                        const string&  memo );

//*** Added GBT

         [[eosio::action]]
         void stake( const name&    account,
                     const name&    receiver,
                      const asset&   quantity,
                      const string&  memo );

         [[eosio::action]]
         void unstake( const name&    account,
                      const name&    receiver,
                      const asset&   quantity,
                      const string&  memo );

         [[eosio::action]]
         void setstaked( const asset& value);

         [[eosio::action]]
         void updateclaim(const name& owner);

         [[eosio::action]]
         void chngstaker( const name&   oldstaker,
                        const name&   newstaker,
                        const name&  account);
         
//***
         /**
          * Add account to the vesting schedule.
          * 
          * @param acct - the account to be added
          * @param quantity - the initial number of locked tokens
          * @param start - the start time of the vesting period
          * @param end - the end time of the vesting period
          * 
          * @pre `acct` exists
          * @pre `quantity` has a symbol of ORE
          * @pre `end` is later than `start`
          * @pre `acct` has an ORE balance of greater than or equal to `quantity`
          * 
          * @post `quantity` tokens will start to unlock at `start` time and unlock every second until `end`
          */
         [[eosio::action]]
         void addvestacct(const name&  acct, 
                          const asset& quantity,
                          const time_point& start,
                          const time_point& end);

         /**
          * Remove account from the vesting schedule
          * 
          * @param acct - the account to remove
          * @param index - vesting information index
          * 
          * @pre `acct` is in the vesting schedule
          * @pre `index` is less than the vector's size
          */
         [[eosio::action]]
         void rmvestacct(const name& acct, uint64_t index);

         /**
          * Allows `ram_payer` to create an account `owner` with zero balance for
          * token `symbol` at the expense of `ram_payer`.
          *
          * @param owner - the account to be created,
          * @param symbol - the token to be payed with by `ram_payer`,
          * @param ram_payer - the account that supports the cost of this action.
          *
          * More information can be read [here](https://github.com/EOSIO/eosio.contracts/issues/62)
          * and [here](https://github.com/EOSIO/eosio.contracts/issues/61).
          */
         [[eosio::action]]
         void open( const name& owner, const symbol& symbol, const name& ram_payer );

         /**
          * This action is the opposite for open, it closes the account `owner`
          * for token `symbol`.
          *
          * @param owner - the owner account to execute the close action for,
          * @param symbol - the symbol of the token to execute the close action for.
          *
          * @pre The pair of owner plus symbol has to exist otherwise no action is executed,
          * @pre If the pair of owner plus symbol exists, the balance has to be zero.
          */
         [[eosio::action]]
         void close( const name& owner, const symbol& symbol );

         static asset get_supply( const name& token_contract_account, const symbol_code& sym_code )
         {
            stats statstable( token_contract_account, sym_code.raw() );
            const auto& st = statstable.get( sym_code.raw() );
            return st.supply;
         }

         static asset get_balance( const name& token_contract_account, const name& owner, const symbol_code& sym_code )
         {
            accounts accountstable( token_contract_account, owner.value );
            const auto& ac = accountstable.get( sym_code.raw() );
            return ac.balance;
         }

//*** Added GBT

         static asset get_total_staked( const name& token_contract_account, const symbol_code& sym_code )
         {  

            stakestats stable( token_contract_account, sym_code.raw() );
            const auto& st = stable.get( sym_code.raw() );

            return st.ore_staked;
         }

         static asset get_staked( const name& token_contract_account, const name& owner, const symbol_code& sym_code )
         {
            reserves reservestable( token_contract_account, owner.value );
            const auto& r = reservestable.get( sym_code.raw() );
            return r.staked;
         }
//***

         using create_action = eosio::action_wrapper<"create"_n, &token::create>;
         using issue_action = eosio::action_wrapper<"issue"_n, &token::issue>;
         using retire_action = eosio::action_wrapper<"retire"_n, &token::retire>;
         using transfer_action = eosio::action_wrapper<"transfer"_n, &token::transfer>;
         using open_action = eosio::action_wrapper<"open"_n, &token::open>;
         using stake_action = eosio::action_wrapper<"stake"_n, &token::stake>;
         using unstake_action = eosio::action_wrapper<"unstake"_n, &token::unstake>;
         using setstaked_action = eosio::action_wrapper<"setstaked"_n, &token::setstaked>;
         using updateclaim_action = eosio::action_wrapper<"updateclaim"_n, &token::updateclaim>;
         using close_action = eosio::action_wrapper<"close"_n, &token::close>;
         using addvest_action = eosio::action_wrapper<"addvestacct"_n, &token::addvestacct>;
         using rmvest_action = eosio::action_wrapper<"rmvestacct"_n, &token::rmvestacct>;
      private:
         struct [[eosio::table]] account {
            asset       balance;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
         };

         struct [[eosio::table]] currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
         };

         typedef eosio::multi_index< "accounts"_n, account > accounts;
         typedef eosio::multi_index< "stat"_n, currency_stats > stats;
         
         void sub_balance( const name& owner, const asset& value );
         void sub_balance_same_payer( const name& owner, const asset& value );
         void add_balance( const name& owner, const asset& value, const name& ram_payer );

//*** Added GBT

         void sub_stake( const name& account, const name& receiver, const asset& value );
         void add_stake( const name& account, const name& receiver, const asset& value );

         void add_stake_stats( const asset& value );
         void sub_stake_stats( const asset& value );

         void add_stake_reserve( const name& account, const asset& value );
         void sub_stake_reserve( const name& account, const asset& value );

         void add_stake_info( const name& account, const name& receiver, const asset& value );
         void sub_stake_info( const name& account, const name& receiver, const asset& value );

//***
         void check_vesting_info(const name& account, const asset& value);
   };

}

#include <eosio.token/eosio.token.hpp>

namespace eosio
{

   void token::create(const name &issuer,
                      const asset &maximum_supply)
   {
      require_auth(get_self());

      auto sym = maximum_supply.symbol;
      check(sym.is_valid(), "invalid symbol name");
      check(maximum_supply.is_valid(), "invalid supply");
      check(maximum_supply.amount > 0, "max-supply must be positive");

      stats statstable(get_self(), sym.code().raw());
      auto existing = statstable.find(sym.code().raw());
      check(existing == statstable.end(), "token with symbol already exists");

      statstable.emplace(get_self(), [&](auto &s) {
         s.supply.symbol = maximum_supply.symbol;
         s.max_supply = maximum_supply;
         s.issuer = issuer;
      });
   }

   void token::issue(const name &to, const asset &quantity, const string &memo)
   {
      auto sym = quantity.symbol;
      check(sym.is_valid(), "invalid symbol name");
      check(memo.size() <= 256, "memo has more than 256 bytes");

      stats statstable(get_self(), sym.code().raw());
      auto existing = statstable.find(sym.code().raw());
      check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
      const auto &st = *existing;
      check(to == st.issuer, "tokens can only be issued to issuer account");

      require_auth(st.issuer);
      check(quantity.is_valid(), "invalid quantity");
      check(quantity.amount > 0, "must issue positive quantity");

      check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
      check(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

      statstable.modify(st, same_payer, [&](auto &s) {
         s.supply += quantity;
      });

      add_balance(st.issuer, quantity, st.issuer);
   }

   void token::retire(const asset &quantity, const string &memo)
   {
      auto sym = quantity.symbol;
      check(sym.is_valid(), "invalid symbol name");
      check(memo.size() <= 256, "memo has more than 256 bytes");

      stats statstable(get_self(), sym.code().raw());
      auto existing = statstable.find(sym.code().raw());
      check(existing != statstable.end(), "token with symbol does not exist");
      const auto &st = *existing;

      require_auth(st.issuer);
      check(quantity.is_valid(), "invalid quantity");
      check(quantity.amount > 0, "must retire positive quantity");

      check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");

      statstable.modify(st, same_payer, [&](auto &s) {
         s.supply -= quantity;
      });

      sub_balance(st.issuer, quantity);
   }

   void token::transfer(const name &from,
                        const name &to,
                        const asset &quantity,
                        const string &memo)
   {
      check(from != to, "cannot transfer to self");
      require_auth(from);
      check(is_account(to), "to account does not exist");
      auto sym = quantity.symbol.code();
      stats statstable(get_self(), sym.raw());
      const auto &st = statstable.get(sym.raw());

      require_recipient(from);
      require_recipient(to);

      check(quantity.is_valid(), "invalid quantity");
      check(quantity.amount > 0, "must transfer positive quantity");
      check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
      check(memo.size() <= 256, "memo has more than 256 bytes");

      auto payer = has_auth(to) ? to : from;

      sub_balance(from, quantity);
      add_balance(to, quantity, payer);
   }

   //*** Added GBT
   void token::stake(const name &account,
                     const name &receiver,
                     const asset &quantity,
                     const string &memo)
   {
      require_auth(ore_system);
      //require_auth(account);
      check(is_account(account), "account does not exist");
      auto sym = quantity.symbol.code();
      stats statstable(get_self(), sym.raw());
      const auto &st = statstable.get(sym.raw());

      check(quantity.is_valid(), "invalid quantity");
      check(quantity.amount > 0, "must stake positive quantity");
      check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
      check(memo.size() <= 256, "memo has more than 256 bytes");

      sub_balance_same_payer(account, quantity);
      add_stake(account, receiver, quantity);
   }

   void token::unstake(const name &account,
                       const name &receiver,
                       const asset &quantity,
                       const string &memo)
   {
      require_auth(ore_system);
      check(is_account(account), "account does not exist");
      auto sym = quantity.symbol.code();
      stats statstable(get_self(), sym.raw());
      const auto &st = statstable.get(sym.raw());

      check(quantity.is_valid(), "invalid quantity");
      check(quantity.amount > 0, "must unstake positive quantity");
      check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
      check(memo.size() <= 256, "memo has more than 256 bytes");

      sub_stake(account, receiver, quantity);
      add_balance(account, quantity, account);
   }

   void token::chngstaker( const name &oldstaker,
                          const name &newstaker,
                          const name &account)
   {
      require_auth(oldstaker);
      stakeinfo itable(get_self(), account.value);

      const auto &rec = itable.get(ore_symbol.code().raw(), "No stake info found");
      check(rec.staker == oldstaker, "You are not the old staker of account");
      
      sub_stake_reserve(oldstaker, rec.amount);
      add_stake_reserve(newstaker, rec.amount);

      sub_stake_info(oldstaker, account, rec.amount);
      add_stake_info(newstaker, account, rec.amount);
   }
   //***


   void token::sub_balance_same_payer(const name &owner, const asset &value)
   {
      accounts from_acnts(get_self(), owner.value);

      const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found for sub_balance_same_payer");
      check(from.balance.amount >= value.amount, "overdrawn balance");

      from_acnts.modify(from, same_payer, [&](auto &a) {
         a.balance -= value;
      });
   }

   void token::sub_balance(const name &owner, const asset &value)
   {
      accounts from_acnts(get_self(), owner.value);

      const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found for subbalance");
      check(from.balance.amount >= value.amount, "overdrawn balance");

      from_acnts.modify(from, same_payer, [&](auto &a) {
         a.balance -= value;
      });
   }

   void token::add_balance(const name &owner, const asset &value, const name &ram_payer)
   {
      accounts to_acnts(get_self(), owner.value);
      auto to = to_acnts.find(value.symbol.code().raw());
      if (to == to_acnts.end())
      {
         to_acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = value;
         });
      }
      else
      {
         to_acnts.modify(to, same_payer, [&](auto &a) {
            a.balance += value;
         });
      }
   }

   //*** Added GBT
   void token::sub_stake(const name &owner, const name &receiver, const asset &value)
   {
      sub_stake_reserve(owner, value);
      sub_stake_info(owner, receiver, value);
      sub_stake_stats(value);
   }

   void token::add_stake(const name &owner, const name &receiver, const asset &value)
   {
      add_stake_reserve(owner, value);
      add_stake_info(owner, receiver, value);
      add_stake_stats(value);
   }

      //*** Added GBT
   
   void token::sub_stake_stats(const asset &value)
   {
      stakestats stable(get_self(), value.symbol.code().raw());

      auto st = stable.find(value.symbol.code().raw());

      check(st->ore_staked.amount - value.amount >= 0, "cannot unstake more than what is available");

      stable.modify(st, ore_system, [&](auto &a) {
         a.ore_staked -= value;
      });
   }

   void token::add_stake_stats(const asset &value)
   {
      stakestats stable(get_self(), value.symbol.code().raw());

      auto st = stable.find(value.symbol.code().raw());

      if (st == stable.end())
      {
         stable.emplace(ore_system, [&](auto &a) {
            a.ore_staked = value;
         });
      }
      else
      {
         stable.modify(st, ore_system, [&](auto &a) {
            a.ore_staked += value;
         });
      }
   }

   void token::sub_stake_reserve(const name &owner, const asset &value)
   {
      reserves rtable(get_self(), owner.value);

      const auto &from = rtable.get(value.symbol.code().raw(), "no reserve object found");
      check(from.staked.amount >= value.amount, "overdrawn balance");

      rtable.modify(from, ore_system, [&](auto &a) {
         a.staked -= value;
      });
   }

   void token::add_stake_reserve(const name &owner, const asset &value)
   {

      reserves rtable(get_self(), owner.value);
      auto to = rtable.find(value.symbol.code().raw());
      if (to == rtable.end())
      {
         rtable.emplace(ore_system, [&](auto &a) {
            a.staked = value;
         });
      }
      else
      {
         rtable.modify(to, ore_system, [&](auto &a) {
            a.staked += value;
         });
      }
   }

   void token::sub_stake_info(const name &owner, const name &receiver, const asset &value)
   {
      stakeinfo itable(get_self(), receiver.value);

      const auto &rec = itable.get(value.symbol.code().raw(), "No stake info found");
      check(rec.amount.amount >= value.amount, "overdrawn balance");
      check(rec.staker == owner, "Receiver has different staker!");
      if(rec.amount.amount == value.amount) {
         itable.erase(rec);
      } else {
         itable.modify(rec, ore_system, [&](auto &a) {
            a.amount -= value;
         });
      }
   }

   void token::add_stake_info(const name &owner, const name &receiver, const asset &value)
   {
      stakeinfo itable(get_self(), receiver.value);
      auto rec = itable.find(value.symbol.code().raw());
      if (rec == itable.end())
      {
         itable.emplace(ore_system, [&](auto &a) {
            a.amount = value;
            a.staker = owner;
         });
      }
      else
      {
         check(rec->staker == owner, "Receiver already has a staker!");
         itable.modify(rec, ore_system, [&](auto &a) {
            a.amount += value;
         });
      }
   }

   //one-time configuration step, should only be called once by eosio account
   void token::setstaked(const asset &value)
   {

      require_auth(_self);

      stakestats stable(get_self(), value.symbol.code().raw());

      auto st = stable.find(value.symbol.code().raw());

      if (st == stable.end())
      {
         stable.emplace(_self, [&](auto &a) {
            a.ore_staked = value;
         });
      }
      else
      {
         stable.modify(st, same_payer, [&](auto &a) {
            a.ore_staked = value;
         });
      }
   }
   //***

   void token::open(const name &owner, const symbol &symbol, const name &ram_payer)
   {
      require_auth(ram_payer);

      check(is_account(owner), "owner account does not exist");

      auto sym_code_raw = symbol.code().raw();
      stats statstable(get_self(), sym_code_raw);
      const auto &st = statstable.get(sym_code_raw, "symbol does not exist");
      check(st.supply.symbol == symbol, "symbol precision mismatch");

      accounts acnts(get_self(), owner.value);
      auto it = acnts.find(sym_code_raw);
      if (it == acnts.end())
      {
         acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = asset{0, symbol};
         });
      }
   }

   //*** Changed GBT

   void token::updateclaim(const name &owner)
   {

      require_auth("eosio"_n);

      token::reserves rtable(get_self(), owner.value);
      auto itr = rtable.find(ore_symbol.code().raw());

      rtable.modify(itr, same_payer, [&](auto &a) {
         a.last_claimed = current_time_point();
      });
   }

   void token::close(const name &owner, const symbol &symbol)
   {
      require_auth(owner);
      accounts acnts(get_self(), owner.value);
      reserves rtable(get_self(), owner.value);
      auto ait = acnts.find(symbol.code().raw());
      auto rit = rtable.find(symbol.code().raw());
      check(ait != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect.");
      check(rit != rtable.end(), "Reserve row already deleted or never existed. Action won't have any effect.");
      check(ait->balance.amount == 0, "Cannot close because the balance is not zero.");
      check(rit->staked.amount == 0, "Cannot close because the staked balance is not zero.");

      acnts.erase(ait);
      rtable.erase(rit);
   }
   //***
} // namespace eosio

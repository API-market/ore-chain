#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/symbol.hpp>
#include <eosio/transaction.hpp>
#include <eosio/time.hpp>
#include <string>
#include <map>

using namespace eosio;

class [[eosio::contract]] oregate : public eosio::contract {

public:
  using eosio::contract::contract;

  struct [[eosio::table]] priority_list {
    eosio::name                account;
    uint64_t primary_key() const { return account.value; }
  };

  typedef eosio::multi_index< "priority"_n, priority_list> priority_table;

  struct [[eosio::table]] queue_info {
    eosio::name                account;
    eosio::asset               total_sent;
    eosio::asset               total_locked;

    uint64_t primary_key() const { return account.value; }
  };

  typedef eosio::multi_index< "queue"_n, queue_info> queue_table;

  [[eosio::action]]
  void addpriority(name account) {
    require_auth(permission_level(get_self(), "active"_n));
    priority_table ptable(get_self(), get_self().value);
    auto pacct = ptable.find(account.value);
    check(pacct == ptable.end(), "Account is already in priority list");
    ptable.emplace(get_self(), [&](priority_list& priority) {
      priority.account = account;
      });
  }
  [[eosio::action]]
  void rmvpriority(name account) {
    require_auth(permission_level(get_self(), "active"_n));
    priority_table ptable(get_self(), get_self().value);
    auto pacct = ptable.find(account.value);
    check(pacct != ptable.end(), "Account is already not in priority list");
    ptable.erase(pacct);
  }

  [[eosio::on_notify("eosio.token::transfer")]]
  void queue(
    const eosio::name sender,
    const eosio::name receiver,
    const eosio::asset quantity,
    const std::string memo
  ) {
    if (sender == get_self()) return;
    eosio::check(receiver == get_self(), "Reject: invalid receiver");
    name tokenContract = _first_receiver;
    eosio::check(quantity.amount > 0, "Reject: insufficient value");
    eosio::check(SUPPORTED_TOKENS.find(tokenContract) != SUPPORTED_TOKENS.end(), "Reject: unsupported token contract");
    eosio::check(SUPPORTED_TOKENS.find(tokenContract)->second == quantity.symbol, "Reject: unsupported token symbol");

    queue_table qtable(get_self(), get_self().value);
    auto qacct = qtable.find(sender.value);

    if (qacct != qtable.end()) {
      // modify
      qtable.modify(qacct, get_self(), [&](queue_info& queue) {
        queue.total_locked += quantity;
        });
    }
    else {
      qtable.emplace(get_self(), [&](queue_info& queue) {
        queue.account = sender;
        queue.total_locked = quantity;
        queue.total_sent.symbol = quantity.symbol;
        });
    }
  };

  [[eosio::action]]
  void sendtoeth(name sender, asset quantity, std::string receiver) {
    require_auth(permission_level(sender, "active"_n));
    check(ore_symbol == quantity.symbol, "invalid symbol");
    check(quantity.amount > 0, "quantity must be positive");
    queue_table qtable(get_self(), get_self().value);
    auto qacct = qtable.find(sender.value);
    check(qacct != qtable.end(), "Sender do not have queued tokens.");
    double unlockRatio = unlock_ratio(sender);
    const int64_t totalUnlocked = qacct->total_locked.amount * unlockRatio;
    const int64_t claimable = totalUnlocked - qacct->total_sent.amount;
    check(quantity.amount <= claimable, "Quantity can't be more than claimable amount: " + asset{ claimable, quantity.symbol }.to_string());

    qtable.modify(qacct, get_self(), [&](queue_info& queue) {
      queue.total_sent += quantity;
      });
    action(
      permission_level{ get_self(), "active"_n },
      "eosio.token"_n,
      "transfer"_n,
      std::make_tuple(get_self(), bridge_contract, quantity, receiver)
    ).send();
    require_recipient(bridge_contract);
  }

  [[eosio::action]]
  void reclaim(name sender, asset quantity) {
    require_auth(permission_level(sender, "active"_n));
    check(ore_symbol == quantity.symbol, "invalid symbol");
    check(quantity.amount > 0, "quantity must be positive");
    queue_table qtable(get_self(), get_self().value);
    auto qacct = qtable.find(sender.value);
    check(qacct != qtable.end(), "Sender do not have queued tokens.");
    double unlockRatio = unlock_ratio(sender);
    const int64_t unrefundableLocked = qacct->total_sent.amount / unlockRatio;
    const int64_t refundable = qacct->total_locked.amount - unrefundableLocked;

    check(quantity.amount <= refundable, "Quantity can't be more than refundable amount: " + asset{ refundable, quantity.symbol }.to_string());

    qtable.modify(qacct, get_self(), [&](queue_info& queue) {
      queue.total_locked -= quantity;
      });
    action(
      permission_level{ get_self(), "active"_n },
      "eosio.token"_n,
      "transfer"_n,
      std::make_tuple(get_self(), sender, quantity, std::string(""))
    ).send();
    require_recipient(sender);
  }

private:
  static constexpr name bridge_contract = "xeth.ptokens"_n;
  static constexpr symbol ore_symbol = symbol(symbol_code("ORE"), 4);
  std::map<eosio::name, eosio::symbol> SUPPORTED_TOKENS = { {"eosio.token"_n, ore_symbol} };
  time_point_sec lock_start_time = eosio::time_point_sec(1634072399);
  time_point_sec lock_end_time = eosio::time_point_sec(1697144399);
  time_point_sec lock_priority_end_time = eosio::time_point_sec(1665608399);

  double unlock_ratio(name account) {
    priority_table ptable(get_self(), get_self().value);
    auto pacct = ptable.find(account.value);
    time_point end_time;
    if (pacct == ptable.end()) {
      end_time = lock_end_time;
    }
    else {
      end_time = lock_priority_end_time;
    }
    auto ct = current_time_point();
    return double(ct.sec_since_epoch() - lock_start_time.sec_since_epoch()) / (end_time.sec_since_epoch() - lock_start_time.sec_since_epoch());
  }
};

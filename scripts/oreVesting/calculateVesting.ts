import dotenv from "dotenv";
import { Api, JsonRpc } from 'eosjs';
import { JsSignatureProvider } from 'eosjs/dist/eosjs-jssig';  // development only

dotenv.config();

import { argv } from 'process';

const url = argv[3] || 'https://ore.openrights.exchange'

const fetch = require('node-fetch');
const signatureProvider = new JsSignatureProvider([]);
const rpc = new JsonRpc(url, { fetch }); //required to read blockchain state
const api = new Api({ rpc, signatureProvider }); //required to submit transactions

async function main() {
  const accountName = argv[2]
  const [balanceResult] = await rpc.get_currency_balance('eosio.token', accountName, 'ORE')
  const [balanceStr] =  balanceResult.split(' ')
  console.log('balance: ', balanceStr)
  const {rows} = await rpc.get_table_rows({
    json: true,                 // Get the response as json
    code: 'eosio.token',           // Contract that we target
    scope: 'eosio.token',          // Account that owns the data
    table: 'vesting',          // Table name
    lower_bound: accountName,     // Table primary key value
    upper_bound: accountName,     // Table primary key value
    limit: 1,                   // Here we limit to 1 to get only the single row with primary key equal to 'testacc'
    reverse: false,             // Optional: Get reversed data
    show_payer: false,          // Optional: Show ram payer
  });
  const {vesting} = rows[0] 
  console.log(vesting)
  let originalAmountLocked = 0
  let totalUnlockedAvailable = 0
  // For each Vesting index increase total locked and unlocked amounts to calculate total available amount in the end
  vesting.forEach( (vest: any) => {
    const {claimed, locked, start_time, end_time} = vest
    const [lockedStr] = locked.split(' ') 
    const [claimedStr] = claimed.split(' ') 
    const start = Math.round((Date.parse(start_time) / 1000))
    const end = Math.round((Date.parse(end_time) / 1000))
    const now = Math.round((Date.now() / 1000))
    // assume now < start which means no tokens are unlocked
    var unlockRatio = 0
    if (now >= start && now <= end) {
      unlockRatio = (now - start)/(end - start)
    } else if(now > end) {
      // all the tokens are unlocked
      unlockRatio = 1
    }
    console.log('lockRatio: ', unlockRatio)
    const unlocked = Number(lockedStr) * unlockRatio
    const unlockedAvailable = unlocked - Number(claimedStr)
    totalUnlockedAvailable += unlockedAvailable
    originalAmountLocked += Number(lockedStr)
  })
  console.log('originalAmountLocked:', originalAmountLocked)
  console.log('totalUnlockedAvailable:', totalUnlockedAvailable)
  console.log('balance:', Number(balanceStr))
  const totalAvailable = Math.max(0, Number(balanceStr) - originalAmountLocked + totalUnlockedAvailable)
  console.log('totalAvailable:', totalAvailable)
}

(async () => {
  await main().catch((error) => console.log("Unexpected error", error));
  process.exit(); // exit Node execution
})();
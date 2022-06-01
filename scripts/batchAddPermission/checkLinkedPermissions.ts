import { ChainFactory, ChainType } from "@open-rights-exchange/chainjs";
const { MongoClient } = require("mongodb");
const createCsvWriter = require('csv-writer').createObjectCsvWriter;

const dotenv = require('dotenv')

dotenv.config();

const oreMainEndpoints = [
  {
    url: "https://ore.openrights.exchange/",
    chainId: "7900eaca71d5b213d3e1e15d54d98ad235a7a5b8166361be78e672edeeb2b47a",
  },
];

const eosChain = new ChainFactory().create(ChainType.EosV2, oreMainEndpoints);

async function run() {
  await eosChain.connect();

  // get vesting accounts
  // TODO loop through and check if vestingInfo.more is true
  const vestingInfo = await eosChain.fetchContractData('eosio.token','vesting','eosio.token');
  
  const vestingAccounts: string[] = []
  vestingInfo.rows.forEach((element: any) => {
    vestingAccounts.push(element.account)
  });

  // connect to mongodb
  const client = new MongoClient(process.env.MONGO_URI);
  await client.connect();
  const database = client.db("oreid-prod");
  const wallets = database.collection("walletAccounts");
  const csvData : any[] = [];
  // check account is linked
  for (const acct of vestingAccounts) {
    const query = { "accounts.accountName": acct };

    const wallet = await wallets.findOne(query);
    const data = {
      vestedaccount: acct,
      islinked: false,
      linkedaccount: ''
    }
    if(wallet) {
      console.log(`${acct} is linked ${wallet.accountName}`);
      data.islinked = true;
      data.linkedaccount = wallet.accountName
    } 
    else {
      console.log(`${acct} is not linked`);
    }
    csvData.push(data);
  };
  // write data out to csv file
  const csvWriter = createCsvWriter({
    path: 'linked_accounts.csv',
    header: [
      {id: 'vestedaccount', title: 'vestedaccount'},
      {id: 'islinked', title: 'islinked'},
      {id: 'linkedaccount', title: 'linkedaccount'}
    ]
  });

  await csvWriter
  .writeRecords(csvData);

  await client.close();
}

(async () => {
  try {
    await run();
  } catch (error) {
    console.log("Error:", error);
  }
  process.exit();
})();

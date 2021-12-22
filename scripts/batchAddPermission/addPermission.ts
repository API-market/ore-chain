import * as fs from "fs";
import { ChainFactory, ChainType } from "@open-rights-exchange/chainjs";
import {
  asyncForEach,
  isNullOrEmpty,
} from "@open-rights-exchange/chainjs/dist/helpers";
import dotenv from "dotenv";
import { ConfirmType } from "@open-rights-exchange/chainjs/dist/models";
dotenv.config();

const { FUNDER_ACCOUNT_PRIVATE_KEY, VESTING_PRIVATE_KEY, NETWORK_TYPE } =
  process.env;

const oreStagingEndpoints = [
  {
    url: "https://ore-staging.openrights.exchange/",
    chainId: "a6df478d5593b4efb1ea20d13ba8a3efc1364ee0bf7dbd85d8d756831c0e3256",
  },
];
const oreMainEndpoints = [
  {
    url: "https://ore.openrights.exchange/",
    chainId: "7900eaca71d5b213d3e1e15d54d98ad235a7a5b8166361be78e672edeeb2b47a",
  },
];
const endpoints =
  NETWORK_TYPE === "ore_main" ? oreMainEndpoints : oreStagingEndpoints;

const eosChain = new ChainFactory().create(ChainType.EosV2, endpoints);

const firstAuthAction = {
  account: "createescrow",
  name: "ping",
  authorization: [{ actor: "app.oreid", permission: "active" }],
  data: { from: "app.oreid" },
};

async function run() {
  await eosChain.connect();
  const transaction = await eosChain.new.Transaction();

  const lines = fs
    .readFileSync("addPermission-input.txt", "utf8")
    .toString()
    .split("\n");

  await asyncForEach(lines, async (line) => {
    const json = JSON.parse(line);

    const {
      accountName,
      accountNameToLink,
      permission,
      parentPermission,
      linkToAction,
      linkToContract
    } = json;

    const account = await eosChain.new.Account(accountName);

    const [activePermission] = account
      .toJson()
      .permissions.filter((perm: any) => perm?.perm_name === "active");

    const activeKey = activePermission.required_auth.keys[0].key;

    console.log("key: ", activeKey);

    let actions: any = [
      firstAuthAction,
      {
        account: "eosio",
        name: "updateauth",
        authorization: [{ actor: accountNameToLink, permission: "active" }],
        data: {
          account: accountNameToLink,
          auth: {
            accounts: [],
            keys: [
              {
                key: activeKey,
                weight: 1,
              },
            ],
            threshold: 1,
            waits: [],
          },
          parent: parentPermission,
          permission,
        },
      },
    ];

    if ( linkToAction && linkToContract ) {
      const linkAction = {
        account: "eosio",
        name: "linkauth",
        authorization: [
          {
            actor: accountNameToLink,
            permission: "owner",
          },
        ],
        data: {
          account: accountNameToLink,
          code: linkToContract,
          type: linkToAction,
          requirement: permission,
        },
      };
      actions = [...actions, linkAction]
    }

    try {
      await transaction.setTransaction(actions);
      await transaction.prepareToBeSigned();
      await transaction.validate();
      await transaction.sign([FUNDER_ACCOUNT_PRIVATE_KEY, VESTING_PRIVATE_KEY]);
      console.log('missing sig: ', transaction.missingSignatures)
      const response = await transaction.send(ConfirmType.After001);
      console.log("TxResonse: ", response);
    } catch (err) {
      console.log("problem with transaction: ", err);
    }

    const DBRecord = {
      accountName: accountNameToLink,
      accountType: "native",
      chainNetwork: NETWORK_TYPE,
      permissions: [
        {
          permissionName: permission, // always this permission name
          publicKey: activeKey,
        },
      ],
    };

    fs.writeFileSync(`${accountName}.txt`, JSON.stringify(DBRecord, null, 2));
  });
}

// DB sample output
// {
//   "accountName" : "orevest11214",     // vesting account
//   "accountType" : "native",
//   "chainNetwork" : "ore_test",
//   "permissions" : [
//     {
//         "permissionName" : "apporeidperm",   // always this permission name
//         "publicKey" : "EOS7uzaxjZpZyDQ2GxHtc3nAZVcRuiXkoq54iXkWra4L6AoHooyEc"
//     }
//   ]
// }

(async () => {
  try {
    await run();
  } catch (error) {
    console.log("Error:", error);
  }
  process.exit();
})();

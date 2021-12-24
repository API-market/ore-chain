import * as fs from "fs";
import { createDfuseClient } from "@dfuse/client";
import dotenv from "dotenv";
import * as csv from "fast-csv";

dotenv.config();

global.fetch = require("node-fetch");
global.WebSocket = require("ws");

const { DFUSE_GRAPHQL_URL, DFUSE_KEY, DEFAULT_TOKEN_CONTRACT } = process.env;

const client = createDfuseClient({
  apiKey: DFUSE_KEY,
  network: DFUSE_GRAPHQL_URL,
});

const operation = `query($contract: String!, $symbol:String!, $limit: Uint32, $cursor:String, $opts: [ACCOUNT_BALANCE_OPTION!]) {
    tokenBalances(contract: $contract, symbol: $symbol,limit: $limit, cursor: $cursor, options: $opts) {
      blockRef {
        id
        number
      }
      pageInfo {
        startCursor
        endCursor
      }
      edges {
        node {
          account
          contract
          symbol
          precision
          balance
        }
      }
    }
  }`;

const { argv } = process;

const variableConst = {
  contract: argv[3] || DEFAULT_TOKEN_CONTRACT,
  symbol: argv[2],
  //   opts: ["EOS_INCLUDE_STAKED"],
  limit: 100,
};

async function main(cursorSelected?: string, num?: number) {
  const headers = num > 0 ? false : true;
  const csvStream = csv.format({ headers });
  const writeableStream = fs.createWriteStream(
    `accountsBalances-${variableConst.symbol}.csv`,
    { flags: "a" }
  );
  csvStream.pipe(writeableStream).on("end", () => process.exit());

  const variables = cursorSelected
    ? { ...variableConst, cursor: cursorSelected }
    : variableConst;
  const { data } = await client.graphql(operation, {
    variables,
    operationType: "query",
  });
  if (!data) return;
  const { pageInfo, edges } = data.tokenBalances || {};
  if (!edges) return;
  const { endCursor } = pageInfo;
  let count = num || 0;
  let write;
  edges?.forEach((edge: any) => {
    count++;
    const { account, balance } = edge.node;
    write = {
      count,
      account,
      balance,
    };
    console.log(JSON.stringify(write));
    csvStream.write(write);
    fs.appendFileSync(
      `accountsBalances-${variableConst.symbol}.txt`,
      JSON.stringify(write) + "\n"
    );
  });
  if (edges?.length == variableConst.limit) {
    console.log("Cursor: ", endCursor);
    await main(endCursor, count);
  } else {
    fs.appendFileSync("accountsBalances.txt", `cursor: ${endCursor}` + "\n");
    csvStream.end();
  }
}

(async () => {
  await main().catch((error) => console.log("Unexpected error", error));
  process.exit(); // exit Node execution
})();

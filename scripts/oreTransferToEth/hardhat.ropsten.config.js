require('@nomiclabs/hardhat-waffle');
require('@openzeppelin/hardhat-upgrades');
require("@nomiclabs/hardhat-etherscan");

const fs = require('fs');

const { asyncForEach } = require('./helpers');


const RPC_URL = 'https://ropsten.infura.io/v3/d9cdd6a730b44793becb790e3a3ac372';
const PRIVATE_KEY = '...';
const GAS_LIMIT = 200000

const TOKEN_ADDRESS = '0x0745c21c6EE943E2D29387812DbDD1839006eb52'

const assert = (condition, message) => {
  if (condition) return;
  throw new Error(message);
}

task('distribute', 'Distribute Vested ORE Tokens')
  .addParam('filename', 'Json file of Token recipient list')
  .setAction(async ({filename}) => {
    const [sender] = await ethers.getSigners();
    const senderAddress = sender.address
    console.log('sendeR: ', senderAddress)
    let rawdata = fs.readFileSync(filename);
    const { accounts } = JSON.parse(rawdata);

    const Token = await ethers.getContractFactory('PTokenWithLPS')
    console.log('tokenaddress: ', TOKEN_ADDRESS)
    const token = await Token.attach(TOKEN_ADDRESS, Token.interface);
    const senderBalance = await token.balanceOf(senderAddress);
    console.log('Sender Balance: ', senderBalance)

    await asyncForEach(accounts, async account => {
      const {address, amount} = account
      console.log('Address: ', address, ' Amount: ', amount)
      const amountWei = ethers.utils.parseUnits(amount, 'ether')
      tx = await token.connect(sender).transfer(address, amountWei)
      await tx.wait()
      console.log('transferred: ', tx)
    })
  });


module.exports = {
  networks: {
    target: {
      // gas: GAS_LIMIT,
      url: RPC_URL,
      accounts: [PRIVATE_KEY],
    },
    fork: {
      url: 'http://localhost:8545',
    },
    hardhat: {
      forking: {
        url: RPC_URL,
      }
    },
  },
  solidity: {
    version: '0.6.12',
    settings: {
      optimizer: {
        enabled: true,
        runs: 200,
      },
    },
  },
  etherscan: {
    // Your API key for Etherscan
    // Obtain one at https://etherscan.io/
    apiKey: "15947TD4DXEI9AGKVFNQKC4123XENUFC7S"
  },
  mocha: {
    timeout: 100000
  }
};

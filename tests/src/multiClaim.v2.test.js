import 'core-js/stable';
import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, genericTx, nano_models } from './test.fixture';
import { ethers } from 'ethers';
import { parseEther } from 'ethers/lib/utils';
import { ledgerService } from '@ledgerhq/hw-app-eth';

const contractAddr = '0x2e3956e1ee8b44ab826556770f69e3b9ca04a2a7';

const pluginName = 'Coinbase';
const abi_path = `../cal/abis/${contractAddr}.json`;
const abi = require(abi_path);

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] MultiClaim V2 Eth',
    zemu(model, async (sim, eth) => {
      const contract = new ethers.Contract(contractAddr, abi);

      const { data } = await contract.populateTransaction.multiClaim(
        [
          '0x86358F7B33b599c484e0335B8Ee4f7f7f92d8b60',
          '0x8d6Fd650500f82c7D978a440348e5a9b886943bF',
        ],
        [
          [42, 47],
          [150, 2],
        ],
        [
          [0, 1],
          [0, 1],
        ]
      );

      let unsignedTx = genericTx;

      unsignedTx.to = contractAddr;
      unsignedTx.data = data;
      unsignedTx.value = parseEther('0');

      const serializedTx = ethers.utils
        .serializeTransaction(unsignedTx)
        .slice(2);
      const resolution = await ledgerService.resolveTransaction(
        serializedTx,
        eth.loadConfig,
        {
          externalPlugins: true,
        }
      );
      const tx = eth.signTransaction("44'/60'/0'/0", serializedTx, resolution);
      const right_clicks = 4;

      await waitForAppScreen(sim);
      await sim.navigateAndCompareSnapshots('.', model.name + '_multiClaimv2', [
        right_clicks,
        0,
      ]);
      await tx;
    }),
    30000
  );
});

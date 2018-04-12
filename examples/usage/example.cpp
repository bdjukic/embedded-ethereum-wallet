//
// AnyLedger - Created by Bogdan Djukic on 01-04-2018.
//

#include "application.h"

#include "../src/Transaction.h"
#include "./helpers/CryptoHelper.h"

#include <vector>

static const uint8_t PRIVATE_KEY[] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                                      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                                      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
                                      0x11, 0x11};

#define CONTRACT_ADDRESS "0x351c7fca3d8ff5d1a385353d71af8e90083538a2"
#define CHAIN_ID 3 // 0x1-mainnet, 0x3-Ropsten, 0x4-Rinkeby
#define GAS_PRICE 50000000000
#define GAS_LIMIT 100000
#define SMART_CONTRACT_METHOD "0x48f14470"

void loop() {

  // First, we need to create public key from the private one
  vector<uint8_t> publicKey = CryptoHelper::generateAddress(PRIVATE_KEY);

  // Then, we will create public address using public key
  vector<char> checksumAddress = CryptoHelper::generateChecksumAddress(publicKey);

  // Let's convert the address into HEX format
  string checksumAddressHex(checksumAddress.begin(), checksumAddress.end());

  Serial.printlnf("Public address: %s", checksumAddressHex.c_str());

  // Now we can start building up the transaction itself
  Transaction transaction;
  transaction.setPrivateKey((uint8_t*)PRIVATE_KEY);

  // Make sure that nonce number is not smaller than the total number of transactions for the address
  // Total transaction is available through JSON-RPC method eth_getTransactionCount
  uint32_t nonce = 42;
  uint32_t chainId = CHAIN_ID;
  uint32_t gasPrice = GAS_PRICE;
  uint32_t gasLimit = GAS_LIMIT;

  string to = CONTRACT_ADDRESS;
  string value = "";
  string data = "0xc604091c0000000000000000000000000000000000000000000000000000000000000011";

  // The last step is passing all the necessary values for transaction signing process
  String rawTransaction = transaction.getRaw(nonce, gasPrice, gasLimit, &to, &value, &data, chainId);

  // Raw transaction is ready to be sent to JSON-RPC method call eth_sendRawTransaction
  Serial.printlnf("Signed raw transaction: %s", rawTransaction);

  delay(5000);
}

void setup() {
}

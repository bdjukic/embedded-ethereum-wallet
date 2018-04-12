# embedded-ethereum-wallet

A Particle library for Embedded Ethereum Wallet

## Welcome to your Embedded Ethereum Wallet library!

You can find example usage of the library inside [examples](examples)

## Usage

Add the embedded-ethereum-wallet library to your project and follow this simple example:

```
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
```

See the [examples](examples) folder for more details.

## Contributing

To get started, [clone the library from GitHub to your local machine](https://github.com/bdjukic/embedded-ethereum-wallet).

## LICENSE
Copyright 2018 Bogdan Djukic

Licensed under the GNU GENERAL PUBLIC LICENSE

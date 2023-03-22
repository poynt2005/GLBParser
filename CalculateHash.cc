#include <string>
#include "CalculateHash.h"
#include "cryptlib.h"
#include "channels.h"
#include "filters.h"
#include "files.h"
#include "sha.h"
#include "sha3.h"
#include "hex.h"

void GetSHA512StringValue(const uint8_t *data, const int length, std::string &outHashString)
{
    CryptoPP::SHA3_512 hash;
    auto stringPipeline = new CryptoPP::StringSink(outHashString);
    auto hexEncoderPipeline = new CryptoPP::HexEncoder(stringPipeline);
    CryptoPP::HashFilter filter(hash, hexEncoderPipeline);
    CryptoPP::ChannelSwitch cs;
    cs.AddDefaultRoute(filter);
    auto cryptoRedirector = new CryptoPP::Redirector(cs);
    CryptoPP::ArraySource(data, length, true, cryptoRedirector);
}
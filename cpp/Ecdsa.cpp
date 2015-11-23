/* 
 * Bitcoin cryptography library
 * Copyright (c) Project Nayuki
 * 
 * http://www.nayuki.io/page/bitcoin-cryptography-library
 * https://github.com/nayuki/Bitcoin-Cryptography-Library
 */

#include <cassert>
#include <cstdint>
#include <cstring>
#include "Ecdsa.hpp"
#include "CurvePoint.hpp"
#include "FieldInt.hpp"
#include "Sha256.hpp"


bool Ecdsa::sign(const Uint256 &privateKey, const Sha256Hash &msgHash, const Uint256 &nonce, Uint256 &outR, Uint256 &outS) {
	/* 
	 * Pseudocode:
	 *   if (nonce outside range [1, order-1]) return false;
	 *   p = nonce * G;
	 *   r = p.x % order;
	 *   if (r == 0) return false;
	 *   s = nonce^-1 * (msgHash + r * privateKey) % order;
	 *   if (s == 0) return false;
	 *   s = min(s, order - s);
	 */
	
	const Uint256 &order = CurvePoint::ORDER;
	const Uint256 &zero = Uint256::ZERO;
	if (nonce == zero || nonce >= order)
		return false;
	
	CurvePoint p(CurvePoint::G);
	p.multiply(nonce);
	p.normalize();
	
	Uint256 r;
	for (int i = 0; i < 8; i++)  // Copy raw value from FieldInt to Uint256
		r.value[i] = p.x.value[i];
	r.subtract(order, static_cast<uint32_t>(r >= order));
	if (r == zero)
		return false;
	assert(r < order);
	
	Uint256 s(r);
	Uint256 z(msgHash.data());
	multiplyModOrder(s, privateKey);
	uint32_t carry = s.add(z, 1);
	s.subtract(order, carry | static_cast<uint32_t>(s >= order));
	
	Uint256 kInv(nonce);
	kInv.reciprocal(order);
	multiplyModOrder(s, kInv);
	if (s == zero)
		return false;
	
	Uint256 negS(order);
	negS.subtract(s);
	s.replace(negS, static_cast<uint32_t>(negS < s));  // To ensure low S values for BIP 62
	outR = r;
	outS = s;
	return true;
}


bool Ecdsa::signWithHmacNonce(const Uint256 &privateKey, const Sha256Hash &msgHash, Uint256 &outR, Uint256 &outS) {
	uint8_t privkeyBytes[32] = {};
	uint8_t msghashBytes[SHA256_HASH_LEN] = {};
	privateKey.getBigEndianBytes(privkeyBytes);
	memcpy(msghashBytes, msgHash.data(), SHA256_HASH_LEN);
	
	Sha256Hash hmac(Sha256::getHmac(privkeyBytes, sizeof(privkeyBytes), msghashBytes, sizeof(msghashBytes)));
	Uint256 nonce(hmac.data());
	return sign(privateKey, msgHash, nonce, outR, outS);
}


void Ecdsa::multiplyModOrder(Uint256 &x, const Uint256 &y) {
	/* 
	 * Russian peasant multiplication with modular reduction at each step. Pseudocode:
	 *   z = 0;
	 *   for (i = 255 .. 0) {
	 *     z = (z * 2) % order;
	 *     if (y.bit[i] == 1)
	 *       z = (z + x) % order;
	 *   }
	 *   x = z;
	 */
	const Uint256 &mod = CurvePoint::ORDER;
	assert(&x != &y && x < mod);
	Uint256 z(Uint256::ZERO);
	
	for (int i = 255; i >= 0; i--) {
		// Multiply by 2
		uint32_t c = z.shiftLeft1();
		z.subtract(mod, c | static_cast<uint32_t>(z >= mod));
		// Conditionally add x
		uint32_t enable = (y.value[i >> 5] >> (i & 31)) & 1;
		c = z.add(x, enable);
		z.subtract(mod, c | static_cast<uint32_t>(z >= mod));
		assert(z < mod);
	}
	x = z;
}
	
	
Ecdsa::Ecdsa() {}

/* bits - handle operations on arrays of bits. 
 *
 * This file is copyright 2002 Jim Kent, but license is hereby
 * granted for all use - public, private or commercial. */

#ifndef BITS_H
#define BITS_H

#include "localmem.h"

typedef unsigned char Bits;

#define bitToByteSize(bitSize) ((bitSize+7)/8)
/* Convert number of bits to number of bytes needed to store bits. */

void bitFree(Bits **pB);
/* Free bits. */

Bits *lmBitAlloc(struct lm *lm,int bitCount);
// Allocate bits.  Must supply local memory.

Bits *lmBitClone(struct lm *lm, Bits* orig, int bitCount);
// Clone bits.  Must supply local memory.

void bitSetOne(Bits *b, int bitIx);
/* Set a single bit. */

void bitClearOne(Bits *b, int bitIx);
/* Clear a single bit. */

void bitSetRange(Bits *b, int startIx, int bitCount);
/* Set a range of bits. */

boolean bitReadOne(Bits *b, int bitIx);
/* Read a single bit. */

int bitFindSet(Bits *b, int startIx, int bitCount);
/* Find the index of the the next set bit. */

int bitFindClear(Bits *b, int startIx, int bitCount);
/* Find the index of the the next clear bit. */

void bitClear(Bits *b, int bitCount);
/* Clear many bits (possibly up to 7 beyond bitCount). */

void bitClearRange(Bits *b, int startIx, int bitCount);
/* Clear a range of bits. */

void bitAnd(Bits *a, Bits *b, int bitCount);
/* And two bitmaps.  Put result in a. */

int bitAndCount(Bits *a, Bits *b, int bitCount);
// Without altering 2 bitmaps, count the AND bits.

void bitOr(Bits *a, Bits *b, int bitCount);
/* Or two bitmaps.  Put result in a. */

int bitOrCount(Bits *a, Bits *b, int bitCount);
// Without altering 2 bitmaps, count the OR'd bits.

void bitXor(Bits *a, Bits *b, int bitCount);
/* Xor two bitmaps.  Put result in a. */

int bitXorCount(Bits *a, Bits *b, int bitCount);
// Without altering 2 bitmaps, count the XOR'd bits.

void bitNot(Bits *a, int bitCount);
/* Flip all bits in a. */

void bitReverseRange(Bits *bits, int startIx, int bitCount);
// Reverses bits in range (e.g. 110010 becomes 010011)

void bitPrint(Bits *a, int startIx, int bitCount, FILE* out);
/* Print part or all of bit map as a string of 0s and 1s.  Mostly useful for
 * debugging */

void bitsOut(FILE* out, Bits *bits, int startIx, int bitCount, boolean onlyOnes);
// Print part or all of bit map as a string of 0s and 1s.
// If onlyOnes, enclose result in [] and use ' ' instead of '0'.

extern int bitsInByte[256];
/* Lookup table for how many bits are set in a byte. */

void bitsInByteInit(void);
/* Initialize bitsInByte array. */

#endif /* BITS_H */


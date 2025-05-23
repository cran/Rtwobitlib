/* Some stuff that you'll likely need in any program that works with
 * DNA.  Includes stuff for amino acids as well. 
 *
 * Assumes that DNA is stored as a character.
 * The DNA it generates will include the bases 
 * as lowercase tcag.  It will generally accept
 * uppercase as well, and also 'n' or 'N' or '-'
 * for unknown bases. 
 *
 * Amino acids are stored as single character upper case. 
 *
 * This file is copyright 2002 Jim Kent, but license is hereby
 * granted for all use - public, private or commercial. */

#include "common.h"
#include "dnautil.h"


struct codonTable
/* The dread codon table. */
    {
    DNA *codon;		/* Lower case. */
    AA protCode;	/* Upper case. The "Standard" code */
    AA mitoCode;	/* Upper case. Vertebrate Mitochondrial translations */
    AA uniqCode;	/* unique code for each codon */
    };

struct codonTable codonTable[] = 
/* The master codon/protein table. */
{
    {"ttt", 'F', 'F', 'a'},
    {"ttc", 'F', 'F', 'b'},
    {"tta", 'L', 'L', 'c'},
    {"ttg", 'L', 'L', 'd'},

    {"tct", 'S', 'S', 'e'},
    {"tcc", 'S', 'S', 'f'},
    {"tca", 'S', 'S', 'g'},
    {"tcg", 'S', 'S', 'h'},

    {"tat", 'Y', 'Y', 'i'},
    {"tac", 'Y', 'Y', 'j'},
    {"taa", 0, 0, 'k'},
    {"tag", 0, 0, 'l'},

    {"tgt", 'C', 'C', 'm'},
    {"tgc", 'C', 'C', 'n'},
    {"tga", 0, 'W', 'o'},
    {"tgg", 'W', 'W', 'p'},


    {"ctt", 'L', 'L', 'q'},
    {"ctc", 'L', 'L', 'r'},
    {"cta", 'L', 'L', 's'},
    {"ctg", 'L', 'L', 't'},

    {"cct", 'P', 'P', 'u'},
    {"ccc", 'P', 'P', 'v'},
    {"cca", 'P', 'P', 'w'},
    {"ccg", 'P', 'P', 'x'},

    {"cat", 'H', 'H', 'y'},
    {"cac", 'H', 'H', 'z'},
    {"caa", 'Q', 'Q', 'A'},
    {"cag", 'Q', 'Q', 'B'},

    {"cgt", 'R', 'R', 'C'},
    {"cgc", 'R', 'R', 'D'},
    {"cga", 'R', 'R', 'E'},
    {"cgg", 'R', 'R', 'F'},


    {"att", 'I', 'I', 'G'},
    {"atc", 'I', 'I', 'H'},
    {"ata", 'I', 'M', 'I'},
    {"atg", 'M', 'M', 'J'},

    {"act", 'T', 'T', 'K'},
    {"acc", 'T', 'T', 'L'},
    {"aca", 'T', 'T', 'M'},
    {"acg", 'T', 'T', 'N'},

    {"aat", 'N', 'N', 'O'},
    {"aac", 'N', 'N', 'P'},
    {"aaa", 'K', 'K', 'Q'},
    {"aag", 'K', 'K', 'R'},

    {"agt", 'S', 'S', 'S'},
    {"agc", 'S', 'S', 'T'},
    {"aga", 'R', 0, 'U'},
    {"agg", 'R', 0, 'V'},


    {"gtt", 'V', 'V', 'W'},
    {"gtc", 'V', 'V', 'X'},
    {"gta", 'V', 'V', 'Y'},
    {"gtg", 'V', 'V', 'Z'},

    {"gct", 'A', 'A', '1'},
    {"gcc", 'A', 'A', '2'},
    {"gca", 'A', 'A', '3'},
    {"gcg", 'A', 'A', '4'},

    {"gat", 'D', 'D', '5'},
    {"gac", 'D', 'D', '6'},
    {"gaa", 'E', 'E', '7'},
    {"gag", 'E', 'E', '8'},

    {"ggt", 'G', 'G', '9'},
    {"ggc", 'G', 'G', '0'},
    {"gga", 'G', 'G', '@'},
    {"ggg", 'G', 'G', '$'},
};

/* A table that gives values 0 for t
			     1 for c
			     2 for a
			     3 for g
 * (which is order aa's are in biochemistry codon tables)
 * and gives -1 for all others. */
int ntVal[256];
int ntValLower[256];	/* NT values only for lower case. */
int ntValUpper[256];	/* NT values only for upper case. */
int ntVal5[256];
int ntValNoN[256]; /* Like ntVal, but with T_BASE_VAL in place of -1 for nonexistent ones. */
DNA valToNt[(N_BASE_VAL|MASKED_BASE_BIT)+1];

/* convert tables for bit-4 indicating masked */
int ntValMasked[256];
DNA valToNtMasked[256];

static boolean inittedNtVal = FALSE;

static void initNtVal(void)
{
if (!inittedNtVal)
    {
    int i;
    for (i=0; i<ArraySize(ntVal); i++)
        {
	ntValUpper[i] = ntValLower[i] = ntVal[i] = -1;
        ntValNoN[i] = T_BASE_VAL;
	if (isspace(i) || isdigit(i))
	    ntVal5[i] = ntValMasked[i] = -1;
	else
            {
	    ntVal5[i] = N_BASE_VAL;
	    ntValMasked[i] = (islower(i) ? (N_BASE_VAL|MASKED_BASE_BIT) : N_BASE_VAL);
            }
        }
    ntVal5['t'] = ntVal5['T'] = ntValNoN['t'] = ntValNoN['T'] = ntVal['t'] = ntVal['T'] = 
    	ntValLower['t'] = ntValUpper['T'] = T_BASE_VAL;
    ntVal5['u'] = ntVal5['U'] = ntValNoN['u'] = ntValNoN['U'] = ntVal['u'] = ntVal['U'] = 
    	ntValLower['u'] = ntValUpper['U'] = U_BASE_VAL;
    ntVal5['c'] = ntVal5['C'] = ntValNoN['c'] = ntValNoN['C'] = ntVal['c'] = ntVal['C'] = 
    	ntValLower['c'] = ntValUpper['C'] = C_BASE_VAL;
    ntVal5['a'] = ntVal5['A'] = ntValNoN['a'] = ntValNoN['A'] = ntVal['a'] = ntVal['A'] = 
    	ntValLower['a'] = ntValUpper['A'] = A_BASE_VAL;
    ntVal5['g'] = ntVal5['G'] = ntValNoN['g'] = ntValNoN['G'] = ntVal['g'] = ntVal['G'] = 
    	ntValLower['g'] = ntValUpper['G'] = G_BASE_VAL;

    valToNt[T_BASE_VAL] = valToNt[T_BASE_VAL|MASKED_BASE_BIT] = 't';
    valToNt[C_BASE_VAL] = valToNt[C_BASE_VAL|MASKED_BASE_BIT] = 'c';
    valToNt[A_BASE_VAL] = valToNt[A_BASE_VAL|MASKED_BASE_BIT] = 'a';
    valToNt[G_BASE_VAL] = valToNt[G_BASE_VAL|MASKED_BASE_BIT] = 'g';
    valToNt[N_BASE_VAL] = valToNt[N_BASE_VAL|MASKED_BASE_BIT] = 'n';

    /* masked values */
    ntValMasked['T'] = T_BASE_VAL;
    ntValMasked['U'] = U_BASE_VAL;
    ntValMasked['C'] = C_BASE_VAL;
    ntValMasked['A'] = A_BASE_VAL;
    ntValMasked['G'] = G_BASE_VAL;

    ntValMasked['t'] = T_BASE_VAL|MASKED_BASE_BIT;
    ntValMasked['u'] = U_BASE_VAL|MASKED_BASE_BIT;
    ntValMasked['c'] = C_BASE_VAL|MASKED_BASE_BIT;
    ntValMasked['a'] = A_BASE_VAL|MASKED_BASE_BIT;
    ntValMasked['g'] = G_BASE_VAL|MASKED_BASE_BIT;

    valToNtMasked[T_BASE_VAL] = 'T';
    valToNtMasked[C_BASE_VAL] = 'C';
    valToNtMasked[A_BASE_VAL] = 'A';
    valToNtMasked[G_BASE_VAL] = 'G';
    valToNtMasked[N_BASE_VAL] = 'N';

    valToNtMasked[T_BASE_VAL|MASKED_BASE_BIT] = 't';
    valToNtMasked[C_BASE_VAL|MASKED_BASE_BIT] = 'c';
    valToNtMasked[A_BASE_VAL|MASKED_BASE_BIT] = 'a';
    valToNtMasked[G_BASE_VAL|MASKED_BASE_BIT] = 'g';
    valToNtMasked[N_BASE_VAL|MASKED_BASE_BIT] = 'n';

    inittedNtVal = TRUE;
    }
}

/* Returns one letter code for protein, 
 * 0 for stop codon or X for bad input,
 * The "Standard" Code */
AA lookupCodon(DNA *dna)
{
int ix;
int i;
char c;

if (!inittedNtVal)
    initNtVal();
ix = 0;
for (i=0; i<3; ++i)
    {
    int bv = ntVal[(int)dna[i]];
    if (bv<0)
	return 'X';
    ix = (ix<<2) + bv;
    }
c = codonTable[ix].protCode;
return c;
}

boolean isStopCodon(DNA *dna)
/* Return TRUE if it's a stop codon. */
{
return lookupCodon(dna) == 0;
}

boolean isKozak(char *dna, int dnaSize, int pos)
/* Return TRUE if it's a Kozak compatible start. */
{
if (lookupCodon(dna+pos) != 'M')
   {
   return FALSE;
   }
if (pos + 3 < dnaSize)
    {
    if (ntVal[(int)dna[pos+3]] == G_BASE_VAL)
        return TRUE;
    }
if (pos >= 3)
    {
    int c = ntVal[(int)dna[pos-3]];
    if (c == A_BASE_VAL || c == G_BASE_VAL)
        return TRUE;
    }
return FALSE;
}


boolean isReallyStopCodon(char *dna, boolean selenocysteine)
/* Return TRUE if it's really a stop codon, even considering
 * possibilility of selenocysteine. */
{
if (selenocysteine)
    {
    /* Luckily the mitochondria *also* replaces TGA with 
     * something else, even though it isn't selenocysteine */
    return lookupMitoCodon(dna) == 0;
    }
else
    {
    return lookupCodon(dna) == 0;
    }
}


/* Returns one letter code for protein, 
 * 0 for stop codon or X for bad input,
 * Vertebrate Mitochondrial Code */
AA lookupMitoCodon(DNA *dna)
{
int ix;
int i;
char c;

if (!inittedNtVal)
    initNtVal();
ix = 0;
for (i=0; i<3; ++i)
    {
    int bv = ntVal[(int)dna[i]];
    if (bv<0)
	return 'X';
    ix = (ix<<2) + bv;
    }
c = codonTable[ix].mitoCode;
c = toupper(c);
return c;
}

AA lookupUniqCodon(DNA *dna)
{
int ix;
int i;
char c;

if (!inittedNtVal)
    initNtVal();
ix = 0;
for (i=0; i<3; ++i)
    {
    int bv = ntVal[(int)dna[i]];
    if (bv<0)
	return 'X';
    ix = (ix<<2) + bv;
    }
c = codonTable[ix].uniqCode;
c = toupper(c);
return c;
}

Codon codonVal(DNA *start)
/* Return value from 0-63 of codon starting at start. 
 * Returns -1 if not a codon. */
{
int v1,v2,v3;

if ((v1 = ntVal[(int)start[0]]) < 0)
    return -1;
if ((v2 = ntVal[(int)start[1]]) < 0)
    return -1;
if ((v3 = ntVal[(int)start[2]]) < 0)
    return -1;
return ((v1<<4) + (v2<<2) + v3);
}

DNA *valToCodon(int val)
/* Return  codon corresponding to val (0-63) */
{
assert(val >= 0 && val < 64);
return codonTable[val].codon;
}

void dnaTranslateSome(DNA *dna, char *out, int outSize)
/* Translate DNA upto a stop codon or until outSize-1 amino acids, 
 * whichever comes first. Output will be zero terminated. */
{
int i;
int dnaSize;
int protSize = 0;

outSize -= 1;  /* Room for terminal zero */
dnaSize = strlen(dna);
for (i=0; i<dnaSize-2; i+=3)
    {
    if (protSize >= outSize)
        break;
    if ((out[protSize++] = lookupCodon(dna+i)) == 0)
        break;
    }
out[protSize] = 0;
}

/* A little array to help us decide if a character is a 
 * nucleotide, and if so convert it to lower case. */
char ntChars[256];

static void initNtChars(void)
{
static boolean initted = FALSE;

if (!initted)
    {
    zeroBytes(ntChars, sizeof(ntChars));
    ntChars['a'] = ntChars['A'] = 'a';
    ntChars['c'] = ntChars['C'] = 'c';
    ntChars['g'] = ntChars['G'] = 'g';
    ntChars['t'] = ntChars['T'] = 't';
    ntChars['n'] = ntChars['N'] = 'n';
    ntChars['u'] = ntChars['U'] = 'u';
    ntChars['-'] = 'n';
    initted = TRUE;
    }
}

char ntMixedCaseChars[256];

static void initNtMixedCaseChars(void)
{
static boolean initted = FALSE;

if (!initted)
    {
    zeroBytes(ntMixedCaseChars, sizeof(ntMixedCaseChars));
    ntMixedCaseChars['a'] = 'a';
    ntMixedCaseChars['A'] = 'A';
    ntMixedCaseChars['c'] = 'c';
    ntMixedCaseChars['C'] = 'C';
    ntMixedCaseChars['g'] = 'g';
    ntMixedCaseChars['G'] = 'G';
    ntMixedCaseChars['t'] = 't';
    ntMixedCaseChars['T'] = 'T';
    ntMixedCaseChars['n'] = 'n';
    ntMixedCaseChars['N'] = 'N';
    ntMixedCaseChars['u'] = 'u';
    ntMixedCaseChars['U'] = 'U';
    ntMixedCaseChars['-'] = 'n';
    initted = TRUE;
    }
}

/* Another array to help us do complement of DNA */
DNA ntCompTable[256];
static boolean inittedCompTable = FALSE;

static void initNtCompTable(void)
{
zeroBytes(ntCompTable, sizeof(ntCompTable));
ntCompTable[' '] = ' ';
ntCompTable['-'] = '-';
ntCompTable['='] = '=';
ntCompTable['a'] = 't';
ntCompTable['c'] = 'g';
ntCompTable['g'] = 'c';
ntCompTable['t'] = 'a';
ntCompTable['u'] = 'a';
ntCompTable['n'] = 'n';
ntCompTable['-'] = '-';
ntCompTable['.'] = '.';
ntCompTable['A'] = 'T';
ntCompTable['C'] = 'G';
ntCompTable['G'] = 'C';
ntCompTable['T'] = 'A';
ntCompTable['U'] = 'A';
ntCompTable['N'] = 'N';
ntCompTable['R'] = 'Y';
ntCompTable['Y'] = 'R';
ntCompTable['M'] = 'K';
ntCompTable['K'] = 'M';
ntCompTable['S'] = 'S';
ntCompTable['W'] = 'W';
ntCompTable['V'] = 'B';
ntCompTable['H'] = 'D';
ntCompTable['D'] = 'H';
ntCompTable['B'] = 'V';
ntCompTable['X'] = 'N';
ntCompTable['r'] = 'y';
ntCompTable['y'] = 'r';
ntCompTable['s'] = 's';
ntCompTable['w'] = 'w';
ntCompTable['m'] = 'k';
ntCompTable['k'] = 'm';
ntCompTable['v'] = 'b';
ntCompTable['h'] = 'd';
ntCompTable['d'] = 'h';
ntCompTable['b'] = 'v';
ntCompTable['x'] = 'n';
ntCompTable['('] = ')';
ntCompTable[')'] = '(';
inittedCompTable = TRUE;
}

/* Complement DNA (not reverse). */
void complement(DNA *dna, long length)
{
int i;

if (!inittedCompTable) initNtCompTable();
for (i=0; i<length; ++i)
    {
    *dna = ntCompTable[(int)*dna];
    ++dna;
    }
}


/* Reverse complement DNA. */
void reverseComplement(DNA *dna, long length)
{
reverseBytes(dna, length);
complement(dna, length);
}

/* Reverse offset - return what will be offset (0 based) to
 * same member of array after array is reversed. */
long reverseOffset(long offset, long arraySize)
{
return arraySize-1 - offset;
}

/* Switch start/end (zero based half open) coordinates
 * to opposite strand. */
void reverseIntRange(int *pStart, int *pEnd, int size)
{
int temp;
temp = *pStart;
*pStart = size - *pEnd;
*pEnd = size - temp;
}

/* Switch start/end (zero based half open) coordinates
 * to opposite strand. */
void reverseUnsignedRange(unsigned *pStart, unsigned *pEnd, int size)
{
unsigned temp;
temp = *pStart;
*pStart = size - *pEnd;
*pEnd = size - temp;
}

char *reverseComplementSlashSeparated(char *alleleStr)
/* Given a slash-separated series of sequences (a common representation of variant alleles),
 * returns a slash-sep series with the reverse complement of each sequence (if it is a
 * nucleotide sequence).
 * Special behavior to support dbSNP's variant allele conventions:
 * 1. Reverse the order of sequences (to maintain alphabetical ordering).
 * 2. If alleleStr begins with "-/", then after reversing, move "-/" back to the beginning. */
{
int len = strlen(alleleStr);
char choppyCopy[len+1];
safecpy(choppyCopy, sizeof(choppyCopy), alleleStr);
char *alleles[len];
int alCount = chopByChar(choppyCopy, '/', alleles, ArraySize(alleles));
char *outStr = needMem(len+1);
int i;
for (i = alCount-1;  i >= 0;  i--)
    {
    char *allele = alleles[i];
    int alLen = strlen(allele);
    if (isAllNt(allele, alLen))
        reverseComplement(allele, alLen);
    if (i != alCount-1)
        safecat(outStr, len+1, "/");
    safecat(outStr, len+1, allele);
    }
if (startsWith("-/", alleleStr))
    {
    // Keep "-/" at the beginning:
    memmove(outStr+2, outStr, len-2);
    outStr[0] = '-';
    outStr[1] = '/';
    }
return outStr;
}

int cmpDnaStrings(DNA *a, DNA *b)
/* Compare using screwy non-alphabetical DNA order TCGA */
{
for (;;)
    {
    DNA aa = *a++;
    DNA bb = *b++;
    if (aa != bb)
        return ntVal[(int)aa] - ntVal[(int)bb];
    if (aa == 0)
	break;
    }
return 0;
}


/* Convert U's to T's */
void toDna(DNA *rna)
{
DNA c;
for (;;)
    {
    c = *rna;
    if (c == 'u')
	*rna = 't';
    else if (c == 'U')
	*rna = 'T';
    else if (c == 0)
	break;
    ++rna;
    }
}

/* Convert T's to U's */
void toRna(DNA *dna)
{
DNA c;
for (;;)
    {
    c = *dna;
    if (c == 't')
	*dna = 'u';
    else if (c == 'T')
	*dna = 'U';
    else if (c == 0)
	break;
    ++dna;
    }
}

char *skipIgnoringDash(char *a, int size, bool2 skipTrailingDash)
/* Count size number of characters, and any 
 * dash characters. */
{
while (size > 0)
    {
    if (*a++ != '-')
        --size;
    }
if (skipTrailingDash)
    while (*a == '-')
       ++a;
return a;
}

int countNonDash(char *a, int size)
/* Count number of non-dash characters. */
{
int count = 0;
int i;
for (i=0; i<size; ++i)
    if (a[i] != '-') 
        ++count;
return count;
}

int nextPowerOfFour(long x)
/* Return next power of four that would be greater or equal to x.
 * For instance if x < 4, return 1, if x < 16 return 2.... 
 * (From biological point of view how many bases are needed to
 * code this number.) */
{
int count = 1;
while (x > 4)
    {
    count += 1;
    x >>= 2;
    }
return count;
}

long dnaOrAaFilteredSize(char *raw, char filter[256])
/* Return how long DNA will be after non-DNA is filtered out. */
{
char c;
long count = 0;
dnaUtilOpen();
while ((c = *raw++) != 0)
    {
    if (filter[(int)c]) ++count;
    }
return count;
}

void dnaOrAaFilter(char *in, char *out, char filter[256])
/* Run chars through filter. */
{
char c;
dnaUtilOpen();
while ((c = *in++) != 0)
    {
    if ((c = filter[(int)c]) != 0) *out++ = c;
    }
*out++ = 0;
}

long dnaFilteredSize(char *rawDna)
/* Return how long DNA will be after non-DNA is filtered out. */
{
return dnaOrAaFilteredSize(rawDna, ntChars);
}

void dnaFilter(char *in, DNA *out)
/* Filter out non-DNA characters and change to lower case. */
{
dnaOrAaFilter(in, out, ntChars);
}

void dnaFilterToN(char *in, DNA *out)
/* Change all non-DNA characters to N. */
{
DNA c;
initNtChars();
while ((c = *in++) != 0)
    {
    if ((c = ntChars[(int)c]) != 0) *out++ = c;
    else *out++ = 'n';
    }
*out++ = 0;
}

void dnaMixedCaseFilter(char *in, DNA *out)
/* Filter out non-DNA characters but leave case intact. */
{
dnaOrAaFilter(in, out, ntMixedCaseChars);
}

long aaFilteredSize(char *raw)
/* Return how long aa will be after non-aa chars is filtered out. */
{
return dnaOrAaFilteredSize(raw, aaChars);
}

void aaFilter(char *in, DNA *out)
/* Filter out non-aa characters and change to upper case. */
{
dnaOrAaFilter(in, out, aaChars);
}

void upperToN(char *s, int size)
/* Turn upper case letters to N's. */
{
char c;
int i;
for (i=0; i<size; ++i)
    {
    c = s[i];
    if (isupper(c))
        s[i] = 'n';
    }
}

void lowerToN(char *s, int size)
/* Turn lower case letters to N's. */
{
char c;
int i;
for (i=0; i<size; ++i)
    {
    c = s[i];
    if (islower(c))
        s[i] = 'N';
    }
}


void dnaBaseHistogram(DNA *dna, int dnaSize, int histogram[4])
/* Count up frequency of occurance of each base and store 
 * results in histogram. */
{
int val;
zeroBytes(histogram, 4*sizeof(int));
while (--dnaSize >= 0)
    {
    if ((val = ntVal[(int)*dna++]) >= 0)
        ++histogram[val];
    }
}

bits64 basesToBits64(char *dna, int size)
/* Convert dna of given size (up to 32) to binary representation */
{
if (size > 32)
    errAbort("basesToBits64 called on %d bases, can only go up to 32", size);
bits64 result = 0;
int i;
for (i=0; i<size; ++i)
    {
    result <<= 2;
    result += ntValNoN[(int)dna[i]];
    }
return result;
}

bits32 packDna16(const DNA *in)
/* pack 16 bases into a word */
{
bits32 out = 0;
int count = 16;
int bVal;
while (--count >= 0)
    {
    bVal = ntValNoN[(int)*in++];
    out <<= 2;
    out += bVal;
    }
return out;
}

bits16 packDna8(const DNA *in)
/* Pack 8 bases into a short word */
{
bits16 out = 0;
int count = 8;
int bVal;
while (--count >= 0)
    {
    bVal = ntValNoN[(int)*in++];
    out <<= 2;
    out += bVal;
    }
return out;
}

UBYTE packDna4(const DNA *in)
/* Pack 4 bases into a UBYTE */
{
UBYTE out = 0;
int count = 4;
int bVal;
while (--count >= 0)
    {
    bVal = ntValNoN[(int)*in++];
    out <<= 2;
    out += bVal;
    }
return out;
}

void unpackDna(bits32 *tiles, int tileCount, DNA *out)
/* Unpack DNA. Expands to 16x tileCount in output. */
{
int i, j;
bits32 tile;

for (i=0; i<tileCount; ++i)
    {
    tile = tiles[i];
    for (j=15; j>=0; --j)
        {
        out[j] = valToNt[tile & 0x3];
        tile >>= 2;
        }
    out += 16;
    }
}

void unpackDna4(UBYTE *tiles, int byteCount, DNA *out)
/* Unpack DNA. Expands to 4x byteCount in output. */
{
int i, j;
UBYTE tile;

for (i=0; i<byteCount; ++i)
    {
    tile = tiles[i];
    for (j=3; j>=0; --j)
        {
        out[j] = valToNt[tile & 0x3];
        tile >>= 2;
        }
    out += 4;
    }
}




static void checkSizeTypes(void)
/* Make sure that some of our predefined types are the right size. */
{
assert(sizeof(UBYTE) == 1);
assert(sizeof(WORD) == 2);
assert(sizeof(bits32) == 4);
assert(sizeof(bits16) == 2);
}

int intronOrientationMinSize(DNA *iStart, DNA *iEnd, int minIntronSize)
/* Given a gap in genome from iStart to iEnd, return 
 * Return 1 for GT/AG intron between left and right, -1 for CT/AC, 0 for no
 * intron.  Assumes DNA is lower cased. */
{
if (iEnd - iStart < minIntronSize)
    return 0;
if (iStart[0] == 'g' && iStart[1] == 't' && iEnd[-2] == 'a' && iEnd[-1] == 'g')
    {
    return 1;
    }
else if (iStart[0] == 'c' && iStart[1] == 't' && iEnd[-2] == 'a' && iEnd[-1] == 'c')
    {
    return -1;
    }
else
    return 0;
}

int intronOrientation(DNA *iStart, DNA *iEnd)
/* Given a gap in genome from iStart to iEnd, return 
 * Return 1 for GT/AG intron between left and right, -1 for CT/AC, 0 for no
 * intron.  Assumes DNA is lower cased. */
{
return intronOrientationMinSize(iStart, iEnd, 32);
}

int dnaScore2(DNA a, DNA b)
/* Score match between two bases (relatively crudely). */
{
if (a == 'n' || b == 'n') return 0;
if (a == b) return 1;
else return -1;
}

int  dnaOrAaScoreMatch(char *a, char *b, int size, int matchScore, int mismatchScore, 
	char ignore)
/* Compare two sequences (without inserts or deletions) and score. */
{
int i;
int score = 0;
for (i=0; i<size; ++i)
    {
    char aa = a[i];
    char bb = b[i];
    if (aa == ignore || bb == ignore)
        continue;
    if (aa == bb)
        score += matchScore;
    else
        score += mismatchScore;
    }
return score;
}

int dnaScoreMatch(DNA *a, DNA *b, int size)
/* Compare two pieces of DNA base by base. Total mismatches are
 * subtracted from total matches and returned as score. 'N's 
 * neither hurt nor help score. */
{
return dnaOrAaScoreMatch(a, b, size, 1, -1, 'n');
}

int aaScore2(AA a, AA b)
/* Score match between two amino acids (relatively crudely). */
{
if (a == 'X' || b == 'X') return 0;
if (a == b) return 2;
else return -1;
}

int aaScoreMatch(AA *a, AA *b, int size)
/* Compare two peptides aa by aa. */
{
return dnaOrAaScoreMatch(a, b, size, 2, -1, 'X');
}

void writeSeqWithBreaks(FILE *f, char *letters, int letterCount, int maxPerLine)
/* Write out letters with newlines every maxLine. */
{
int lettersLeft = letterCount;
int lineSize;
while (lettersLeft > 0)
    {
    lineSize = lettersLeft;
    if (lineSize > maxPerLine)
        lineSize = maxPerLine;
    mustWrite(f, letters, lineSize);
    fputc('\n', f);
    letters += lineSize;
    lettersLeft -= lineSize;
    }
}

boolean isDna(char *poly, int size)
/* Return TRUE if letters in poly are at least 90% ACGTNU- */
{
int i;
int dnaCount = 0;

dnaUtilOpen();
for (i=0; i<size; ++i)
    {
    if (ntChars[(int)poly[i]])
	dnaCount += 1;
    }
return (dnaCount >= round(0.9 * size));
}

boolean isAllNt(char *seq, int size)
/* Return TRUE if all letters in seq are ACGTNU-. */
{
int i;
dnaUtilOpen();
for (i = 0;  i < size;  ++i)
    {
    if (ntChars[(int)seq[i]] == 0)
	return FALSE;
    }
return TRUE;
}



/* Tables to convert from 0-20 to ascii single letter representation
 * of proteins. */
int aaVal[256];
AA valToAa[21];

AA aaChars[256];	/* 0 except for value aa characters.  Converts to upper case rest. */

struct aminoAcidTable
/* A little info about each amino acid. */
    {
    int ix;
    char letter;
    char abbreviation[3];
    char *name;
    };

struct aminoAcidTable aminoAcidTable[] = 
{
    {0, 'A', "ala", "alanine"},
    {1, 'C', "cys", "cysteine"},
    {2, 'D', "asp",  "aspartic acid"},
    {3, 'E', "glu",  "glutamic acid"},
    {4, 'F', "phe",  "phenylalanine"},
    {5, 'G', "gly",  "glycine"},
    {6, 'H', "his",  "histidine"},
    {7, 'I', "ile",  "isoleucine"},
    {8, 'K', "lys",  "lysine"},
    {9, 'L', "leu",  "leucine"},
    {10, 'M',  "met", "methionine"},
    {11, 'N',  "asn", "asparagine"},
    {12, 'P',  "pro", "proline"},
    {13, 'Q',  "gln", "glutamine"},
    {14, 'R',  "arg", "arginine"},
    {15, 'S',  "ser", "serine"},
    {16, 'T',  "thr", "threonine"},
    {17, 'V',  "val", "valine"},
    {18, 'W',  "trp", "tryptophan"},
    {19, 'Y',  "tyr", "tyrosine"},
    {20, 'X',  "ter", "termination"},
};

char *aaAbbr(int i)
/* return pointer to AA abbrevation */
{
return(aminoAcidTable[i].abbreviation);
}

char aaLetter(int i)
/* return AA letter */
{
return(aminoAcidTable[i].letter);
}

static void initAaVal(void)
/* Initialize aaVal and valToAa tables. */
{
int i;
char c, lowc;

for (i=0; i<ArraySize(aaVal); ++i)
    aaVal[i] = -1;
for (i=0; i<ArraySize(aminoAcidTable); ++i)
    {
    c = aminoAcidTable[i].letter;
    lowc = tolower(c);
    aaVal[(int)c] = aaVal[(int)lowc] = i;
    aaChars[(int)c] = aaChars[(int)lowc] = c;
    valToAa[i] = c;
    }
aaChars['x'] = aaChars['X'] = 'X';
}

void dnaUtilOpen(void)
/* Initialize stuff herein. */
{
static boolean opened = FALSE;
if (!opened)
    {
    checkSizeTypes();
    initNtVal();
    initAaVal();
    initNtChars();
    initNtMixedCaseChars();
    initNtCompTable();
    opened = TRUE;
    }
}

char aaAbbrToLetter(char *abbr)
/* Convert an AA abbreviation such as "Ala", "Asp" etc., to its single letter code
 * such as "A", "D" etc.  Return the null char '\0' if abbr is not found. */
{
// Lowercase for comparison.
char abbrLC[4];
safencpy(abbrLC, sizeof(abbrLC), abbr, 3);
toLowerN(abbrLC, 3);
int ix;
for (ix = 0;  ix < ArraySize(aminoAcidTable);  ix++)
    {
    if (sameStringN(abbrLC, aminoAcidTable[ix].abbreviation, 3))
        return aminoAcidTable[ix].letter;
    }
return '\0';
}

void aaToAbbr(char aa, char *abbrBuf, size_t abbrBufSize)
/* Convert an AA single letter such as "A", "D" etc. to its abbreviation such as "Ala", "Asp" etc.
 * abbrBufSize must be at least 4.  If aa is not found, "?%c?",aa is written into abbrBuf. */
{
// Uppercase for comparison.
char aaUC = toupper(aa);
int ix;
for (ix = 0;  ix < ArraySize(aminoAcidTable);  ix++)
    {
    if (aaUC == aminoAcidTable[ix].letter)
        {
        // safencpy(...3) is required here because aminoAcidTable.abbreviation is char[3] not [4]
        safencpy(abbrBuf, abbrBufSize, aminoAcidTable[ix].abbreviation, 3);
        abbrBuf[0] = toupper(abbrBuf[0]);
        return;
        }
    }
safef(abbrBuf, abbrBufSize, "?%c?", aa);
}

void trimRefAltDir(char *ref, char *alt, unsigned int *pStart, unsigned int *pEnd, int *pRefLen, int *pAltLen,
                   boolean leftJustify)
/* If ref and alt have identical bases at beginning and/or end, trim those & update all params. */
{
int trimStart = 0, trimEnd = 0;
int refLen = strlen(ref);
int altLen = strlen(alt);
if (leftJustify)
    {
    // first trim end, then start
    while (refLen > 0 && altLen > 0 && ref[refLen-1] == alt[altLen-1])
        {
        (*pEnd)--;
        refLen--;
        altLen--;
        ref[refLen] = 0;
        alt[altLen] = 0;
        }
    while (ref[trimStart] != '\0' && alt[trimStart] != '\0' && ref[trimStart] == alt[trimStart])
        trimStart++;
    }
else
    {
    // first trim start, then end
    while (ref[trimStart] != '\0' && alt[trimStart] != '\0' && ref[trimStart] == alt[trimStart])
        trimStart++;
    int iR = refLen - 1, iA = altLen - 1;
    while (iR >= trimStart && iA >= trimStart && ref[iR] == alt[iA])
        {
        iR--;
        iA--;
        trimEnd++;
        }
    }
if (trimEnd)
    {
    *pEnd -= trimEnd;
    refLen -= trimEnd;
    altLen -= trimEnd;
    ref[refLen] = '\0';
    alt[altLen] = '\0';
    }
if (trimStart)
    {
    *pStart += trimStart;
    refLen -= trimStart;
    altLen -= trimStart;
    memmove(ref, ref+trimStart, refLen+1);
    memmove(alt, alt+trimStart, altLen+1);
    }
*pRefLen = refLen;
*pAltLen = altLen;
}

void trimRefAlt(char *ref, char *alt, unsigned int *pStart, unsigned int *pEnd, int *pRefLen, int *pAltLen)
/* If ref and alt have identical bases at beginning and/or end, trim those & update all params. */
{
trimRefAltDir(ref, alt, pStart, pEnd, pRefLen, pAltLen, FALSE);
}

void trimRefAltLeft(char *ref, char *alt, unsigned int *pStart, unsigned int *pEnd, int *pRefLen, int *pAltLen)
/* If ref and alt have identical bases at beginning and/or end, trim those starting on the right
 * so we get the leftmost representation & update all params. */
{
trimRefAltDir(ref, alt, pStart, pEnd, pRefLen, pAltLen, TRUE);
}

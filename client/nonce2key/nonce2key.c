//-----------------------------------------------------------------------------
// Merlok - June 2011
// Roel - Dec 2009
// Unknown author
//
// This code is licensed to you under the terms of the GNU GPL, version 2 or,
// at your option, any later version. See the LICENSE.txt file for the text of
// the license.
//-----------------------------------------------------------------------------
// MIFARE Darkside hack
//-----------------------------------------------------------------------------

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#define llx PRIx64

#include "nonce2key.h"
#include "mifarehost.h"
#include "ui.h"

int compar_state(const void * a, const void * b) {
	// didn't work: (the result is truncated to 32 bits)
	//return (*(int64_t*)b - *(int64_t*)a);

	// better:
	if (*(int64_t*)b == *(int64_t*)a) return 0;
	else if (*(int64_t*)b > *(int64_t*)a) return 1;
	else return -1;
}

int nonce2key(uint32_t uid, uint32_t nt, uint32_t nr, uint64_t par_info, uint64_t ks_info, uint64_t * key) {
  struct Crypto1State *state;
  uint32_t i, pos, rr, nr_diff, key_count;//, ks1, ks2;
  byte_t bt, ks3x[8], par[8][8];
  uint64_t key_recovered;
  int64_t *state_s;
  static uint32_t last_uid;
  static int64_t *last_keylist;
  rr = 0;
  
  if (last_uid != uid && last_keylist != NULL)
  {
	free(last_keylist);
	last_keylist = NULL;
  }
  last_uid = uid;

  // Reset the last three significant bits of the reader nonce
  nr &= 0xffffff1f;
  
  PrintAndLog("\nuid(%08x) nt(%08x) par(%016"llx") ks(%016"llx") nr(%08"llx")\n\n",uid,nt,par_info,ks_info,nr);

  for (pos=0; pos<8; pos++)
  {
    ks3x[7-pos] = (ks_info >> (pos*8)) & 0x0f;
    bt = (par_info >> (pos*8)) & 0xff;
    for (i=0; i<8; i++)
    {
      par[7-pos][i] = (bt >> i) & 0x01;
    }
  }

  printf("|diff|{nr}    |ks3|ks3^5|parity         |\n");
  printf("+----+--------+---+-----+---------------+\n");
  for (i=0; i<8; i++)
  {
    nr_diff = nr | i << 5;
    printf("| %02x |%08x|",i << 5, nr_diff);
    printf(" %01x |  %01x  |",ks3x[i], ks3x[i]^5);
    for (pos=0; pos<7; pos++) printf("%01x,", par[i][pos]);
    printf("%01x|\n", par[i][7]);
  }
  
	if (par_info==0)
		PrintAndLog("parity is all zero,try special attack!just wait for few more seconds...");
  
	state = lfsr_common_prefix(nr, rr, ks3x, par, par_info==0);
	state_s = (int64_t*)state;
	
	//char filename[50] ;
    //sprintf(filename, "nt_%08x_%d.txt", nt, nr);
    //printf("name %s\n", filename);
	//FILE* fp = fopen(filename,"w");
	for (i = 0; (state) && ((state + i)->odd != -1); i++)
	{
		lfsr_rollback_word(state+i, uid^nt, 0);
		crypto1_get_lfsr(state + i, &key_recovered);
		*(state_s + i) = key_recovered;
		//fprintf(fp, "%012llx\n",key_recovered);
	}
	//fclose(fp);
	
	if(!state)
		return 1;
	
	qsort(state_s, i, sizeof(*state_s), compar_state);
	*(state_s + i) = -1;
	
	//Create the intersection:
	if (par_info == 0 )
		if ( last_keylist != NULL)
		{
			int64_t *p1, *p2, *p3;
			p1 = p3 = last_keylist; 
			p2 = state_s;
			while ( *p1 != -1 && *p2 != -1 ) {
				if (compar_state(p1, p2) == 0) {
					printf("p1:%"llx" p2:%"llx" p3:%"llx" key:%012"llx"\n",(uint64_t)(p1-last_keylist),(uint64_t)(p2-state_s),(uint64_t)(p3-last_keylist),*p1);
					*p3++ = *p1++;
					p2++;
				}
				else {
					while (compar_state(p1, p2) == -1) ++p1;
					while (compar_state(p1, p2) == 1) ++p2;
				}
			}
			key_count = p3 - last_keylist;;
		}
		else
			key_count = 0;
	else
	{
		last_keylist = state_s;
		key_count = i;
	}
	
	printf("key_count:%d\n", key_count);

	// The list may still contain several key candidates. Test each of them with mfCheckKeys
	for (i = 0; i < key_count; i++) {
		uint8_t keyBlock[6];
		uint64_t key64;
		key64 = *(last_keylist + i);
		num_to_bytes(key64, 6, keyBlock);
		key64 = 0;
		if (!mfCheckKeys(0, 0, false, 1, keyBlock, &key64)) {
			*key = key64;
			free(last_keylist);
			last_keylist = NULL;
			if (par_info ==0)
				free(state);
			return 0;
		}
	}	

	
	free(last_keylist);
	last_keylist = state_s;
	
	return 1;
}

// 32 bit recover key from 2 nonces
bool mfkey32(nonces_t data, uint64_t *outputkey) {
	struct Crypto1State *s,*t;
	uint64_t outkey = 0;
	uint64_t key=0;     // recovered key
	uint32_t uid     = data.cuid;
	uint32_t nt      = data.nonce;  // first tag challenge (nonce)
	uint32_t nr0_enc = data.nr;  // first encrypted reader challenge
	uint32_t ar0_enc = data.ar;  // first encrypted reader response
	uint32_t nr1_enc = data.nr2; // second encrypted reader challenge
	uint32_t ar1_enc = data.ar2; // second encrypted reader response
	clock_t t1 = clock();
	bool isSuccess = FALSE;
	uint8_t counter=0;

	s = lfsr_recovery32(ar0_enc ^ prng_successor(nt, 64), 0);

	for(t = s; t->odd | t->even; ++t) {
		lfsr_rollback_word(t, 0, 0);
		lfsr_rollback_word(t, nr0_enc, 1);
		lfsr_rollback_word(t, uid ^ nt, 0);
		crypto1_get_lfsr(t, &key);
		crypto1_word(t, uid ^ nt, 0);
		crypto1_word(t, nr1_enc, 1);
		if (ar1_enc == (crypto1_word(t, 0, 0) ^ prng_successor(nt, 64))) {
			//PrintAndLog("Found Key: [%012"llx"]",key);
			outkey = key;
			counter++;
			if (counter==20) break;
		}
	}
	isSuccess = (counter == 1);
	t1 = clock() - t1;
	//if ( t1 > 0 ) PrintAndLog("Time in mfkey32: %.0f ticks \nFound %d possible keys", (float)t1, counter);
	*outputkey = ( isSuccess ) ? outkey : 0;
	crypto1_destroy(s);
	/* //un-comment to save all keys to a stats.txt file 
	FILE *fout;
	if ((fout = fopen("stats.txt","ab")) == NULL) { 
		PrintAndLog("Could not create file name stats.txt");
		return 1;
	}
	fprintf(fout, "mfkey32,%d,%08x,%d,%s,%04x%08x,%.0Lf\r\n", counter, data.cuid, data.sector, (data.keytype) ? "B" : "A", (uint32_t)(outkey>>32) & 0xFFFF,(uint32_t)(outkey&0xFFFFFFFF),(long double)t1);
	fclose(fout);
	*/
	return isSuccess;
}

bool tryMfk32_moebius(nonces_t data, uint64_t *outputkey) {
	struct Crypto1State *s, *t;
	uint64_t outkey  = 0;
	uint64_t key 	   = 0;			     // recovered key
	uint32_t uid     = data.cuid;
	uint32_t nt0     = data.nonce;  // first tag challenge (nonce)
	uint32_t nr0_enc = data.nr;  // first encrypted reader challenge
	uint32_t ar0_enc = data.ar; // first encrypted reader response
	uint32_t nt1     = data.nonce2; // second tag challenge (nonce)
	uint32_t nr1_enc = data.nr2; // second encrypted reader challenge
	uint32_t ar1_enc = data.ar2; // second encrypted reader response	
	bool isSuccess = FALSE;
	int counter = 0;
	
	//PrintAndLog("Enter mfkey32_moebius");
	clock_t t1 = clock();

	s = lfsr_recovery32(ar0_enc ^ prng_successor(nt0, 64), 0);
  
	for(t = s; t->odd | t->even; ++t) {
		lfsr_rollback_word(t, 0, 0);
		lfsr_rollback_word(t, nr0_enc, 1);
		lfsr_rollback_word(t, uid ^ nt0, 0);
		crypto1_get_lfsr(t, &key);
		
		crypto1_word(t, uid ^ nt1, 0);
		crypto1_word(t, nr1_enc, 1);
		if (ar1_enc == (crypto1_word(t, 0, 0) ^ prng_successor(nt1, 64))) {
			//PrintAndLog("Found Key: [%012"llx"]",key);
			outkey=key;
			++counter;
			if (counter==20)
				break;
		}
	}
	isSuccess	= (counter == 1);
	t1 = clock() - t1;
	//if ( t1 > 0 ) PrintAndLog("Time in mfkey32_moebius: %.0f ticks \nFound %d possible keys", (float)t1,counter);
	*outputkey = ( isSuccess ) ? outkey : 0;
	crypto1_destroy(s);
	/* // un-comment to output all keys to stats.txt
	FILE *fout;
	if ((fout = fopen("stats.txt","ab")) == NULL) { 
		PrintAndLog("Could not create file name stats.txt");
		return 1;
	}
	fprintf(fout, "moebius,%d,%08x,%d,%s,%04x%08x,%0.Lf\r\n", counter, data.cuid, data.sector, (data.keytype) ? "B" : "A", (uint32_t) (outkey>>32),(uint32_t)(outkey&0xFFFFFFFF),(long double)t1);
	fclose(fout);
	*/
	return isSuccess;
}

int tryMfk64_ex(uint8_t *data, uint64_t *outputkey){
	uint32_t uid    = le32toh(data);
	uint32_t nt     = le32toh(data+4);  // tag challenge
	uint32_t nr_enc = le32toh(data+8);  // encrypted reader challenge
	uint32_t ar_enc = le32toh(data+12); // encrypted reader response	
	uint32_t at_enc = le32toh(data+16);	// encrypted tag response
	return tryMfk64(uid, nt, nr_enc, ar_enc, at_enc, outputkey);
}

int tryMfk64(uint32_t uid, uint32_t nt, uint32_t nr_enc, uint32_t ar_enc, uint32_t at_enc, uint64_t *outputkey){
	uint64_t key 	= 0;				// recovered key
	uint32_t ks2;     					// keystream used to encrypt reader response
	uint32_t ks3;     					// keystream used to encrypt tag response
	struct Crypto1State *revstate;
	
	PrintAndLog("Enter mfkey64");
	clock_t t1 = clock();
	
	// Extract the keystream from the messages
	ks2 = ar_enc ^ prng_successor(nt, 64);
	ks3 = at_enc ^ prng_successor(nt, 96);
	revstate = lfsr_recovery64(ks2, ks3);
	lfsr_rollback_word(revstate, 0, 0);
	lfsr_rollback_word(revstate, 0, 0);
	lfsr_rollback_word(revstate, nr_enc, 1);
	lfsr_rollback_word(revstate, uid ^ nt, 0);
	crypto1_get_lfsr(revstate, &key);
	PrintAndLog("Found Key: [%012"llx"]", key);
	crypto1_destroy(revstate);
	*outputkey = key;
	
	t1 = clock() - t1;
	if ( t1 > 0 ) PrintAndLog("Time in mfkey64: %.0f ticks \n", (float)t1);
	return 0;
}


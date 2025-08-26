#include <hls_stream.h>
#include "sha256_util.hpp"
#include "sha256.hpp"
//#include "loop.hpp"
//#include "blocks_loop_4.hpp"
#include "compress_blocks.hpp"


void init_state(uint internal_state[]){
	internal_state[0] = 0x6a09e667;
	internal_state[1] = 0xbb67ae85;
	internal_state[2] = 0x3c6ef372;
	internal_state[3] = 0xa54ff53a;
	internal_state[4] = 0x510e527f;
	internal_state[5] = 0x9b05688c;
	internal_state[6] = 0x1f83d9ab;
	internal_state[7] = 0x5be0cd19;
}


//void compress_data(wordChannels &wordStream, hls::stream< axis> &hash){
void compress_data(wordChannels &wordStream, uint internal_state[8]){
#pragma HLS PIPELINE
	uint i=0;
	uint s[8];
	//uint internal_state[8];
	ap_uint<BIT_WCH> ch=0;

#pragma HLS ARRAY_RESHAPE variable=internal_state type=complete

	init_state(internal_state);

	COMPRESS_LOOP_:
	while( !wordStream.done.read() ){
#pragma HLS PIPELINE off
#pragma HLS LOOP_TRIPCOUNT min=1 max=1

		s[0]  = internal_state[0];	//a
		s[1]  = internal_state[1];	//b
		s[2]  = internal_state[2];	//c
		s[3]  = internal_state[3];	//d
		s[4]  = internal_state[4];	//e
		s[5]  = internal_state[5];	//f
		s[6]  = internal_state[6];	//g
		s[7]  = internal_state[7];	//h

		//iter_64(wordStream,s);

		for(int i=0;i<16;i++){
//#pragma HLS UNROLL off
			compress_4(wordStream,s, ch);
		}

		internal_state[0] += s[0];
		internal_state[1] += s[1];
		internal_state[2] += s[2];
		internal_state[3] += s[3];
		internal_state[4] += s[4];
		internal_state[5] += s[5];
		internal_state[6] += s[6];
		internal_state[7] += s[7];
	}

/*
	axis out;
	out.last=0;
	out.keep=-1;
	ENDIAN_LOOP_:
	for(i=0; i < 8; ++i){
#pragma HLS PIPELINE off
		out.data = (internal_state[i] >> 24) & 0x000000ff;
		hash << out;
		out.data = (internal_state[i] >> 16) & 0x000000ff;
		hash << out;
		out.data = (internal_state[i] >> 8) & 0x000000ff;
		hash << out;
		out.data = (internal_state[i]) & 0x000000ff;
		if(i==7)out.last=1;
		hash << out;
	}
*/
}


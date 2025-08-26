#include "sha256.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "round_data.hpp"
#include "compress_data.hpp"

//#include "flow_sha256_2.hpp"


/*
 * -------------------------------------------------------------------------------------
 * VERSIONE USATA ----------------------------------------------------------------------
 * -------------------------------------------------------------------------------------
 */
void read_data(hls::stream<axis> &data, dataChannels &outStream){
//#pragma HLS PIPELINE


	uint i;
	axis d;
	uint len=0;
	ap_uint<64> bitlen;
	uchar last_blk;
	ap_uint<6> from=0; //contatore 6 bit (mod 64)
	ap_uint<BIT_DCH> ch_dc=0;

	READ_LOOP_:
	while(data.read().data){//Leftover data?
#pragma HLS LOOP_TRIPCOUNT min=1 max=1


		while(true)
		{
			//#pragma HLS PIPELINE
//#pragma HLS PIPELINE off
#pragma HLS LOOP_TRIPCOUNT min=8 max=8
			if(!from) outStream.done << 0;
			d=data.read();
			outStream.channel[ch_dc++]<<d.data;

			from++;
			len++;

			if(d.last)break;
		}
	}


	if(!from) outStream.done << 0;
	outStream.channel[ch_dc++] << 0x80;


	from++;

	uint to = (from<56)?56:120;

	PADDING_LOOP_:
	for(i=from; i<to;i++){
		//#pragma HLS PIPELINE
//#pragma HLS PIPELINE off
#pragma HLS LOOP_TRIPCOUNT min=47 max=47
		if(i==64) outStream.done << 0;
		outStream.channel[ch_dc++] << 0x00;
	}
	bitlen=len*8;
	outStream.channel[ch_dc++] << ((bitlen >> 56)&0x000000ff);
	outStream.channel[ch_dc++] << ((bitlen >> 48)&0x000000ff);
	outStream.channel[ch_dc++] << ((bitlen >> 40)&0x000000ff);
	outStream.channel[ch_dc++] << ((bitlen >> 32)&0x000000ff);
	outStream.channel[ch_dc++] << ((bitlen >> 24)&0x000000ff);
	outStream.channel[ch_dc++] << ((bitlen >> 16)&0x000000ff);
	outStream.channel[ch_dc++] << ((bitlen >> 8)&0x000000ff);
	outStream.channel[ch_dc++] << ((bitlen)&0x000000ff);
	outStream.done << 1;

}


/*
 * -------------------------------------------------------------------------------------
 * ALTERNATIVA 1 -----------------------------------------------------------------------
 * -------------------------------------------------------------------------------------
 */
void pre_concat(uint &w, ap_uint<8> d, uint i)
{
	w[0+8*i]=d[0];
	w[1+8*i]=d[1];
	w[2+8*i]=d[2];
	w[3+8*i]=d[3];
	w[4+8*i]=d[4];
	w[5+8*i]=d[5];
	w[6+8*i]=d[6];
	w[7+8*i]=d[7];
}
void read_data_pre_concat(hls::stream<axis> &data, wordConcatChannels &outStream){
//#pragma HLS PIPELINE


	uint i;
	axis d;
	uint len=0;
	ap_uint<64> bitlen;
	uchar last_blk;
	ap_uint<6> from=0; //contatore 6 bit (mod 64)
	ap_uint<2> c_pos=0;
	ap_uint<BIT_DCH> ch_dc=0;
	uint word;

	READ_LOOP_:
	while(data.read().data){//Leftover data?

#pragma HLS LOOP_TRIPCOUNT min=1 max=1


		while(true)
		{
#pragma HLS PIPELINE
//#pragma HLS UNROLL factor=64
#pragma HLS LOOP_TRIPCOUNT min=8 max=8
			if(!from) outStream.done << 0;
			d=data.read();
			pre_concat(word,d.data,c_pos++);
			if(!c_pos) outStream.channel[ch_dc++]<<word;

			from++;
			len++;

			if(d.last)break;
		}
	}


	if(!from) outStream.done << 0;
	pre_concat(word,0x80,c_pos++);
	if(!c_pos) outStream.channel[ch_dc++]<<word;


	from++;

	uint to = (from<56)?56:120;

	PADDING_LOOP_:
	for(i=from; i<to;i++){
		//#pragma HLS PIPELINE
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=47 max=47
		if(i==64) outStream.done << 0;
		pre_concat(word,0x00,c_pos++);
		if(!c_pos) outStream.channel[ch_dc++]<<word;
	}
	bitlen=len*8;

	pre_concat(word,((bitlen >> 56)&0x000000ff),c_pos++);
	pre_concat(word,((bitlen >> 48)&0x000000ff),c_pos++);
	pre_concat(word,((bitlen >> 40)&0x000000ff),c_pos++);
	pre_concat(word,((bitlen >> 32)&0x000000ff),c_pos++);
	outStream.channel[ch_dc++]<<word;
	pre_concat(word,((bitlen >> 24)&0x000000ff),c_pos++);
	pre_concat(word,((bitlen >> 16)&0x000000ff),c_pos++);
	pre_concat(word,((bitlen >> 8)&0x000000ff),c_pos++);
	pre_concat(word,((bitlen)&0x000000ff),c_pos++);
	outStream.channel[ch_dc++]<<word;

	outStream.done << 1;

}

void endian(uint internal_state[8], hls::stream<axis> &hash){
	axis out;
	out.last=0;
	out.keep=-1;
	ENDIAN_LOOP_:
	for(uint i=0; i < 8; ++i){
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
}


void endian(uint internal_state[8], uint hash[32]){
	ENDIAN_LOOP_:
	for(uint i=0; i < 8; ++i){
#pragma HLS PIPELINE off
		hash[4*i] = internal_state[i].range(31,24);//(internal_state[i] >> 24) & 0x000000ff;

		hash[4*i+1] = internal_state[i].range(24,16);//(internal_state[i] >> 16) & 0x000000ff;

		hash[4*i+2] = internal_state[i].range(16,8);//(internal_state[i] >> 8) & 0x000000ff;

		hash[4*i+3] = internal_state[i].range(7,0);//(internal_state[i]) & 0x000000ff;
		//if(i==7)out.last=1;
	}
}


/*
 * -------------------------------------------------------------------------------------
 * TOP FUNCTION ------------------------------------------------------------------------
 * -------------------------------------------------------------------------------------
 */
void sha256(hls::stream< axis> &input, hls::stream<axis> &hash)
{
#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=hash
#pragma HLS INTERFACE s_axilite port=return

	dataChannels dataStream;
	wordChannels wStream;
	uint internal_state[8];
#pragma HLS DATAFLOW //disable_start_propagation

	read_data(input, dataStream);

	round_data_shift(dataStream, wStream);

	compress_data(wStream, internal_state);

	endian(internal_state, hash);

}


void sha256_alternativa_1(hls::stream< axis> &input, hls::stream<axis> &hash)
{
#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=hash
#pragma HLS INTERFACE s_axilite port=return



	wordConcatChannels dataStream;
	wordChannels wStream;
	uint internal_state[8];
#pragma HLS DATAFLOW //disable_start_propagation

	read_data_pre_concat(input, dataStream);

	round_data_shift_pre_concat(dataStream, wStream);

	compress_data(wStream, internal_state);

	endian(internal_state, hash);;


}


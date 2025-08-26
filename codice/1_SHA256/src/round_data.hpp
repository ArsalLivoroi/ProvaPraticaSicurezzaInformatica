#include <hls_stream.h>
#include "sha256_util.hpp"
#include "sha256.hpp"

uint concat(ap_uint<8> d0, ap_uint<8> d1, ap_uint<8> d2, ap_uint<8> d3)
{
	uint wi;
	wi[0]=d3[0];
	wi[1]=d3[1];
	wi[2]=d3[2];
	wi[3]=d3[3];
	wi[4]=d3[4];
	wi[5]=d3[5];
	wi[6]=d3[6];
	wi[7]=d3[7];
	wi[8]=d2[0];
	wi[9]=d2[1];
	wi[10]=d2[2];
	wi[11]=d2[3];
	wi[12]=d2[4];
	wi[13]=d2[5];
	wi[14]=d2[6];
	wi[15]=d2[7];
	wi[16]=d1[0];
	wi[17]=d1[1];
	wi[18]=d1[2];
	wi[19]=d1[3];
	wi[20]=d1[4];
	wi[21]=d1[5];
	wi[22]=d1[6];
	wi[23]=d1[7];
	wi[24]=d0[0];
	wi[25]=d0[1];
	wi[26]=d0[2];
	wi[27]=d0[3];
	wi[28]=d0[4];
	wi[29]=d0[5];
	wi[30]=d0[6];
	wi[31]=d0[7];
	return wi;
}


uint shift_reg(uint w_shift[], uint tmp_w)
{
#pragma HLS INLINE
	//uint res=w_shift[0];
	//tmp_w = sig1(w_shift[14]) + w_shift[9] + sig0(w_shift[1]) + w_shift[0];
	w_shift[0]=w_shift[1];
	w_shift[1]=w_shift[2];
	w_shift[2]=w_shift[3];
	w_shift[3]=w_shift[4];
	w_shift[4]=w_shift[5];
	w_shift[5]=w_shift[6];
	w_shift[6]=w_shift[7];
	w_shift[7]=w_shift[8];
	w_shift[8]=w_shift[9];
	w_shift[9]=w_shift[10];
	w_shift[10]=w_shift[11];
	w_shift[11]=w_shift[12];
	w_shift[12]=w_shift[13];
	w_shift[13]=w_shift[14];
	w_shift[14]=w_shift[15];
	w_shift[15]=tmp_w;
	return tmp_w;
}

/*
 * -------------------------------------------------------------------------------------
 * VERSIONE USATA ----------------------------------------------------------------------
 * -------------------------------------------------------------------------------------
 */
void round_data_shift( dataChannels &dataStream,  wordChannels &w){
	//#pragma HLS PIPELINE
	mybit done=0;
	ap_uint<BIT_WCH> w_ch=0;
	ap_uint<BIT_DCH> d_ch=0;
	//ap_uint<6> i_mod64;
	ap_uint<8> i=0;
	uint tmp_w;
	uint w_shift[16];
#pragma HLS ARRAY_PARTITION variable=w_shift complete dim=0

	ROUND_LOOP_:
	while(true)
	{
#pragma HLS PIPELINE off
#pragma HLS LOOP_TRIPCOUNT min=1 max=1

		if(dataStream.done.read()){
			w.done<<1;
			break;
		}
		w.done<<0;

		for(i=0;i<16;i++){
			#pragma HLS PIPELINE
//#pragma HLS UNROLL
			tmp_w=CONCAT(dataStream.channel[d_ch++].read(),dataStream.channel[d_ch++].read(),dataStream.channel[d_ch++].read(),dataStream.channel[d_ch++].read());
			w.channel[w_ch++]<<(shift_reg(w_shift, tmp_w)+Ki[i]);
		}
		for(;i<64;i++){
#pragma HLS PIPELINE rewind
			tmp_w = sig1(w_shift[14]) + w_shift[9] + sig0(w_shift[1]) + w_shift[0];
			w.channel[w_ch++]<<(shift_reg(w_shift, tmp_w)+Ki[i]);
		}

	}
}



/*
 * -------------------------------------------------------------------------------------
 * ALTERNATIVA 1 -----------------------------------------------------------------------
 * -------------------------------------------------------------------------------------
 */
void round_data_shift_pre_concat( wordConcatChannels &dataStream,  wordChannels &w){
	//#pragma HLS PIPELINE
	mybit done=0;
	ap_uint<BIT_WCH> w_ch=0;
	ap_uint<BIT_DCH> d_ch=0;
	//ap_uint<6> i_mod64;
	ap_uint<8> i=0;
	uint tmp_w;
	uint w_shift[16];
#pragma HLS ARRAY_PARTITION variable=w_shift complete dim=0

	ROUND_LOOP_:
	while(true)
	{
#pragma HLS PIPELINE off
#pragma HLS LOOP_TRIPCOUNT min=1 max=1

		if(dataStream.done.read()){
			w.done<<1;
			break;
		}
		w.done<<0;

		for(i=0;i<16;i++){
#pragma HLS PIPELINE
#pragma HLS UNROLL factor=16
			tmp_w=dataStream.channel[d_ch++].read();
			w.channel[w_ch++]<<(shift_reg(w_shift, tmp_w)+Ki[i]);
		}
		for(;i<64;i++){
#pragma HLS PIPELINE
#pragma HLS UNROLL factor=4
			tmp_w = sig1(w_shift[14]) + w_shift[9] + sig0(w_shift[1]) + w_shift[0];
			w.channel[w_ch++]<<(shift_reg(w_shift, tmp_w)+Ki[i]);
		}

	}
}


/*
 * -------------------------------------------------------------------------------------
 * ALTERNATIVA 2 -----------------------------------------------------------------------
 * -------------------------------------------------------------------------------------
 */
void round_data_shift2( dataChannels &dataStream,  wordChannels &w){
	//#pragma HLS PIPELINE
	mybit done=0;
	ap_uint<BIT_WCH> w_ch=0;
	ap_uint<BIT_DCH> d_ch=0;
	//ap_uint<6> i_mod64;
	ap_uint<8> i=0;
	uint tmp_w;
	uint w_shift[16];
#pragma HLS ARRAY_PARTITION variable=w_shift complete dim=0

	ROUND_LOOP_:
	while(true)
	{
#pragma HLS PIPELINE off
#pragma HLS LOOP_TRIPCOUNT min=1 max=1

		if(dataStream.done.read()){
			w.done<<1;
			break;
		}
		w.done<<0;

		for(i=0;i<64;i++){
			//#pragma HLS PIPELINE
			if(i<16)
				tmp_w=CONCAT(dataStream.channel[d_ch++].read(),dataStream.channel[d_ch++].read(),dataStream.channel[d_ch++].read(),dataStream.channel[d_ch++].read());
			else
				tmp_w = sig1(w_shift[14]) + w_shift[9] + sig0(w_shift[1]) + w_shift[0];
			w.channel[w_ch++]<<(shift_reg(w_shift, tmp_w)+Ki[i]);
		}
	}
}


/*
 * -------------------------------------------------------------------------------------
 * ALTERNATIVA 3 -----------------------------------------------------------------------
 * -------------------------------------------------------------------------------------
 */
void round_data_buffer( dataChannels &dataStream,  wordChannels &w){
	//#pragma HLS PIPELINE off
	mybit done=0;
	ap_uint<2> ch=0;
	ap_uint<7> i_mod64=0;
	uint tmp_w;
	uint w_buff[64];
#pragma HLS ARRAY_PARTITION variable=w_buff complete dim=0

	ROUND_LOOP_:
	while(true)
	{
#pragma HLS PIPELINE off
#pragma HLS LOOP_TRIPCOUNT min=64 max=64

		if(dataStream.done.read()){
			w.done<<1;
			break;
		}
		w.done<<0;

		for( i_mod64==0; i_mod64<16; i_mod64++ ){
#pragma HLS PIPELINE off

			tmp_w=CONCAT(dataStream.channel[0].read(),dataStream.channel[1].read(),dataStream.channel[2].read(),dataStream.channel[3].read());

			w_buff[i_mod64]=tmp_w;
			w.channel[ch++]<<tmp_w;
		}
		for( ; i_mod64<64; i_mod64++ ){
#pragma HLS PIPELINE off

			tmp_w = sig1(w_buff[i_mod64-2]) + w_buff[i_mod64-7] + sig0(w_buff[i_mod64-15]) + w_buff[i_mod64-16];

			w_buff[i_mod64]=tmp_w;
			w.channel[ch++]<<tmp_w;
		}
	}

}

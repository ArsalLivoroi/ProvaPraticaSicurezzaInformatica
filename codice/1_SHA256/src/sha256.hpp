#ifndef _sha256hpp
#define _sha256hpp

#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"


#define N_DCH 4
#define BIT_DCH 2

#define N_WCH 4
#define BIT_WCH 2


/*
#define N_DCH 4
#define BIT_DCH 2

#define N_WCH 4
#define BIT_WCH 2
*/


#define axis  ap_axiu<8,0,0,0>
typedef ap_uint<1> mybit;
#define uchar unsigned char // 8-bit char
#define uint ap_uint<32> //unsigned int // 32-bit word

typedef struct dataChannels{
	hls::stream<uchar, 64> channel[N_DCH];
	hls::stream<mybit> done;
};

typedef struct wordConcatChannels{
	hls::stream<ap_uint<32>,8> channel[N_DCH];
	hls::stream<mybit> done;
};

typedef struct wordChannels{
	hls::stream<ap_uint<32>,8> channel[N_WCH];
	hls::stream<mybit> done;
};

void sha256(hls::stream<axis > &input, hls::stream< axis> &output);

void read(hls::stream<axis> &data, dataChannels &outStream);

#endif

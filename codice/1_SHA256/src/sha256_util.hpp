#ifndef _sha256_util_hpp
#define _sha256_util_hpp

#define CONCAT(d0,d1,d2,d3) ((d0<<24) | (d1<<16) | (d2<<8) | (d3))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))


static const uint Ki[64] = {
		0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
		0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
		0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
		0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
		0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
		0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

void concat(uint &wi, ap_uint<8> d0, ap_uint<8> d1, ap_uint<8> d2, ap_uint<8> d3)
{
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

}



uint ep0(uint a)
{
//#pragma HLS PIPELINE off
uint res;
res[31]=a[12] ^ a[1] ^ a[21];
res[30]=a[0] ^ a[11] ^ a[20];
res[29]=a[10] ^ a[19] ^ a[31];
res[28]=a[18] ^ a[30] ^ a[9];
res[27]=a[17] ^ a[29] ^ a[8];
res[26]=a[16] ^ a[28] ^ a[7];
res[25]=a[15] ^ a[27] ^ a[6];
res[24]=a[14] ^ a[26] ^ a[5];
res[23]=a[13] ^ a[25] ^ a[4];
res[22]=a[12] ^ a[24] ^ a[3];
res[21]=a[11] ^ a[23] ^ a[2];
res[20]=a[10] ^ a[1] ^ a[22];
res[19]=a[0] ^ a[21] ^ a[9];
res[18]=a[20] ^ a[31] ^ a[8];
res[17]=a[19] ^ a[30] ^ a[7];
res[16]=a[18] ^ a[29] ^ a[6];
res[15]=a[17] ^ a[28] ^ a[5];
res[14]=a[16] ^ a[27] ^ a[4];
res[13]=a[15] ^ a[26] ^ a[3];
res[12]=a[14] ^ a[25] ^ a[2];
res[11]=a[13] ^ a[1] ^ a[24];
res[10]=a[0] ^ a[12] ^ a[23];
res[9]=a[11] ^ a[22] ^ a[31];
res[8]=a[10] ^ a[21] ^ a[30];
res[7]=a[20] ^ a[29] ^ a[9];
res[6]=a[19] ^ a[28] ^ a[8];
res[5]=a[18] ^ a[27] ^ a[7];
res[4]=a[17] ^ a[26] ^ a[6];
res[3]=a[16] ^ a[25] ^ a[5];
res[2]=a[15] ^ a[24] ^ a[4];
res[1]=a[14] ^ a[23] ^ a[3];
res[0]=a[13] ^ a[22] ^ a[2];
return res;
}
uint ep1(uint e)
{
//#pragma HLS PIPELINE off
uint res;
res[31]=e[10] ^ e[24] ^ e[5];
res[30]=e[23] ^ e[4] ^ e[9];
res[29]=e[22] ^ e[3] ^ e[8];
res[28]=e[21] ^ e[2] ^ e[7];
res[27]=e[1] ^ e[20] ^ e[6];
res[26]=e[0] ^ e[19] ^ e[5];
res[25]=e[18] ^ e[31] ^ e[4];
res[24]=e[17] ^ e[30] ^ e[3];
res[23]=e[16] ^ e[29] ^ e[2];
res[22]=e[15] ^ e[1] ^ e[28];
res[21]=e[0] ^ e[14] ^ e[27];
res[20]=e[13] ^ e[26] ^ e[31];
res[19]=e[12] ^ e[25] ^ e[30];
res[18]=e[11] ^ e[24] ^ e[29];
res[17]=e[10] ^ e[23] ^ e[28];
res[16]=e[22] ^ e[27] ^ e[9];
res[15]=e[21] ^ e[26] ^ e[8];
res[14]=e[20] ^ e[25] ^ e[7];
res[13]=e[19] ^ e[24] ^ e[6];
res[12]=e[18] ^ e[23] ^ e[5];
res[11]=e[17] ^ e[22] ^ e[4];
res[10]=e[16] ^ e[21] ^ e[3];
res[9]=e[15] ^ e[20] ^ e[2];
res[8]=e[14] ^ e[19] ^ e[1];
res[7]=e[0] ^ e[13] ^ e[18];
res[6]=e[12] ^ e[17] ^ e[31];
res[5]=e[11] ^ e[16] ^ e[30];
res[4]=e[10] ^ e[15] ^ e[29];
res[3]=e[14] ^ e[28] ^ e[9];
res[2]=e[13] ^ e[27] ^ e[8];
res[1]=e[12] ^ e[26] ^ e[7];
res[0]=e[11] ^ e[25] ^ e[6];
return res;
}

uint sig1(uint w)
{
	//return SIG1(w);

	uint res;
	res[31]=w[16] ^ w[18];
	res[30]=w[15] ^ w[17];
	res[29]=w[14] ^ w[16];
	res[28]=w[13] ^ w[15];
	res[27]=w[12] ^ w[14];
	res[26]=w[11] ^ w[13];
	res[25]=w[10] ^ w[12];
	res[24]=w[11] ^ w[9];
	res[23]=w[10] ^ w[8];
	res[22]=w[7] ^ w[9];
	res[21]=w[31] ^ w[6] ^ w[8];
	res[20]=w[30] ^ w[5] ^ w[7];
	res[19]=w[29] ^ w[4] ^ w[6];
	res[18]=w[28] ^ w[3] ^ w[5];
	res[17]=w[27] ^ w[2] ^ w[4];
	res[16]=w[1] ^ w[26] ^ w[3];
	res[15]=w[0] ^ w[25] ^ w[2];
	res[14]=w[1] ^ w[24] ^ w[31];
	res[13]=w[0] ^ w[23] ^ w[30];
	res[12]=w[22] ^ w[29] ^ w[31];
	res[11]=w[21] ^ w[28] ^ w[30];
	res[10]=w[20] ^ w[27] ^ w[29];
	res[9]=w[19] ^ w[26] ^ w[28];
	res[8]=w[18] ^ w[25] ^ w[27];
	res[7]=w[17] ^ w[24] ^ w[26];
	res[6]=w[16] ^ w[23] ^ w[25];
	res[5]=w[15] ^ w[22] ^ w[24];
	res[4]=w[14] ^ w[21] ^ w[23];
	res[3]=w[13] ^ w[20] ^ w[22];
	res[2]=w[12] ^ w[19] ^ w[21];
	res[1]=w[11] ^ w[18] ^ w[20];
	res[0]=w[10] ^ w[17] ^ w[19];
	return res;

}

uint sig0(uint w)
{
	//return SIG0(w);

	uint res;
	res[31]=w[17] ^ w[6];
	res[30]=w[16] ^ w[5];
	res[29]=w[15] ^ w[4];
	res[28]=w[14] ^ w[31] ^ w[3];
	res[27]=w[13] ^ w[2] ^ w[30];
	res[26]=w[12] ^ w[1] ^ w[29];
	res[25]=w[0] ^ w[11] ^ w[28];
	res[24]=w[10] ^ w[27] ^ w[31];
	res[23]=w[26] ^ w[30] ^ w[9];
	res[22]=w[25] ^ w[29] ^ w[8];
	res[21]=w[24] ^ w[28] ^ w[7];
	res[20]=w[23] ^ w[27] ^ w[6];
	res[19]=w[22] ^ w[26] ^ w[5];
	res[18]=w[21] ^ w[25] ^ w[4];
	res[17]=w[20] ^ w[24] ^ w[3];
	res[16]=w[19] ^ w[23] ^ w[2];
	res[15]=w[18] ^ w[1] ^ w[22];
	res[14]=w[0] ^ w[17] ^ w[21];
	res[13]=w[16] ^ w[20] ^ w[31];
	res[12]=w[15] ^ w[19] ^ w[30];
	res[11]=w[14] ^ w[18] ^ w[29];
	res[10]=w[13] ^ w[17] ^ w[28];
	res[9]=w[12] ^ w[16] ^ w[27];
	res[8]=w[11] ^ w[15] ^ w[26];
	res[7]=w[10] ^ w[14] ^ w[25];
	res[6]=w[13] ^ w[24] ^ w[9];
	res[5]=w[12] ^ w[23] ^ w[8];
	res[4]=w[11] ^ w[22] ^ w[7];
	res[3]=w[10] ^ w[21] ^ w[6];
	res[2]=w[20] ^ w[5] ^ w[9];
	res[1]=w[19] ^ w[4] ^ w[8];
	res[0]=w[18] ^ w[3] ^ w[7];
	return res;

}


#endif


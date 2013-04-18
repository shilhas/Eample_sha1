/*
 * sha_1.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: sradhakr
 */


#include "Std_Types.h"

uint32 h0 = 0x67452301U;
uint32 h1 = 0xEFCDAB89U;
uint32 h2 = 0x98BADCFEU;
uint32 h3 = 0x10325476U;
uint32 h4 = 0xC3D2E1F0U;


uint32  sha_1(uint8 *msg, uint64 len)
{
	/*length is assumed to be in bits*/
	uint16 k = 0;
	uint8 m[64];
	uint16 s_len = 0;
	uint16 cyc_rep = 0;

	/*convert the length to bytes*/
	len = len >> 3;
	if(len > 64)
	{
		/*divide length into 512 bit chunks*/
		cyc_rep = (len >> 9) + 1;
	}
	else if((len > 56) && (len < 65))
	{
		cyc_rep = 2;
	}

	/*loop for each chunk*/
	for(uint16 j = 0; j < cyc_rep; j++)
	{
		uint16 i;

		/*Check if it is the last repeatation*/
		if(j == (cyc_rep - 1))
		{
			/*Get lenght of last repeatation*/
			s_len = len & 0x3F;
			m[s_len] = 0x80;     /*append 1*/


			/*calculate k in bytes*/
			/*if (s_len < 56)
			{*/
				k = ((55 - s_len) & 0x3F);
				/*
			}
			else
			{
				k = (120 - s_len)
			}*/
			for(uint8 n = 0; n < k; n++)
			{
				m[s_len + n + 1] = 0;
			}
		}
		else
		{
			/*not last repeatation hence set the lenght as 512*/
			s_len = 512;
		}

		for (i = 0; i < (s_len >> 3); i++)
		{
			m[i] = (uint8)(*msg++);
		}


	}



	return (uint32*)&k;
}


void calculatehash(uint8 *msg)
{
	uint32 a,b,c,d,e,f,k;
	uint32 temp;
	uint32 temp1;
	uint8 i;
	for(i = 16; i < 80; i++)
	{
		msg[i] = (msg[i - 3] ^ msg[i - 8] ^ msg[i - 14] ^ msg[i - 16] );
		temp = (msg[i] & 0x80000000) >> 31;
		msg[i] = (msg[i] << 1) & temp;
	}

	/*Set Initial values*/
	a = h0;
	b = h1;
	c = h2;
	d = h3;
	e = h4;

	for(i = 0; i< 80 ;i++)
	{
		if(i < 20)
		{
			f = (b & c) | ((!b) & d);
			k = 0x5A827999U;
		}
		else if((i > 19)&&(i < 40))
		{
			f = b ^ c ^ d;
			k = 0x6ED9EBA1U;
		}
		else if((i > 39)&&(i < 60))
		{
			f = ((b & c) | (b & d) | (c & d));
			k = 0x8F1BBCDCU;

		}
		else
		{
			f = b ^ c ^ d;
			k = 0xCA62C1D6U;
		}

		temp = (a & 0xF8000000U) >> 27;
		temp1 = ((a << 5) & temp) + f + e + k + msg[i];
		e = d;
		d = c;
		temp = ((b & 0xFFFFFFFCU)>>2);
		c = (b << 30) & temp;
		b = a;
		a = temp1;
	}

	h0 = h0 + a;
	h1 = h1 + b;
	h2 = h2 + c;
	h3 = h3 + d;
	h4 = h4 + e;

}


int main()
{
	uint8 a[27] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
	uint32 msg_d;
	msg_d = sha_1(&a[0],480);
	return 0;
}







/*
 * sha_1.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: sradhakr
 */


#include "Std_Types.h"
#include "sha_1_cfg.h"

/*Global Variable definition*/

#if (BIG_ENDIAN == TRUE)
uint32 h0 = 0x67452301U;
uint32 h1 = 0xEFCDAB89U;
uint32 h2 = 0x98BADCFEU;
uint32 h3 = 0x10325476U;
uint32 h4 = 0xC3D2E1F0U;
#else
uint32 h0 = 0x01234567U;
uint32 h1 = 0x89ABCDEFU;
uint32 h2 = 0xFEDCBA98U;
uint32 h3 = 0x76543210U;
uint32 h4 = 0xF0E1D2C3U;
#endif

static bool mid_size = FALSE;
static uint8 m[64] = {0};
static uint16 cyc_rep;

/*End Global Variable Definition*/

/*Static Function Declaration*/

static void creat_firstblock_midsize(uint8 *msg, uint16 len);
static uint32  sha_1(uint8 *msg, uint64 len);
static void calculatehash(uint8 *msg);
static void conv_msgbyte_to_word(uint8 *msg,uint8 * msg_32);

/*End Static Function Declaration*/

///////////////Block Manipulation Section/////////////////////

static void creat_firstblock_midsize(uint8 *msg, uint16 len)
{
	uint8 x;
	mid_size = TRUE;
	/*Create first block of mid size block*/
	/*Append 1*/
	m[len] = 0x80;
	/*Fill 0 in remaining bytes since length cannot be appended*/
	for(x = 0; x < (63 - len);x++)
	{
		m[len + x + 1] = 0x00U;
	}
	cyc_rep = 2;
}

#if (BIG_ENDIAN == FALSE)
/*can convert data up to length 65535*/
/*NOTE:message buffer size should be multiple of 4bytes else during append operation there may be problem*/
static void conv_Little2Big_Endian(uint8 *msg, uint16 length)
{
	uint8 i;
	uint8 j;
	uint8 temp;
	//uint8 rep_val;

	for (i = 0; i < (length - 3); i+=4)
	{
		if((length - i) < 4)
		{
			//append 0 to make length multiple of 4
			for(j = 0;j < (4 - (length - i));j++)
			{
				msg[length + j] = 0;
			}
		}

		for(j = 4; j > 0; j--)
		{
			temp = msg[i + (4 - j)];
			msg[i + (4 - j)] =  msg[i+j-1];
			msg[i+j-1] = temp;
		}
	}
}
#endif


static uint32  sha_1(uint8 *msg, uint64 len)
{
	uint16 k = 0;
	uint16 s_len = 0;
	uint8 x;
	uint16 i;

	/*convert the length to bytes*/
#if (LEN_IN_BIT == TRUE)
	len = len >> 3;
#endif

	/*Check if the block needs to be divided, if the length is > 55 then it would
	 * be necessary to divide the block*/
	if(len > 64)
	{
		/*divide length into 512 bit chunks*/
		cyc_rep = (len >> 6) + 1;
	}
	else if((len > 56) && (len < 65))
	{
		creat_firstblock_midsize(msg, (uint16)len);
	}
	else
	{
		cyc_rep = 1;
	}

	/*loop for each chunk*/
	for(uint16 j = 0; j < cyc_rep; j++)
	{

		/*Check if it is the last repetition*/
		if((j == (cyc_rep - 1))&&(FALSE == mid_size))
		{
			/*Check if the last block size is > 55 and < 64*/
			if((len & 0x3f) > 55)
			{
				creat_firstblock_midsize(msg,(uint16)len);
			}
			else
			{
				/*Get length of last repetition*/
				s_len = len & 0x3F;
#if (BIG_ENDIAN == FALSE)
				//TODO:Append bit '1' for little endian machine.
				conv_Little2Big_Endian(m,s_len);
#endif
				m[s_len] = 0x80;     /*append 1*/
				k = ((55 - s_len) & 0x3F);

				for(uint8 n = 0; n < k; n++)
				{
					m[s_len + n + 1] = 0;
				}
			}
		}
		else if((j == (cyc_rep - 1))&&(TRUE == mid_size))
		{
			/*Create 2nd block for mid size msg*/
			/*Fill zeroes for the last block and append length*/
			for (x = 0; x < 64; x++)
			{
				if(x < 56)
				{
					m[x] = 0;
				}
				/*append length of the message*/
				else
				{
					m[x] = (len >> ((63 - x)<<3)) & 0xff;
				}
			}
			calculatehash(&m[0]);
			break;
		}

		else if((j != (cyc_rep - 1))&&(TRUE == mid_size))
		{
			/*Just copy the msg data rest of the info is already appended to the buffer*/
			s_len = len;
		}
		else
		{
			/*not last repetition hence set the length as 512*/
			s_len = 64;
		}

		for (i = 0; i < (s_len); i++)
		{
			m[i] = (uint8)(*msg++);
		}

		calculatehash(&m[0]);


	}//End For loop



	return k;
}

///////////////End Block Manipulation Section/////////////////////

///////////////Block Hash Calculation Section/////////////////////

static void conv_msgbyte_to_word(uint8 *msg,uint8 * msg_32)
{
	uint8 i;
#if (BIG_ENDIAN == FALSE)
	uint8 j;
	for (i = 0; i < 61; i+=4)
	{
		for(j = 4; j > 0; j--)
		{
			msg_32[i + (4 - j)] = msg[i + j - 1];
		}
	}
#else
	msg_32 = msg;
#endif
}

static void calculatehash(uint8 *msg)
{
	uint32 a,b,c,d,e,f,k;
	uint32 temp;
	uint32 temp1;
	uint8 i;
	uint32 msg_32[80] = {0};

	/*Convert msg into 16 32bit long word in big endian format*/
	conv_msgbyte_to_word(msg,(uint8 *)msg_32);

	for(i = 16; i < 80; i++)
	{
		msg_32[i] = (msg_32[i - 3] ^ msg_32[i - 8] ^ msg_32[i - 14] ^ msg_32[i - 16] );
		temp = (msg_32[i] & 0x80000000) >> 31;
		msg_32[i] = (msg_32[i] << 1) & temp;
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
		temp1 = ((a << 5) & temp) + f + e + k + msg_32[i];
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

///////////////End Block Hash Calculation Section/////////////////////

int main()
{
	uint32 a[15] = {0x12345678,0x9abcdef0,0x12345678,0x9abcdef0,0x12345678,0x9abcdef0,
			0x12345678,0x9abcdef0,0x12345678,0x9abcdef0,0x12345678,0x9abcdef0,0x12345678,
			0x9abcdef0,0x12345678};
	uint32 msg_d;
	msg_d = sha_1((uint8 *)&a[0],60);
	return 0;
}







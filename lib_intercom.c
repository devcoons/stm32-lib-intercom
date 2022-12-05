/*!
	@file   lib_intercom.c
    @brief  <brief description here>
	@t.odo	-
	---------------------------------------------------------------------------
	MIT License
	Copyright (c) 2022 Energica Motor Company (Dan.R)
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/******************************************************************************
* Preprocessor Definitions & Macros
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/

#include "lib_intercom.h"

#ifdef LIB_INTERCOM_ENABLED

/******************************************************************************
* Enumerations, structures & Variables
******************************************************************************/

/******************************************************************************
* Declaration | Static Functions
******************************************************************************/

/******************************************************************************
* Definition  | Static Functions
******************************************************************************/

/******************************************************************************
* Definition  | Public Functions
******************************************************************************/

i_status intercom_init(intercom_t* h, intercom_entry_t* entries, size_t entries_cnt)
{
	h->entries     = entries;
	h->entries_cnt = entries_cnt;
	return I_OK;
}

i_status intercom_serialize(intercom_t* h, uint8_t* buffer_send, uint32_t* b_sz)  
{
	uint32_t c_pos = 4;

	for (int i = 0; i < h->entries_cnt; i++)
	{
		if (h->entries[i].sz == 1)
		{
			memmove(&buffer_send[c_pos], h->entries[i].var, 1);
			c_pos += 1;
		}
		else if (h->entries[i].sz == 2 && h->entries[i].is_mem == 0)
		{
			
				buffer_send[c_pos]   = (uint8_t)(((*((uint16_t*)h->entries[i].var)) & 0xFF00) >> 8);
				buffer_send[c_pos+1] = (uint8_t)((*((uint16_t*)h->entries[i].var) & 0x00FF));
				c_pos += 2;

		}
		else if (h->entries[i].sz == 4 && h->entries[i].is_mem == 0)
		{
			
				buffer_send[c_pos]     = (uint8_t)(((*((uint32_t*)h->entries[i].var)) & 0xFF000000) >> 24);
				buffer_send[c_pos + 1] = (uint8_t)((*((uint32_t*)h->entries[i].var) & 0x00FF0000) >> 16);
				buffer_send[c_pos + 2] = (uint8_t)(((*((uint32_t*)h->entries[i].var)) & 0x0000FF00) >> 8);
				buffer_send[c_pos + 3] = (uint8_t)((*((uint32_t*)h->entries[i].var) & 0x000000FF));
			    c_pos += 4;
		}
		else
		{

			memmove(&buffer_send[c_pos], (uint8_t*)h->entries[i].var, h->entries[i].sz);
			c_pos += h->entries[i].sz;
		
		}
	}
	*b_sz = c_pos;                             

	uint16_t crc_s = crc16_ccitt(0xffff, &buffer_send[4], c_pos - 4);

	buffer_send[0] = (uint8_t)(((c_pos - 4) & 0xFF00) >> 8);
	buffer_send[1] = (uint8_t)(((c_pos - 4) & 0x00FF) >> 0);
	buffer_send[2] = (uint8_t)(((crc_s) & 0xFF00) >> 8);
	buffer_send[3] = (uint8_t)(((crc_s) & 0x00FF) >> 0);

	return I_OK;
}

i_status intercom_deserialize(intercom_t* h, uint8_t* buffer_read, uint32_t* b_sz)                            
{
	
	uint16_t usable_sz = 0x0000;
	usable_sz          = (buffer_read[0] << 8) | (buffer_read[1]);				
	size_t actual_sz   = 0;
	for (int i = 0; i < h->entries_cnt; i++)
	{
		actual_sz += h->entries[i].sz; 
	}

	if ((usable_sz != actual_sz))
		return I_FAILED;	
	
	uint16_t crc_r = 0x0000;
	crc_r          = (buffer_read[2] << 8) | (buffer_read[3]);                   
	uint16_t crc_c = crc16_ccitt(0xffff, &buffer_read[4], usable_sz);   

	if (crc_c != crc_r)
		return I_FAILED;
	 
	uint32_t c_pos = 4;
	for (int i = 0; i < h->entries_cnt; i++) 
	{
		if (h->entries[i].sz == 1)
		{
			memmove(h->entries[i].var, &buffer_read[c_pos], 1);
			c_pos++;
		}
		else if (h->entries[i].sz == 2 && h->entries[i].is_mem == 0)
		{
			uint16_t t16;
			t16  = (uint16_t)(buffer_read[c_pos] << 8);
			t16 |= (uint16_t)(buffer_read[c_pos + 1]);
			memmove(h->entries[i].var, &t16, 2);
			c_pos += 2;
		}
		else if (h->entries[i].sz == 4 && h->entries[i].is_mem == 0)
		{
			uint32_t t32;
			t32  = (uint32_t)(buffer_read[c_pos] << 24);
			t32 |= (uint32_t)(buffer_read[c_pos + 1] << 16);
			t32 |= (uint32_t)(buffer_read[c_pos + 2] << 8);
			t32 |= (uint32_t)(buffer_read[c_pos + 3]);
			memmove(h->entries[i].var, &t32, 4);
			c_pos += 4;
		}
		else
		{
			memmove((uint8_t*)(h->entries[i].var) , &buffer_read[c_pos], h->entries[i].sz);
			c_pos += h->entries[i].sz;
		}
	}
	return I_OK;
}

/******************************************************************************
* EOF - NO CODE AFTER THIS LINE
******************************************************************************/
#endif

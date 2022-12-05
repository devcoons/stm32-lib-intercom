# Intercom library

This library is needed to place in a unique structure the data to be shared between microcontrollers.

## Supported Hardware

- NUCLEO-H745ZI-Q

### Functions Guide

- `intercom_init`: initializes the structure `intercom_t` that contains the entries and their number.
- `intercom_serialize`: prepares the entries to be sent.
- `intercom_deserialize`: prepares the entries to be received.



## How to use

- In your main, include `lib_intercom.h`
- Create an instance of `intercom_t` to be used as the handler and afterwards create an array of `intercom_entry_t` (intercom_entry_t entries[]={entry1, entry2...}) and add all the variables you want to send or receive (order matters). For each entry, define
	- `.var` : pointer to the variable to be sent
	- `.sz :`size of the variable
	- `.is_mem` : 1 if the variable is already a memory address, 0 if it's not. 

For example:

```C
intercom_t h;

uint8_t h_data[] = { 1, 4, 3 ,4, 5, 6, 7, 8, 9, 10, 11 };
uint32_t h_v1	= 0x12345678;
uint16_t h_v2	= 0xAABB;
uint8_t h_v3	= 0xCC;

intercom_entry_t h_entries[] = 
{
		{.var = &h_v1, .sz = 4, .is_mem = 0},
		{.var = &h_v2, .sz = 2, .is_mem = 0},
		{.var = &h_v3, .sz = 1, .is_mem = 0},
		{.var = h_data, .sz = 11, .is_mem = 1},
};
```

- Use the functions 
  - `intercom_serialize` : To serialize the `entries` into the given `buffer`
  - `intercom_deserialize` : To deserialize the given `buffer` into the `entries`

```C
intercom_serialize(&h, output_buffer, &output_buffer_size); 
intercom_deserialize(&h, input_buffer ,&input_buffer_size);
```


## Example

In this example, data is prepared to be sent and saved in a buffer (used as `buffer_send`).
The same buffer is then used as `buffer_read` in a "deserialize" function.

```C
#include <stdio.h>
#include "lib_intercom.h"

uint8_t h1_data[] = { 1, 4, 3 ,4, 5, 6, 7, 8, 9, 10, 11 };
uint32_t h1_v1	= 0x12345678;
uint16_t h1_v2	= 0xAABB;
uint8_t h1_v3	= 0xCC;

uint8_t h2_data[11] = { 0 };
uint32_t h2_v1 = 0;
uint16_t h2_v2 = 0;
uint8_t h2_v3 = 0;

intercom_t h1,h2; //structures for sent and received data

intercom_entry_t h1_entries[] = {

		{.var = &h1_v1, .sz = 4, .is_mem = 0},
		{.var = &h1_v2, .sz = 2, .is_mem = 0},
		{.var = &h1_v3, .sz = 1, .is_mem = 0},
		{.var = h1_data, .sz = 11, .is_mem = 1},
}; //entries to be sent

intercom_entry_t h2_entries[] = {

		{.var = &h2_v1, .sz = 4, .is_mem = 0},
		{.var = &h2_v2, .sz = 2, .is_mem = 0},
		{.var = &h2_v3, .sz = 1, .is_mem = 0},
		{.var = h2_data, .sz = 11, .is_mem = 1},
}; //received entries go there

uint8_t sd_buffer[22] = {0}; //buffer to store the sent data
							// or read the received data
uint32_t sd_buffer = 0;       

int main(void)
{
	
	intercom_init(&h1, h1_entries, 4); 
	intercom_init(&h2, h2_entries, 4);
	
	intercom_serialize(&h1, sd_buffer, &buffer_size); 

	intercom_deserialize(&h2, sd_buffer,NULL);


	return 0;
}

```

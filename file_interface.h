#ifndef FILEIO_H
#define FILEIO_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "file_classes.h"

extern "C"
{
	//helper struct for file read/write
	typedef struct
	{
		unsigned char *buffer;	//byte array pointer
		int size;				//size of buffer
		int good_bytes;			//number of good bytes read into buffer
		char done;				//end of input file flag
		FILE *in_pointer;		//input file pointer
		char *out_pointer;		//output file name pointer
	} FileIO;

	//struct related functions
	void init_FileIO(FileIO *file_io, char *in_file_name, char *out_file_name, unsigned char *meta_data, int meta_data_size);
	void free_FileIO(FileIO *file_io);
	void read_FileIO(FileIO *file_io, int bytes);
	void write_FileIO(FileIO *file_io, int bytes);

	//requested functions
	int read(FILE* file_pointer, unsigned char buffer[], int bytes, char *done);
	void write(char* file_pointer, unsigned char buffer[], int good_bytes);
}
#endif
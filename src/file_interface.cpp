#include "../include/file_interface.h"

void init_FileIO(FileIO *file_io, char *in_file_name, char *out_file_name, unsigned char *meta_data, int meta_data_size)
{
    //Write Initialization
    StreamManager *manager = StreamManager::GetInstance();
    manager->GetNewStream(out_file_name, meta_data, meta_data_size);
    file_io->out_pointer = out_file_name;
    
    //Read Initialization
    file_io->done = 0;
    file_io->in_pointer = fopen(in_file_name, "rb");	//open in_file_name for reading in binary mode
}
void free_FileIO(FileIO *file_io)
{
    StreamManager* manager = StreamManager::GetInstance();
	manager->CloseStream(file_io->out_pointer);

	if (manager->GetStreamMapSize() == 0)
	{
		delete manager;
	}

    
    //delete file_io->buffer;
    fclose(file_io->in_pointer);
    
}
void read_FileIO(FileIO *file_io, int bytes)
{
	StreamManager* manager = StreamManager::GetInstance();
	file_io->buffer = manager->GetNewReadBuffer(bytes);
    
    file_io->good_bytes = read(file_io->in_pointer, file_io->buffer, bytes, &(file_io->done)); //store number of bytes actually read into FileIO buffer in good_bytes
}

void write_FileIO(FileIO *file_io, int bytes)
{
	StreamManager *manager = StreamManager::GetInstance();
	StreamWriter *stream = manager->GetExistingStream(file_io->out_pointer);
	//stream->Queue(file_io->buffer, std::min(bytes, file_io->good_bytes));
	write(file_io->out_pointer, file_io->buffer, std::min(bytes, file_io->good_bytes)); //could comment out and use above line to avoid extra calls
	stream->SetDoneQueueing(file_io->done); //done flag set here and not in read_fileIO to ensure data is queued before write thread exits
}

int read(FILE* file_pointer, unsigned char buffer[], int bytes, char *done)
{
    size_t good_bytes = fread(buffer, 1, bytes, file_pointer);
    
	if (good_bytes != bytes && feof(file_pointer))
    {
        *done = 1; //if the end of file is reached, set done
    }
    
	return (int)good_bytes;
}

//only used to more closely match original C file_interface structure
void write(char* file_pointer, unsigned char buffer[], int good_bytes)
{
    StreamManager *manager = StreamManager::GetInstance();
    StreamWriter *stream = manager->GetExistingStream(file_pointer);
    stream->Queue(buffer, good_bytes);
}
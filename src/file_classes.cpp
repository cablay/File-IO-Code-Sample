#include "../include/file_classes.h"

/**************************
		StreamWriter
**************************/

StreamWriter::~StreamWriter()
{
	thread->join(); //wait for write thread to complete

	delete thread;
    delete mx;
    delete buffer;
	delete buffer_sizes;
    
    fclose(file_pointer); //close output file stream
}

void StreamWriter::Queue(unsigned char *data, int bytes)
{
	std::lock_guard<std::mutex> lock(*mx); //waits until mutex is available, then locks mutex
	buffer->push(data);
	buffer_sizes->push(bytes);
}

void StreamWriter::ThreadedWrite()
{ 
	while (!(done_queueing && GetBufferEmpty())) //runs until the buffer is empty and queueing has finished
    {
		if (!GetBufferEmpty()) //write if buffer is not empty
		{
			unsigned char *byte = LockedBufferPop<unsigned char*>(buffer);
			int bytes = LockedBufferPop<int>(buffer_sizes);

			fwrite(byte, 1, bytes, file_pointer);

			delete byte; //free input buffer memory
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); //simulate slow tape writing
		}
    }
}

bool StreamWriter::GetBufferEmpty()
{
	std::lock_guard<std::mutex> lock(*mx); //waits until mutex is available, then locks mutex
	return buffer->empty();
}

void StreamWriter::SetDoneQueueing(char done_queueing)
{
	this->done_queueing = done_queueing ? true : false;
}

template<typename T>
T StreamWriter::LockedBufferPop(std::queue<T>* queue)
{
	std::lock_guard<std::mutex> lock(*mx); //waits until mutex is available, then locks mutex

	T front = queue->front();
	queue->pop();

	return front;
}



/**************************
	 StreamWriterMeta
**************************/

StreamWriterMeta::StreamWriterMeta(char *file_name, unsigned char *meta_data, int meta_data_size)
{
    buffer = new std::queue<unsigned char*>;
	buffer_sizes = new std::queue<int>;
    mx = new std::mutex;
    file_pointer = fopen(file_name, "wb"); //open output file stream
	done_queueing = false;

	thread = new std::thread(&StreamWriter::ThreadedWrite, this);

	unsigned char* meta_data_buffer = AllocateMetaDataMemory(meta_data, meta_data_size);
    
	Queue(meta_data_buffer, meta_data_size);
}

unsigned char* StreamWriterMeta::AllocateMetaDataMemory(unsigned char* meta_data, int meta_data_size)
{
	unsigned char* meta_data_buffer = new unsigned char[meta_data_size];
	memcpy(meta_data_buffer, meta_data, meta_data_size);
	return meta_data_buffer;
}



/**************************
	 StreamWriterPlain
**************************/

StreamWriterPlain::StreamWriterPlain(char *file_name)
{
    buffer = new std::queue<unsigned char*>;
	buffer_sizes = new std::queue<int>;
    mx = new std::mutex;
    file_pointer = fopen(file_name, "wb"); //open output file stream
	done_queueing = false;

	thread = new std::thread(&StreamWriter::ThreadedWrite, this);
}



/**************************
	  StreamManager
**************************/

StreamManager* StreamManager::instance = 0; //initialize StreamManager instance to 0/NULL

StreamManager::StreamManager()
{
    streams = new std::map<char*, StreamWriter*>;
}

StreamManager::~StreamManager()
{
    streams->clear();
    delete streams;
}

StreamManager* StreamManager::GetInstance()
{
    if (!instance)
    {
        instance = new StreamManager();
    }
    
    return instance;
}

StreamWriter* StreamManager::GetNewStream(char *file_name, unsigned char *meta_data, int meta_data_size)
{
    StreamWriter *new_writer;
    
    if (meta_data)
    {
        new_writer = new StreamWriterMeta(file_name, meta_data, meta_data_size); //create new StreamWriterMeta
    }
    else
    {
        new_writer = new StreamWriterPlain(file_name); //create new StreamWriterPlain
    }
    
    (*streams)[file_name] = new_writer; //store stream in map
    
    return new_writer;
    
}

StreamWriter* StreamManager::GetExistingStream(char *file_name)
{
    return (*streams)[file_name];
}

void StreamManager::CloseStream(char *file_name)
{
	delete (*streams)[file_name];
    streams->erase(file_name);
}

int StreamManager::GetStreamMapSize()
{
    return (int) streams->size();
}

unsigned char* StreamManager::GetNewReadBuffer(int bytes)
{
	unsigned char *buffer = (unsigned char *)malloc(bytes);
	return buffer;
}
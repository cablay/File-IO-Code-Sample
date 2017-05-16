#ifndef FILE_CLASSES_H
#define FILE_CLASSES_H

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>

//Abstract StreamWriter Base Class
class StreamWriter
{
	public:
		virtual ~StreamWriter() = 0;

		void Queue(unsigned char *data, int bytes);	//used to queue the bytes from input buffer to write
		void ThreadedWrite();						//used to actually write the queued bytes in a separate thread
		void SetDoneQueueing(char done_queueing);	//accessor for done_queueing

	protected:
		bool GetBufferEmpty();						//locked check if byte buffer is empty
		
		template<typename T>
		T LockedBufferPop(std::queue<T>*);			//locked pop of the queue whose pointer is passed in; returns front element of queue prior to pop

		FILE *file_pointer;					//pointer to output file
		std::mutex *mx;						//mutex for stream threading
		std::queue<unsigned char*> *buffer;	//queue of unsigned char pointers
		std::queue<int> *buffer_sizes;		//queue of input buffer sizes corresponding to buffer
		std::thread *thread;				//pointer to write thread
		bool done_queueing;					//true when queueing complete
};

//Class for Writing w/ Metadata
class StreamWriterMeta : public StreamWriter
{
	public:
		StreamWriterMeta(char *file_name, unsigned char *meta_data, int meta_data_size);
		unsigned char* AllocateMetaDataMemory(unsigned char* meta_data, int meta_data_size); //ensure meta_data is dynamically allocated so that ThreadedWrite free() works
};

//Class for Writing w/o Metadata
class StreamWriterPlain : public StreamWriter
{
	public:
		StreamWriterPlain(char *file_name);
};

//StreamManager Class (Singleton)
class StreamManager
{
	public:
		~StreamManager();
		static StreamManager* GetInstance(); //gets a pointer to the single StreamManager and creates it if it does not already exist

		StreamWriter* GetNewStream(char *file_name, unsigned char *meta_data = 0, int meta_data_size = 0); //open a new StreamWriter, store its pointer, and return its pointer
		StreamWriter* GetExistingStream(char *file_name); //access a stream in streams
        void CloseStream(char *file_name); //close a stream in streams

        int GetStreamMapSize();							//accessor for streams' size
		unsigned char* GetNewReadBuffer(int bytes);		//create new read buffer, which is deleted in ThreadedWrite once data has been written to output file

	private:
        StreamManager(); //singleton class, so constructor is private
		StreamManager(StreamManager const&) = delete;	//prevent copies
		void operator =(StreamManager const&) = delete; //prevent copies

		static StreamManager *instance;				//pointer to single StreamManager instance
		std::map<char*, StreamWriter*> *streams;	//stores StreamWriter pointers
};

#endif
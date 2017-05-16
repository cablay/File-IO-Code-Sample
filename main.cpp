#include "file_interface.h"

int main()
{
	unsigned char meta_data[] = "AUTHOR: CALEB MARKLEY DATE: 4/16/15 COMMENT: SOOO META";
    char input_file[] = "Input.dat";
    char output_file_1[] = "OutputFile1.dat";
    char output_file_2[] = "OutputFile2.dat";

	//create FileIO structs
    FileIO file_io_1;
    FileIO file_io_2;
    
	//initialize FileIO variables
    init_FileIO(&file_io_1, input_file, output_file_1, 0, 0);
    init_FileIO(&file_io_2, input_file, output_file_2, meta_data, sizeof(meta_data));
    
    while (!(file_io_1.done && file_io_2.done))
    {
        if (!file_io_1.done)
        {
            read_FileIO(&file_io_1, 5);
            
            for (int i = 0; i < 5; i++)
            {
                file_io_1.buffer[i] = ~file_io_1.buffer[i]; //byte complement of FileIO read data (bitwise NOT)
            }
            
            write_FileIO(&file_io_1, 5);
        }
        
        if (!file_io_2.done)
        {
            read_FileIO(&file_io_2, 10);
            write_FileIO(&file_io_2, 10);
        }
    }
    
	//free memory allocated by FileIO
    free_FileIO(&file_io_2);
	free_FileIO(&file_io_1);
    
    return 0;
}
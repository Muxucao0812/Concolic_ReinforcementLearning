#pragma once
#include <string>
#include <vector>
#include <map>
#include "types.h"

typedef struct{
    uint msb;
    uint lsb;
    uint width;
}sig_pos;

//Class for managing data file
class CTDataMem{
public:	
    CTDataMem();
    
    //Load a data file and create corresponding data structure
    bool load(const char* src_file);
    
    //Dump data structure to a file
    void dump(const char* dest_file);
    
    //Generate random bits in data, then dump to file
    void generate();
    
    //Generate step number of input vectors
    void generate_step();

    //Add input signal name to in_ports
    sig_pos* add_input(std::string name, uint port_width);
	
	inline uint get_width() {return width;}
	
	//Read src_file (output of constraint solver), update variables, and dump output
	void update_and_dump(const char* src_file, const char* dest_file);
	
    //Input ports mapped with their name
	std::map<std::string, sig_pos*> in_ports;	
    
private:
    //modify a range of bits
	void modify(uint clock, const sig_pos* sig, const std::string &value);
    //Read src_file (output of constraint solver) and update variables
	bool update_input_vectors(const char* src_file);

    std::vector<std::string> data;			 //structure for holding data    
    uint width;
    uint unroll;
    uint fuzzing;
    uint step;
};

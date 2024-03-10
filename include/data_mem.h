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

    uint get_width() const { return width; }
    void set_width(uint w) { width = w; }

    uint get_clk() const { return clk; }
    void set_clk(uint c) { clk = c; }

    uint get_step() const { return step; }
    void set_step(uint s) { step = s; }

    const std::vector<std::string>& get_input_vector() const { return input_vector; }
    void add_to_input_vector(const std::string& value) { input_vector.push_back(value); }

    //Load a data file and create corresponding data structure
    bool load(const char* src_file);
    
    //Dump data structure to a file
    void dump(const char* dest_file);
    
    //Generate random bits in data, then dump to file
    void generate(const char* file);

    //Add input signal name to in_ports
    sig_pos* add_input(std::string name, uint port_width);
	
	//Read src_file (output of constraint solver), update variables, and dump output
	void update_and_dump(const char* src_file, const char* dest_file, uint clock);
	
    //Input ports mapped with their name
	std::map<std::string, sig_pos*> in_ports;	
    
    // Connect two vectors
    void connect(const CTDataMem& src);

    // Intercept one vector
    void intercept(const CTDataMem& source, uint start, uint end);

    //clear all data
    void clear_input_vector();

    std::vector<std::string> input_vector;			 //structure for holding data    
private:
    //modify a range of bits
	void modify(uint clock, const sig_pos* sig, const std::string &value);
    //Read src_file (output of constraint solver) and update variables
	bool update_input_vectors(const char* src_file, uint clock_limitation);


    uint width;
    uint clk;
    uint step;
};

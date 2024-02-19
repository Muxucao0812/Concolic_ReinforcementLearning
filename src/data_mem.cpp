#include <fstream>
#include <cassert>
#include "data_mem.h"
#include "globals.h"
#include <iostream>
using namespace std;

CTDataMem::CTDataMem(){
	clk = 0;
    step = 0;
	width = 0;
}

// load data from file
bool CTDataMem::load(const char* file) {
	input_vector.clear();
    ifstream mem(file);
    if(!mem.is_open()){
        return false;
    }
    string line;
    // read each line without '\n'
    while(mem >> line){
        input_vector.push_back(line);
    }
    mem.close();
    return true;
}

//Dump data to file
void CTDataMem::dump(const char* file) {
    ofstream mem(file);
    for(auto line:input_vector){
        mem << line << '\n';
    }
    mem.close();
}



// Yangdi: Be careful when using rand()
// It generates 31 bits of random number in lab machine
// Xiangchen: This function generates fuzzing number input vectors
void CTDataMem::generate(const char* file) {
    step = g_step;
    assert(step);
    assert(width);
    clk = step;
    input_vector.clear();

    // We want to use 16 bits from each rand()
    const uint count = width >> 4;
    const uint extra = width & 0b1111;
    for(uint k=0; k<step; k++){
        string in_vector;
        uint num = rand();
        for(uint j=0; j < extra; j++){
            in_vector += '0' + (num & 1);
            num >>= 1;
        }
        for(uint i=0; i < count; i++){
            num = rand();
            for(uint j=0; j < 16; j++){
                in_vector += '0' + (num & 1);
                num >>= 1;
            }
        }
        if(enable_obs_padding){
            in_vector[0] = '0' + (k & 1);
        }
        input_vector.push_back(in_vector);
    }
    dump(file);
}

sig_pos* CTDataMem::add_input(string name, uint port_width){
	sig_pos* sig = new sig_pos;
	sig->lsb = width;
	sig->msb = width + port_width - 1;
	sig->width = port_width;
	in_ports[name] = sig;
	width += port_width;
	return sig;
}

void CTDataMem::modify(uint clock, const sig_pos* sig, const std::string &value){
	string &str = input_vector[clock];
    str.replace(str.length() - sig->msb - 1, sig->width, value);
}

bool CTDataMem::update_input_vectors(const char* src_file, uint clock_limitation){
	ifstream f_in(src_file);
	if(!f_in.is_open()){
		return false;
	}
    string line;
	while(getline(f_in, line)){
        string name;
        uint clock = 0;
    
        //if the line contain "(function" or the first char is " ", continue
        if(line.find("(function") != string::npos || line[0] == ' '){
            continue;
        }
        //parse
        uint pos = 3;//标注第一个空格开始的位置
        while(line[pos] != ' ') pos++;//标注信号与值之间的空格
        uint mark1 = pos + 3;//标注值开始的位置
        line[pos--] = 0;
        while(line[pos] != '_') pos--;
        name = line.substr(3, (pos-3));
        map<string, sig_pos*>::iterator it = in_ports.find(name);
        if(it != in_ports.end()){
            pos++;
            while(line[pos]){
                clock = clock*10 + line[pos]-'0';
                pos++;
            }
            //Yangdi: skip clock 0
            if (clock == 0) continue;
            pos = mark1 + 1;
            while(line[pos] != ')') pos++;
            if(clock <= clock_limitation){
                modify(clock-1, it->second, line.substr(mark1, pos - mark1));
            }else{
                continue;
            }
 
        }
    }
    return true;
}

void CTDataMem::clear_input_vector(){
    input_vector.clear();
}

void CTDataMem::update_and_dump(const char* src_file, const char* dest_file, uint clock){
	update_input_vectors(src_file, clock);
	dump(dest_file);
    dump(g_data_mem);
}

void CTDataMem::connect(const CTDataMem& src) {
    for(uint i = 0; i < src.input_vector.size(); i++) {
        this->input_vector.push_back(src.input_vector[i]);
    }
}

void CTDataMem::intercept(const CTDataMem& source, uint start, uint end) {
    // 清空当前的input_vector
    this->input_vector.clear();
    this->in_ports = source.in_ports;
    this->width = source.width;
    this->clk = end;
   
    // 从source的input_vector中截取部分数据并添加到当前对象的input_vector中
    if (end > source.input_vector.size()) {
        end = source.input_vector.size();
    }
    if (start < end) {
        this->input_vector.insert(this->input_vector.end(),
                                  source.input_vector.begin() + start,
                                  source.input_vector.begin() + end);
    }
}




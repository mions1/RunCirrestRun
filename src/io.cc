#include <fstream>

#include "headers/io.h"

bool saveToFile(int data,const char file[]){
	std::ofstream f(file) ;
	if (!f)
		return false ;
	f<<data ;
	return true;
}

bool loadFromFile(int &data,const char file[]) {
	std::ifstream f(file);
	if (!f) {
		f.close();
		saveToFile(0,file);
		f.open(file,std::ifstream::in);
	}
	f>>data ;
	f.close();
	return true ;
}

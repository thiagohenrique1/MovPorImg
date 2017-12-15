#ifndef MOVPORIMG_STRUCTS_H
#define MOVPORIMG_STRUCTS_H

#include <string>

struct ponto {
	double x;
	double y;
	double z;
};

struct comando {
	std::string cmd;
	int pos;
	double x;
	double y;
	double z;
};

#endif //MOVPORIMG_STRUCTS_H

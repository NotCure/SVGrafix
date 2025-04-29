#include "FileSVG.h"
#include "DomSVG.h"


FileSVG::FileSVG(std::string Content) : Content(Content) {
	std::cout << "FileSVG constructor called!" << std::endl;
	
	DomSVG domSVG(Content);


};

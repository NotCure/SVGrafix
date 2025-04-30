#include "include/svgrafix/FileSVG.h"
#include "include/svgrafix/DomSVG.h"


FileSVG::FileSVG(std::string Content) : Content(Content) {
	std::cout << "FileSVG constructor called!" << std::endl;
	
	DomSVG domSVG(Content);


};

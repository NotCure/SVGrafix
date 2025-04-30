#pragma once

#include "SVGElement.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory> 




class DomSVG
{
public:	
	std::string fileContent;
	std::string fileWithoutPrologue;
	std::string fileWithoutComments;
	std::vector<std::unique_ptr<SVGElement>> elements;




public:
	DomSVG(const std::string& fileContent);
	void removePrologue();
	void removeComments();
	void parse();
};





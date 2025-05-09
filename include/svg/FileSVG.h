#include "DomSVG.h"

#include <string>
#include <iostream>


class FileSVG
{


public:
	explicit FileSVG(std::string content);
	const DomSVG& get_dom() const noexcept { return domSVG_; }
	error::ParseResult parse() { return domSVG_.parse(); }

private:
	DomSVG domSVG_;
};


#include <svg/FileSVG.h>



FileSVG::FileSVG(std::string content)
    : domSVG_(std::move(content)){}

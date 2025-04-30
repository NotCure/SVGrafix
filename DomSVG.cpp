#include <iostream>

#include "include/svgrafix/DomSVG.h"
#include "include/svgrafix/Utility.h"
#include "include/svgrafix/TagTypes.h"
#include <iostream>
using namespace Utility::String;

DomSVG::DomSVG(const std::string& fileContent) : fileContent(fileContent)
{

	removePrologue();
	removeComments();
	parse();

}

void DomSVG::removePrologue()
{
	auto& fileContent = this->fileContent;
	auto prologueStart = fileContent.find("<?");
	auto prologueEnd = fileContent.find("?>", prologueStart+2);

	if (std::string::npos != prologueEnd && std::string::npos != prologueStart) {
		this->fileWithoutPrologue = fileContent.erase(prologueStart, prologueEnd - prologueStart + 2);
		std::cout << "Prologue removed" << std::endl;
	}
	else
	{
		std::cerr << "No prologue found" << std::endl;
	}
}


void DomSVG::removeComments()
{

	fileWithoutComments = fileWithoutPrologue;

	size_t start = 0;
	while ((start = fileWithoutComments.find("<!--", start)) != std::string::npos) {

		size_t end = fileWithoutComments.find("-->", start + 4);
		if (end == std::string::npos) {
			std::cerr << "fix comments ending" << start << "\n";
			break;
		}
		fileWithoutComments.erase(start, end - start + 3);
	}
}





void DomSVG::parse()
{
	const std::string& content = this->fileWithoutComments;



	size_t pos = 0;
    while ((pos = content.find('<', pos)) != std::string::npos) {
        if (pos + 1 < content.size() && content[pos + 1] == '/') {
            pos++;
            continue;
        }

        size_t endPos = content.find('>', pos + 1);
        size_t nextOpen = content.find('<', pos + 1);
        if (endPos == std::string::npos || (nextOpen != std::string::npos && endPos > nextOpen)) {
            std::cerr << "Ending tag corrupt " << pos << "\n";
            break;
        }

        size_t nameStart = pos + 1;
        size_t nameEnd = content.find_first_of(" \t/>", nameStart);
        if (nameEnd == std::string::npos || nameEnd > endPos)
            nameEnd = endPos;
        std::string tagName = content.substr(nameStart, nameEnd - nameStart);

        size_t bodyStart = nameEnd;
        size_t bodyLen = endPos - bodyStart;
        std::string tagBody = content.substr(bodyStart, bodyLen);



        std::string lower = convertToLowercase(tagName);
        auto it = tagLookup.find(lower);
        TagType type = (it == tagLookup.end()
            ? TagType::UNKNOWN
            : it->second);

        switch (type) {
        case TagType::SVG: {
            auto svgElem = std::make_unique<SVG>();

            svgElem->parseAttributes(tagBody);

            auto attributes = svgElem->getAllAttributes();
            for (const auto& [key, value] : attributes) {
                std::cout << "svg " << key << " = " << value << "\n";
            }


            elements.emplace_back(std::move(svgElem));
           

        } break;
        case TagType::RECT: {
            auto rectElem = std::make_unique<SVGRect>();

            rectElem->parseAttributes(tagBody);

			auto attributes = rectElem->getAllAttributes();
            for (const auto& [key, value] : attributes) {
				std::cout << "rectangle " << key << " = " << value << "\n";
            }


            elements.emplace_back(std::move(rectElem));


        }   break;
        case TagType::CIRCLE:
           // std::cout << "[CIRC] " << tagBody << "\n";
            break;
        default:
          //  std::cout << "[OTHER][" << tagName << "] " << tagBody << "\n";
            break;
        }

 
        pos = endPos + 1;
    }
}


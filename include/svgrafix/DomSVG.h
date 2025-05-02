#pragma once

#include "SVGElement.h"
#include "Error/ParseError.h"
#include <functional>
#include <memory> 




class DomSVG {
public:
    explicit DomSVG(std::string content);
    error::ParseResult parse();
    void debugPrint(std::ostream& os) const;
    
    using ElementList = std::vector<std::unique_ptr<SVGElement>>;
    const ElementList& getElements() const noexcept;

private:
    void remove_prologue();
    void remove_comments();

    std::string text_;
    std::string no_prologue_;
    std::string no_comments_;


    ElementList elements_;
    using Factory = std::function<std::unique_ptr<SVGElement>(std::string_view)>;
    static const std::unordered_map<std::string_view, Factory> registry_;
};

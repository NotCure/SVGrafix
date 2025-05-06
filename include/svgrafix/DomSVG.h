#pragma once

#include "SVGElement.h"
#include "Error/ParseError.h"
#include <functional>
#include <memory> 




class DomSVG {
public:
    explicit DomSVG(std::string content);
    error::ParseResult parse();
    void debug_print(std::ostream& os) const;
    
    using ElementList = std::vector<std::unique_ptr<SVGElement>>;
    const ElementList& get_elements() const noexcept;

private:
    void remove_prologue(error::ParseResult& results);
    void remove_comments(error::ParseResult& results);

    std::string text_;
    std::string no_prologue_;
    std::string no_comments_;


    ElementList elements_;
    using Factory = std::unique_ptr<SVGElement>(*)(std::string_view);

    static const std::unordered_map<std::string_view, Factory> registry_;
};

#pragma once

#include "SVGElement.h"
#include "error/ParseError.h"
#include "SVGNode.h"
#include "Css.h"
#include <functional>
#include <memory> 




class DomSVG {
public:
    explicit DomSVG(std::string content);
    [[nodiscard]] error::ParseResult parse();
    const css::StyleSheet& stylesheet() const noexcept { return sheet_; }

    void debug_print(std::ostream& os) const;
    void extract_css();
    const SVGNode* root()   const noexcept { return root_.get(); }
    SVGNode* root()         noexcept { return root_.get(); }

private:
    void remove_prologue(error::ParseResult& results);
    void remove_comments(error::ParseResult& results);
    void clear();

    std::string text_;
    std::string no_prologue_;
    std::string no_comments_;
	css::StyleSheet sheet_;


    std::unique_ptr<SVGNode> root_;
    std::vector<SVGNode*>    stack_;

    using Factory = std::unique_ptr<SVGElement>(*)(std::string_view);

    static const std::unordered_map<std::string_view, Factory> registry_;
    bool parsing_ = false;

    
};

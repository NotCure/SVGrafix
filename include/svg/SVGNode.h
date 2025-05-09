#pragma once
#include "SVGElement.h"
#include <memory>
#include <vector>

struct SVGNode {
    std::unique_ptr<SVGElement>        element;   
    std::vector<std::unique_ptr<SVGNode>> children;

    explicit SVGNode(std::unique_ptr<SVGElement> e)
        : element(std::move(e)) {}
};
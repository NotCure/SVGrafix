
#pragma once
#include <memory>
#include <svg/DomSVG.h>
#include <svg/SVGElement.h>

template<typename T>
inline std::unique_ptr<SVGElement> element_factory(std::string_view body)
{
    auto e = std::make_unique<T>();
    e->parseAttributes(body);
    return e;
}

#define REGISTER_SVG_ELEMENT(tag, Type)                 \
  namespace {                                           \
    const bool _reg_##Type = [] {                       \
        DomSVG::registry().emplace(                     \
            tag, &::element_factory<Type>);              \
        return true;                                    \
    }();                                                \
  }

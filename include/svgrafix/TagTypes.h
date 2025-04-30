
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

enum class TagType : uint8_t {
    SVG, RECT, CIRCLE, ELLIPSE, LINE, POLYLINE, POLYGON,
    PATH, TEXT, G, DEFS, STYLE, CLIP_PATH, MASK, IMAGE,
    UNKNOWN
};

inline const std::unordered_map<std::string, TagType> tagLookup = {
    {"svg",      TagType::SVG},
    {"rect",     TagType::RECT},
    {"circle",   TagType::CIRCLE},
    {"ellipse",  TagType::ELLIPSE},
    {"line",     TagType::LINE},
    {"polyline", TagType::POLYLINE},
    {"polygon",  TagType::POLYGON},
    {"path",     TagType::PATH},
    {"text",     TagType::TEXT},
    {"g",        TagType::G},
    {"defs",     TagType::DEFS},
    {"style",    TagType::STYLE},
    {"clippath", TagType::CLIP_PATH},
    {"mask",     TagType::MASK},
    {"image",    TagType::IMAGE}
};

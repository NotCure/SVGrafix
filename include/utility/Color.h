#pragma once
#include <cstdint>
#include <string_view>

struct Rgba
{
    float r{}, g{}, b{}, a{ 1.f };

    constexpr bool operator==(const Rgba& o) const noexcept
    {
        return r == o.r && g == o.g && b == o.b && a == o.a;
    }
};


Rgba parse_color(std::string_view css);

#include <functional>
namespace std {
    template<> struct hash<Rgba>
    {
        size_t operator()(const Rgba& c) const noexcept
        {
            const auto* p = reinterpret_cast<const std::uint32_t*>(&c);
            size_t h = 14695981039346656037u;
            h = (h ^ p[0]) * 1099511628211u;
            h = (h ^ p[1]) * 1099511628211u;
            h = (h ^ p[2]) * 1099511628211u;
            h = (h ^ p[3]) * 1099511628211u;
            return h;
        }
    };
}

#include <svg/style/StyleSolver.h>

StyleSolver::StyleSolver(css::StyleSheet const& sheet)
	: sheet_(sheet) {};



ComputedStyle StyleSolver::resolve(SVGElement const& el) const {
	ComputedStyle output;


	auto decls = sheet_.declartions_for(el);

    static const std::unordered_map<
        std::string_view,
        std::function<void(ComputedStyle&, std::string_view)>
    > appliers = {
        { "fill", [](auto& cs, auto v) {
            cs.fill = parse_color(v);
        }},
        { "stroke", [](auto& cs, auto v) {
            cs.stroke = parse_color(v);
        }},
        { "stroke-width", [](auto& cs, auto v) {
            cs.stroke_width = std::stof(std::string(v));
        }},

    };

    for (auto const& [prop, val] : decls) {
        auto it = appliers.find(prop);
        if (it != appliers.end()) {
            it->second(output, val);
        }
    }

    return output;
}


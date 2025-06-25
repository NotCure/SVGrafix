#include <svg/style/Css.h>

namespace css {
    static bool matches(const SVGElement& el, std::string_view sel) {
        if (sel.empty()) return false;
        if (sel[0] == '.') {
            auto cls = el.get_attr("class");
            size_t p = 0;
            while (p < cls.size()) {
                auto nxt = cls.find(' ', p);
                auto part = cls.substr(p, nxt == std::string::npos ? cls.size() : nxt - p);
                if (part == sel.substr(1)) return true;
                if (nxt == std::string::npos) break;
                p = nxt + 1;
            }
            return false;
        }
        else if (sel[0] == '#') {
            return el.get_attr("id") == sel.substr(1);
        }
        else {
            return std::equal(sel.begin(), sel.end(),
                el.tag().begin(), el.tag().end(),
                [](char a, char b) { return tolower(a) == tolower(b); });
        }
    };

    void StyleSheet::add_rule(Rule rule) {
	    rules_.push_back(std::move(rule));
    }

    std::unordered_map<std::string, std::string>
    StyleSheet::declartions_for(const SVGElement& el) const {
        std::unordered_map<std::string, std::string> out;
        std::unordered_map<std::string, int> best_spec;

        for (auto const& rule : rules_) {
            for (auto const& sel : rule.selectors) {
                if (!matches(el, sel)) continue;
                for (auto const& d : rule.declarations) {
                    auto it = best_spec.find(d.property);
                    if (it == best_spec.end() || d.specificity >= it->second) {
                        out[d.property] = d.value;
                        best_spec[d.property] = d.specificity;
                    }
                }
            }
        }
        return out;
    }

}
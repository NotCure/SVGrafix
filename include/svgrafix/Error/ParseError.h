// include/error/ParseResult.h
#pragma once

#include <vector>
#include <string>
#include <ostream>

namespace error {

    // ——— error codes —————————————————————————————————————
    enum class ErrorCode {
        OK = 0,
        UnexpectedEOF,
        MalformedTag,
        TagNameTooLong,
        BadAttribute,
        UnclosedComment,
        FileNotFound,
        ReadError,
    };

    constexpr const char* to_string(ErrorCode c) noexcept {
        switch (c) {
        case ErrorCode::OK:              return "OK";
        case ErrorCode::UnexpectedEOF:   return "Unexpected end-of-file";
        case ErrorCode::MalformedTag:    return "Malformed tag";
        case ErrorCode::TagNameTooLong:  return "Tag name too long";
        case ErrorCode::BadAttribute:    return "Bad attribute syntax";
        case ErrorCode::UnclosedComment: return "Unclosed comment";
        case ErrorCode::FileNotFound:    return "File not found";
        case ErrorCode::ReadError:       return "Read error";
        }
        return "Unknown error thrown";
    }

    // ——— single error description ————————————————————————
    struct ParseError {
        ErrorCode   code;     
        std::size_t position; 
        std::string message;   

        ParseError(ErrorCode c,
            std::size_t pos,
            std::string msg = {})
            : code(c), position(pos), message(std::move(msg)) {}
    };

    inline std::ostream& operator<<(std::ostream& os, ParseError const& e) {
        os << "[" << to_string(e.code) << "] at " << e.position;
        if (!e.message.empty()) os << ": " << e.message;
        return os;
    }

    // ——— aggregate results ——————————————————————————
    struct ParseResult {
        std::vector<ParseError> errors;

        bool ok() const noexcept { return errors.empty(); }

        const ParseError& first() const noexcept {
            static ParseError none{ ErrorCode::OK, 0, "" };
            return errors.empty() ? none : errors.front();
        }
    };

}

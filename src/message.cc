module dxx.http;

import dxx.assert;
import dxx.cstd.fixed;
import dxx.overload;
import std;

import :status_codes;

namespace dxx::http {

namespace {

/**
 * @brief A byte sequence the isolates the request header from the body
 */
constexpr std::string_view header_separator = "\r\n\r\n";

} // <-- namespace<anonymous>

StringMap parse_content_disposition(std::string_view data) {
    StringMap ret{};
    for (const auto& part : std::views::split(data, ';')) {
        std::string_view part_view{ part };
        std::size_t start = 0;

        while (start < part_view.size() && part_view[start] == ' ') {
            ++start;
        }

        part_view = part_view.substr(start);

        if (auto ep = part_view.find('='); ep != std::string_view::npos) {
            const auto key = part_view.substr(0, ep);
            const auto val = part_view.substr(ep + 1);
            ret.emplace(key, val);
        } else {
            ret.emplace(part_view, std::string{});
        }
    }
    return ret;
} // <-- parse_content_disposition()

HeaderError Request::parse_header(bool skip_method) {
    // HeaderError::StrayData is non-fatal, we might return it from a successful
    // run
    HeaderError ret = HeaderError::None;

    std::string_view h_view{ this->header };
    uz cur = 0;

    enum {
        Method, Resource, Protocol, Header, Done
    } mode = skip_method ? Header : Method;

    const auto set_mode = [&] (auto new_mode) {
        mode   = new_mode;
        h_view = h_view.substr(cur);
        cur    = 0;
    }; // <-- set_mode(new_mode)

    while (cur < h_view.length()) {
        const char c = h_view[cur];
        ++cur;

        switch (mode) {
        case Method:
            if (c == ' ') {
                this->method_string = h_view.substr(0, cur - 1);
                set_mode(Resource);
            }
            if (c == '\r' || c == '\n') {
                return HeaderError::Method;
            }
            break;
        case Resource:
            if (c == ' ') {
                uz len = cur - 1;
                while (len > 1 && h_view[len - 1] == '/') {
                    --len;
                }
                this->url = h_view.substr(0, len);
                set_mode(Protocol);
            }
            if (c == '\r' || c == '\n') {
                return HeaderError::Method;
            }
            break;
        case Protocol:
            if (c == '\r' && cur < h_view.length() && h_view[cur] == '\n') {
                this->protocol = h_view.substr(0, cur - 1);
                ++cur;
                set_mode(Header);
            }
            break;
        case Header:
            if (c == '\r' && cur < h_view.length() && h_view[cur] == '\n') {
                if (cur == 1) {
                    // Read empty header, done
                    ++cur;
                    set_mode(Done);
                } else {
                    const auto sep = h_view.find(':');

                    if (
                        // No separator in the header
                        sep == std::string_view::npos
                        // Or '\r' or '\n' before separator
                        || h_view.find('\r') < sep || h_view.find('\n') < sep
                    ) {
                        return HeaderError::Header;
                    }

                    // cur - 1 = : or something else
                    // cur     = \r
                    // cur + 1 = \n
                    if (sep >= cur - 1) {
                        this->headers.emplace(
                            h_view.substr(0, cur - 1),
                            h_view.substr(cur - 1, 0)
                        );
                    } else {
                        auto value = h_view.substr(sep + 1, cur - sep - 2);
                        while (!value.empty() && value.front() == ' ') {
                            value = value.substr(1);
                        }
                        this->headers.emplace(h_view.substr(0, sep), value);
                    }
                    ++cur;
                    set_mode(Header);
                }
            }
            break;
        case Done:
            std::println(std::cerr, "Stray symbol in the header: `{}`", c);
            ret = HeaderError::StrayData;
            break;
        }
    }

    if (mode != Done) {
        return HeaderError::Unterminated;
    }

    if (
        const auto mit = methods_map.find(this->method_string);
        mit != methods_map.cend()
    ) {
        this->method = mit->second;
    } else {
        this->method = http::Method::Invalid;
    }

    // Parse the URL
    const auto qpos = this->url.find('?');
    if (qpos != std::string_view::npos) {
        this->resource      = url::decode(this->url.substr(0, qpos));
        this->params_string = this->url.substr(qpos + 1);
        for (auto p : std::views::split(this->params_string, '&')) {
            const std::string_view pair{ p.begin(), p.end() };
            const auto split = pair.find('=');
            if (split != std::string_view::npos) {
                this->params.emplace(
                    url::decode(pair.substr(0, split)),
                    url::decode(pair.substr(split + 1))
                );
            } else {
                this->params.emplace(url::decode(pair), "");
            }
        }
    } else {
        this->resource = url::decode(this->url);
        dxx::assert::debug(this->params_string.empty());
    }

    return ret;
} // <-- HeaderError Request::parse_header()

std::expected<std::vector<Request>, MultipartError> Request::parse_multipart() const {
    using namespace std::literals;

    const auto content_type = this->headers.at("Content-Type");
    static constexpr auto prefix = "multipart/form-data; boundary="sv;
    if (!content_type.starts_with(prefix)) {
        return std::unexpected{MultipartError::NotMultipart};
    }

    std::vector<Request> ret;

    const auto bound = std::format(
        "--{}\r\n", content_type.substr(prefix.length())
    );
    const auto bound_end = std::format(
        "--{}--\r\n", content_type.substr(prefix.length())
    );

    auto cur = this->body.find(bound);
    while (cur != std::string::npos && cur + bound_end.size() < this->body.size()) {
        const auto start = cur + bound.size();
        auto next  = this->body.find(bound, cur + 1);
        if (next == std::string::npos) {
            next = this->body.find(bound_end, cur + 1);
        }

        // We expect the sequence to terminate with boundary too
        if (next == std::string::npos) {
            return std::unexpected{MultipartError::Unterminated};
        }

        auto& rq = ret.emplace_back();

        const std::string_view part_data{
            std::next(this->body.cbegin(), start),
            std::next(this->body.cbegin(), next)
        };

        const auto separator = part_data.find(header_separator)
                             + header_separator.size();

        // Cut off the last \r\n of the body
        const auto body_len = part_data.ends_with("\r\n")
                              ? part_data.size() - separator - 2
                              : std::string_view::npos;

        // Multipart part does not contain a header-content separator
        if (separator == std::string_view::npos) {
            return std::unexpected{MultipartError::NoPartBody};
        }

        rq.header = part_data.substr(0, separator);
        rq.body   = part_data.substr(separator, body_len);

        const auto err = rq.parse_header(true);
        switch (err) {
        using enum HeaderError;
        case Unterminated:
            return std::unexpected{MultipartError::UnterminatedHeader};
        case Header:
            return std::unexpected{MultipartError::InvalidHeader};
        case Method:
            // Method is not parsed when going through multipart parts
            dxx::assert::debug(false);
        case None: case StrayData:
            break; // Non-fatal errors
        }

        cur = next;
    }

    return ret;
} // <-- expected<vector<Request>, Error> Request::parse_multipart() const

std::string Response::pack() const {
    std::stringstream ret;
    ret << this->protocol << ' '
        << (
            this->status | dxx::overload::overload{
                [] (const std::string& s) -> std::string_view { return s; },
                [] (StatusCode sc) -> std::string_view {
                    return status_text(sc);
                }
            }
        )
        << "\r\n";
    for (const auto& [ header, value ] : this->headers) {
        ret << header << ':' << value << "\r\n";
    }
    ret << "\r\n" << this->body;
    return ret.str();
} // <-- std::string Response::pack() const

} // <-- namespace dxx::http

module;

#include <arpa/inet.h>
#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

module dxx.http;

import dxx.assert;
import dxx.cstd.fixed;
import dxx.overload;
import std;

import :coro;
import :errors;
import :url;
import :utils;

namespace dxx::http {

namespace {

template <typename T>
class StreamReader {
public:
    StreamReader(int c_fd) : fd(c_fd) {}

    inline void next() {
        auto read_bytes = read(this->fd, &this->cur, sizeof(T));
        if (read_bytes == -1) {
            throw StreamError{ Errno{}, "Can't read bytes from stream" };
        }
        if (read_bytes == 0) {
            this->fd = -1;
            return;
        }
        if (read_bytes < sizeof(T)) {
            throw StreamError{
                "Can't read a full object: {}/{} bytes read",
                read_bytes, sizeof(T)
            };
        }
    } // <-- void StreamReader::next()

    [[nodiscard]]
    inline const T& get() const { return this->cur; }

    [[nodiscard]]
    inline bool end() const { return this->fd == -1; }

private:
    T   cur;
    int fd;
}; // <-- struct StreamReader<T>

std::expected<Request, RequestError> read_request(int stream) {
    Request request{};
    request.header.reserve(1024);

    StreamReader<char> reader{ stream };
    while (!reader.end() && !request.header.ends_with("\r\n\r\n")) {
        reader.next();
        request.header.append(1, reader.get());
    }

    const auto hdr_err = request.parse_header();

    switch (hdr_err) {
    using enum HeaderError;
    case Unterminated:
        return std::unexpected{
            RequestError{ "Header data ended, but header was not terminated" }
        };
    case Method:
        return std::unexpected{
            RequestError{ "Failed to parse request method string" }
        };
    case Header:
        return std::unexpected{
            RequestError{ "Failed to parse request headers" }
        };
    case None: case StrayData:
        // Non-fatal error or no error
        break;
    }

    const auto cl = request.headers.find("content-length");
    if (cl != request.headers.cend()) {
        // Load the content body
        uz len = std::stoll(std::string{cl->second});
        while (len != 0) {
            if (reader.end()) {
                std::println(std::cerr, "Full request: {}", request.header);
                return std::unexpected{
                    RequestError{
                        "Stream closed before all content could be read. "
                        "{} unread bytes remain", len
                    }
                };
            }
            reader.next();
            request.body.append(1, reader.get());
            --len;
        }
    }

    return request;
} // <-- Request read_request(int stream)

} // <-- namespace <anonymous>

SimpleTask Server::handle_connection(
    utils::File stream,
    PendingConnection<std::string>& conn_handle,
    Request& request
) {
    /*
     * If the request cannot be parsed, avoid proceeding to co_await
     * Return immediately, the listener code will handle this case and
     * not transfer ownership to `conn_handle`
     */
    try {
        if (auto req = read_request(stream); req.has_value()) {
            request = std::move(req.value());
            const std::string_view head_v{ request.header };
            std::println("{}", head_v.substr(0, head_v.find('\n')));
        } else {
            std::println(std::cerr, "Request error: {}", req.error().what());
            co_return;
        }
    } catch (const StreamError& se) {
        std::println(std::cerr, "Network failure: {}", se.what());
        co_return;
    }

#if 0
    std::println(std::cerr, "Headers:  {}", request.headers);
    std::println(std::cerr, "Resource: {}", request.resource);
    std::println(std::cerr, "Params:   {}", request.params);
    std::println(std::cerr, "Body:\n{}",    request.body);
#endif

    const auto res = co_await conn_handle;
    if (!this->running || !res.has_value()) {
        /*
         * If the server is not running anymore, we should avoid using anything
         * - the socket might be already closed at this point!
         */
        std::println(std::cerr, "Shutdown or no data provided - skip sending");
        co_return;
    }

    if (send(stream, res->data(), res->length(), 0) == -1) {
        throw ServerError{ Errno{}, "Cannot send data to client" };
    }
} // <-- SimpleTask Server::handle_connection(stream, conn_handle)

SimpleTask Server::listen_and_wait(const std::string& ip, u16 port) {
    sockaddr_in addr{ .sin_family = AF_INET, .sin_port = htons(port) };
    if (inet_aton(ip.data(), &addr.sin_addr) == 0) {
        throw ServerError{ "Cannot parse the addr of the server ({})", ip };
    }

    // Open socket
    const utils::File s_fd{ socket(AF_INET, SOCK_STREAM, PF_UNSPEC) };

    if (s_fd == -1) {
        throw ServerError{ Errno{}, "Cannot open socket" };
    }

    static constexpr int on = 1;
    if (setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        throw ServerError{ Errno{}, "Cannot set socket options" };
    }

    if (fcntl(s_fd, F_SETFL, O_NONBLOCK) == -1) {
        throw ServerError{ Errno{}, "Cannot set socket to NONBLOCK" };
    }

    if (::bind(s_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        throw ServerError{ Errno{}, "Cannot bind socket" };
    }

    if (::listen(s_fd, SOMAXCONN) == -1) { // TODO: customize backlog?
        throw ServerError{ Errno{}, "Cannot listen on the socket" };
    }

    this->running = true;
    while (true) {
        co_await std::suspend_always{};

        if (!this->running) {
            break;
        }

        sockaddr_in client_addr;
        socklen_t   client_addr_size = sizeof(client_addr);

        const int c_fd = accept(
            s_fd,
            reinterpret_cast<sockaddr*>(&client_addr),
            &client_addr_size
        ); // <-- c_fd
        if (c_fd == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror(nullptr);
                throw ServerError{ Errno{}, "Cannot accept client" };
            }
            // Non-blocking wait didn't find any clients
            continue;
        }
        std::print(
            "[{:%F %T} from {}:{}] ",
            std::chrono::system_clock::now(),
            inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port)
        );

        // Connection handler to be given to the user
        auto conn = std::make_unique<PendingConnection<std::string>>();
        // Request by the client
        Request req;

        auto task = this->handle_connection(c_fd, *conn, req);
        if (task.get_handle().done()) {
            std::println(
                std::cerr,
                "Error during request processing, do not proceed"
            );
        } else {
            // Default suggested response
            Response res{
                .protocol = std::string{req.protocol},
                .status   = StatusCode::OK,
                .headers  = { { "Connection", "close" } },
            };
            // Find the route for the request to go to
            const auto router = this->dispatchers.find(req.method);
            if (router != this->dispatchers.cend()) {
                // FIXME: `/` in the name that is not a separator
                auto* target = router->second.find(req.resource, &req.substs);
                if (target != nullptr) {
                    dxx::assert::debug(target->get_data().has_value());
                    target->get_data().value() | dxx::overload::overload{
                        [&] (const DispatcherImmediate& di) {
                            di(req, res);
                            conn->respond_with(res);
                        },
                        [&] (const DispatcherDelayed& dd) {
                            conn->own(std::move(task));
                            dd(req, res, std::move(conn));
                        },
                    };
                    continue;
                }
            }

            res.status = StatusCode::NotFound;
            res.headers["Content-Type"] = "text/html";
            res.body = std::format(
                "Could not find resource `{}` on the server",
                req.resource
            );
            conn->respond_with(res);
        }
    }
} // <-- SimpleTask Server::listen_and_wait(ip, port) const

void Server::listen(const std::string& ip, u16 port) {
    auto task = this->listen_and_wait(ip, port);
    while (!task.get_handle().done()) {
        task.force_resume();
    }
} // <-- void Server::listen(ip, port) const

} // <-- namespace dxx::http

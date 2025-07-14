#include <csignal>

import dxx.assert;
import dxx.cstd.fixed;
import dxx.cstd.compat;
import dxx.http;
import std;

static dxx::http::Server srv;

static constexpr auto root = R"html(
<h1><code>dxx::http</code> demo</h1>
<hr>
<a href="/static">Browse server's current directory</a>
<hr>
<a href="/chat">Simple chat demo (long polling)</a>
<hr>
<input
    type="text"
    id="i_echo"
    placeholder="URL matching demo 1"
    onchange="window.location.href = '/echo/' + i_echo.value"
/><hr>
URL matching demo 2: 
<input type="text" id="i_from" value="Me" />
<input type="text" id="i_how" value="says" /> to 
<input type="text" id="i_to" value="self" />: 
<input type="text" id="i_what" value="Hi!" />
<button
    onclick="window.location.href = '/users/'
             + i_from.value + '/say_to/'
             + i_to.value + '/'
             + i_what.value + '?action=' + i_how.value"
>Go!</button>
<hr>
<form action="/upload" method="post" enctype="multipart/form-data">
File upload demo:
<input type="file" onchange="this.form.submit()" name="demo_file" multiple/>
</form>
)html";

static constexpr auto chat = R"html(
<html>
    <head>
        <title>Test lobby</title>
        <style>
        body { display: flex; flex-direction: column; }
        body > * { width: 100%; }
        #box_msg_list { flex: 85vh; }
        #i_message    { flex: 7vh;  }
        </style>
    </head>
    <body>
        <div id="box_msg_list">
            <i>Messages will be displayed here</i>
        </div>
        <input type="text" id="i_message" />
        <script language="javascript">
        console.log("Starting up...");
        i_message.addEventListener(
            "change", e => {
                console.log("Send " + i_message.value);
                let request = new Request(
                    "/chat/send?token=" + token, {
                        method: "POST",
                        body: i_message.value
                    }
                );
                fetch(request).then(r => console.log(r));
                i_message.value = "";
            }
        );

        let token = null;

        function get_messages() {
            let request = new Request(
                "/chat/update", {
                    method: "POST",
                }
            );
            let controller = new AbortController();
            if (token !== null) {
                request = new Request(
                    "/chat/update?token=" + token, {
                        method: "POST",
                        signal: controller.signal,
                    }
                );
            }
            fetch(request).then(
                r => {
                    console.log(r);
                    if (r.ok) {
                        return r.json();
                    }
                }
            ).then(
                json => {
                    console.log(json);
                    if (json) {
                        if (json.token) {
                            token = json.token;
                            console.log("Got token " + token);
                        }

                        if (json.body) {
                            let br   = document.createElement("br");
                            let text = document.createTextNode(json.body);
                            box_msg_list.appendChild(br);
                            box_msg_list.appendChild(text);
                        }
                    }
                    get_messages();
                }
            );
        }

        get_messages();
        </script>
    </body>
</html>
)html";

void sigterm(int) {
    std::println("Interrupt signal detected. Stopping the server...");
    srv.request_stop();
} // <-- int sigterm(int)

int main(int argc, char** argv) {
    std::signal(SIGINT, sigterm);

    if (argc != 3) {
        std::println(std::cerr, "Usage: {} ip port", argv[0]);
        return EXIT_FAILURE;
    }

    srv.bind(
        dxx::http::Method::Get, "/", [] (const auto& req, auto& res) {
            res.headers["Content-Type"] = "text/html";
            res.body = root;
        }
    );

    srv.bind(
        dxx::http::Method::Get, "/echo/##", [] (const auto& req, auto& res) {
            res.headers["Content-Type"] = "text/html";
            res.body = req.substs.at(0);
        }
    );

    srv.bind(
        dxx::http::Method::Get,
        "/users/#/say_to/#/##",
        [] (const auto& req, auto& res) {
            res.headers["Content-Type"] = "text/html";
            const auto act = req.params.find("action");
            res.body = std::format(
                "<b>@{}</b><i> {} to </i><b>@{}</b>: {}",
                req.substs.at(0),
                act == req.params.cend() ? "says" : act->second,
                req.substs.at(1),
                req.substs.at(2)
            );
        }
    );

    const auto directory_browser = [] (const auto& req, auto& res) {
        std::stringstream str;

        res.headers["Content-Type"] = "text/html";

        const std::filesystem::path p{
            req.substs.empty() ? "." : req.substs.at(0)
        }; // <-- p
        if (!std::filesystem::exists(p)) {
            res.body = std::format("./{}: file not found", p.string());
            return;
        }
        if (!std::filesystem::is_directory(p)) {
            const auto as   = req.params.find("as");
            if (as == req.params.cend()) {
                const auto mime = dxx::http::mime_types.find(p.extension());
                if (mime == dxx::http::mime_types.cend()) {
                    res.status = dxx::http::StatusCode::InternalServerError;
                    res.body   = std::format(
                        "Unrecognized mime type for `{}`, try `?as=...`",
                        p.extension().string()
                    );
                    return;
                } else {
                    res.headers["Content-Type"] = mime->second;
                }
            } else {
                res.headers["Content-Type"] = as->second;
            }
            std::ifstream fs{ p };
            if (!fs.good()) {
                res.status = dxx::http::StatusCode::InternalServerError;
                res.body   = std::format("Unable to open `{}`", p.string());
                return;
            }
            res.body = std::string{
                std::istreambuf_iterator<char>{fs},
                std::istreambuf_iterator<char>{},
            };
            return;
        }
        for (auto e : std::filesystem::directory_iterator(p)) {
            if (std::filesystem::is_directory(e.path())) {
                str << "[d] ";
            } else {
                str << "[ ] "; 
            }
            const auto ep = std::filesystem::relative(e.path(), p);
            str << "<a href="
                << std::quoted((req.resource / ep).string())
                << ">" << ep.string() << "</a><br>";
        }

        res.body = str.str();
    }; // <-- directory_browser(req, res)

    srv.bind(dxx::http::Method::Get, "/static/##", directory_browser);
    /*
     * Have to bind the endpoint itself too - the pattern will only match
     * URLs that have parts after it
     */
    srv.bind(dxx::http::Method::Get, "/static",    directory_browser);

    srv.bind(dxx::http::Method::Get, "/chat", [] (const auto&, auto& res) {
        res.headers["Content-Type"] = "text/html";
        res.body = chat;
    });

    std::map<uz, std::unique_ptr<dxx::http::PendingConnection<std::string>>> conns;

    const auto send_to_all = [&conns] (const auto& p, const auto& message) {
        const dxx::http::Response mr{
            .protocol = p,
            .status   = dxx::http::StatusCode::OK,
            .headers  = {
                { "Content-Type", "application/json" },
                { "Connection",   "close" },
            },
            .body = (
                std::stringstream{} << "{ \"body\": "
                    << std::quoted(message) << " }"
            ).str()
        };
        for (auto& [ _, conn ] : conns) {
            conn->respond_with(mr);
        }
    }; // <-- send_to_all

    srv.bind(
        dxx::http::Method::Post, "/chat/send",
        [&conns, &send_to_all] (const auto& req, auto& res) {
            const auto token = req.params.find("token");
            if (token == req.params.cend()) {
                res.status = dxx::http::StatusCode::Forbidden;
                return;
            }

            try {
                const auto user = std::stoll(token->second);

                if (!conns.contains(user)) {
                    res.status = dxx::http::StatusCode::Forbidden;
                    return;
                }

                if (!req.body.empty()) {
                    const auto message = std::format(
                        "[{:%F %T}] {}: {}",
                        std::chrono::system_clock::now(),
                        token->second,
                        req.body
                    );
                    send_to_all(std::string{req.protocol}, message);
                }
                res.body = "Successfully sent";
            } catch (...) {
                res.status = dxx::http::StatusCode::Forbidden;
            }
        }
    );

    srv.bind(
        dxx::http::Method::Post, "/chat/update",
        [&conns, &send_to_all] (const auto& req, auto res, auto ch) {
            const auto token = req.params.find("token");
            auto dummy_response = res;

            if (token == req.params.cend()) {
                std::println("New chat login");
                for (uz i = 0; i < std::numeric_limits<uz>::max(); ++i) {
                    if (conns.contains(i)) {
                        continue;
                    }
                    std::println("Assign token {}", i);
                    const auto [ it, _ ] = conns.emplace(i, std::move(ch));
                    res.headers["Content-Type"] = "application/json";
                    res.body = std::format("{} \"token\": {} {}", '{', i, '}');
                    it->second->respond_with(res);
                    send_to_all(
                        std::string{req.protocol},
                        std::format("{} connected", i)
                    );
                    return;
                }
            }

            try {
                const auto conn = conns.find(std::stoll(token->second));
                if (conn == conns.cend()) {
                    res.status = dxx::http::StatusCode::Forbidden;
                    res.body = "{ \"reason\": \"No such user\" }";
                    ch->respond_with(res);
                    return;
                }

                /*
                 * Close the connection properly with a response, or the client
                 * might wait on an open connection (literally how?)
                 */
                dummy_response.status = dxx::http::StatusCode::RequestTimeout;
                conn->second->respond_with(dummy_response);
                conn->second = std::move(ch);
            } catch (...) {
                res.status = dxx::http::StatusCode::Forbidden;
                res.body = "{ \"reason\": \"Invalid token\" }";
                ch->respond_with(res);
            }
        }
    ); // <-- /chat/update

    srv.bind(
        dxx::http::Method::Post, "/upload", [] (const auto& req, auto& res) {
            std::println("Headers: {}", req.headers);
            std::println("Params:  {}", req.params);
            // std::println("Request body:");
            // std::println("{}", req.body);
            const auto multipart = req.parse_multipart();
            if (multipart.has_value()) {
                const auto& parts = multipart.value();

                if (parts.size() == 0) {
                    res.status = dxx::http::StatusCode::BadRequest;
                    res.body = "No entries in multipart request";
                    return;
                }

                if (parts.size() == 1) {
                    const auto& p = parts.front();
                    res.headers["Content-Type"] = p.headers.at("Content-Type");
                    res.body                    = p.body;
                    return;
                }

                res.headers["Content-Type"] = "text/html";
                res.body += "<table><tr><th>Filename</th><th>Content-type</th></tr>";
                for (const auto& p : parts) {
                    const auto cd = p.headers.find("Content-Disposition");
                    const auto ct = p.headers.find("Content-Type");

                    std::string fname = "[unnamed]";
                    if (cd != p.headers.cend()) {
                        using namespace dxx::http;
                        const auto cdm = parse_content_disposition(cd->second);
                        if (auto n = cdm.find("filename"); n != cdm.cend()) {
                            fname = n->second;
                        }
                    }

                    res.body += std::format(
                        "<tr><td>{}</td><td>{}</td></tr>\n",
                        fname, ct != p.headers.cend() ? ct->second : "[unknown]"
                    );
                }
                res.body += "</table>";
            } else {
                const auto& err = multipart.error();
                res.status = dxx::http::StatusCode::BadRequest;
                res.body   = std::format(
                    "Error parsing multipart request ({})",
                    std::to_underlying(err)
                );
            }
        }
    ); // <-- /upload

    auto listener = srv.listen_and_wait(argv[1], std::stoi(argv[2]));
    while (!listener.get_handle().done()) {
        // Try to accept a new connection
        listener.force_resume();

        for (auto& conn : conns) {
        }
    }
} // <-- int main(argc, argv)

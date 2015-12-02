
#pragma once


cs477::net::http_response get_image(const cs477::net::http_request &request);
cs477::net::http_response get_images();
cs477::net::http_response get_image(int id);

cs477::net::http_response post_image(const cs477::net::http_request &request);
cs477::net::http_response post_image(const std::string &name);


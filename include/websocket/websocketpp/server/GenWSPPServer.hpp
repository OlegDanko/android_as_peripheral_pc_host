#pragma once
#include "WSPPServerDefs.hpp"

std::unique_ptr<Class> gen_websocketpp_server(int port, remote_created_callback callback);



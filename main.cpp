#include "src/server.h"

int main () {
#ifdef _WIN32
    rv_server::RVServer s;
#else
    rv_server::RVServer s ( "192.168.0.105" );
#endif
    s.run_server();
}

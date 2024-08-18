#include <csignal>
#include <cstring>
#include <iostream>

#include "../../common/common_constants.h"
#include "Database.hpp"
#include "Fix.hpp"
#include "HTTPServer.hpp"
#include "UDP.hpp"
#include "constants.hpp"

std::function<void(int)> signal_handler;
void c_signal_handler(int signal) {
    signal_handler(signal);
}

int main(void) {
    for (int i = 0; i < NSIG; i++) {
        signal(i, c_signal_handler);
    }

    zouipocar::Database db(zouipocar::DB_PATH);
    auto last_fix = db.get_last_fix();

    zouipocar::HTTPServer svr(zouipocar::WEB_UI_PATH, &db);

    zouipocar::UDP udp(ZOUIPOCAR_PORT, [&last_fix, &db, &svr](const Fix &fix) {
        if (last_fix.has_value() && fix.timestamp <= last_fix->timestamp) {
            return;
        }

        // Don't store fix in DB if the tracker is not (or barely) moving.
        if (fix.speed > 5) {
            db.insert_fix(fix);
        }

        last_fix = fix;
        svr.update_fix(fix);
    });

    signal_handler = [&svr](int signal) {
        std::cout << "Received signal " << strsignal(signal) << std::endl;
        if (signal == SIGINT || signal == SIGTERM) {
            svr.stop();
        }
    };

    svr.listen("0.0.0.0", ZOUIPOCAR_PORT);

    return 0;
}

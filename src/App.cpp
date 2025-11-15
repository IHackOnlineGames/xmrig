#include <cstdlib>
#include <uv.h>

#include "App.h"
#include "backend/cpu/Cpu.h"
#include "base/io/Console.h"
#include "base/io/log/Log.h"
#include "base/io/log/Tags.h"
#include "base/io/Signals.h"
#include "base/kernel/Platform.h"
#include "core/config/Config.h"
#include "core/Controller.h"
#include "Summary.h"
#include "version.h"

namespace xmrig {

App::App(Process *process) noexcept :
    m_controller(std::make_shared<Controller>(process))
{
}

App::~App() noexcept
{
    Cpu::release();
}

int App::exec()
{
    if (!m_controller->isReady()) {
        LOG_EMERG("no valid configuration found, try https://xmrig.com/wizard");
        return 2;
    }

    int rc = 0;

    // If background() returns true, rc contains exit code
    if (background(rc)) {
        return rc;
    }

    // Signals RAII object
    m_signals = std::make_shared<Signals>(this);

    rc = m_controller->init();
    if (rc != 0) {
        return rc;
    }

    if (!m_controller->isBackground()) {
        m_console = std::make_shared<Console>(this);
    }

    Summary::print(m_controller.get());

    if (m_controller->config()->isDryRun()) {
        LOG_NOTICE("%s " WHITE_BOLD("OK"), Tags::config());
        return 0;
    }

    m_controller->start();

    rc = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    uv_loop_close(uv_default_loop());

    return rc;
}

void App::onConsoleCommand(char command)
{
    if (command == 3) {  // Ctrl + C
        LOG_WARN("%s " YELLOW("Ctrl+C received, exiting"), Tags::signal());
        close();
        return;
    }

    m_controller->execCommand(command);
}

void App::onSignal(int signum)
{
    switch (signum) {
    case SIGHUP:
    case SIGTERM:
    case SIGINT:
        close();
        break;
    default:
        break;
    }
}

void App::close() noexcept
{
    m_signals.reset();
    m_console.reset();

    m_controller->stop();

    Log::destroy();
}

} // namespace xmrig

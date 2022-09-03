#include <common/utils/log.hpp>
#include <common/utils/wait_signal.hpp>

#include <csignal>


void common::waitSignal()
{
    sigset_t signalSet;
    sigemptyset(&signalSet);

    sigaddset(&signalSet, SIGINT);
    sigaddset(&signalSet, SIGTERM);
    sigaddset(&signalSet, SIGQUIT);

    sigprocmask(SIG_BLOCK, &signalSet, nullptr);

    int signal = 0;
    sigwait(&signalSet, &signal);

    Log(info) << "Got signal " << signal;
}
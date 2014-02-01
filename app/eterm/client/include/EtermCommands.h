#ifndef ETERM_COMMANDS_H
#define ETERM_COMMANDS_H

#include "InteractiveCommand.h"
#include "HeimdallControllerClientStub.h"

namespace eterm {

class LedCommand : public liber::cli::InteractiveCommand {
public:

  LedCommand(HeimdallControllerClientStub& rHeimdall);

  void process(const liber::cli::ArgumentList& args);

private:

  HeimdallControllerClientStub& mrHeimdall;
};

// Fingerprint Status Command: Get fingerprint read status
class FpStatusCommand : public liber::cli::InteractiveCommand {
public:

  FpStatusCommand(HeimdallControllerClientStub& rHeimdall);

  void process(const liber::cli::ArgumentList& args);

private:

  HeimdallControllerClientStub& mrHeimdall;
};

}

#endif // ETERM_COMMANDS_H


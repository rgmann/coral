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


class ActivateDoorCommand : public liber::cli::InteractiveCommand {
public:

  ActivateDoorCommand(HeimdallControllerClientStub& rHeimdall);

  void process(const liber::cli::ArgumentList& args);

private:

  HeimdallControllerClientStub& mrHeimdall;
};


class EnrollUserCommand : public liber::cli::InteractiveCommand {
public:

  EnrollUserCommand(HeimdallControllerClientStub& rHeimdall);

  void process(const liber::cli::ArgumentList& args);

private:

  HeimdallControllerClientStub& mrHeimdall;
};


class RemoveOneCommand : public liber::cli::InteractiveCommand {
public:

  RemoveOneCommand(HeimdallControllerClientStub& rHeimdall);

  void process(const liber::cli::ArgumentList& args);

private:

  HeimdallControllerClientStub& mrHeimdall;
};


class RemoveAllCommand : public liber::cli::InteractiveCommand {
public:

  RemoveAllCommand(HeimdallControllerClientStub& rHeimdall);

  void process(const liber::cli::ArgumentList& args);

private:

  HeimdallControllerClientStub& mrHeimdall;
};



class PrintUsersCommand : public liber::cli::InteractiveCommand {
public:

  PrintUsersCommand(HeimdallControllerClientStub& rHeimdall);

  void process(const liber::cli::ArgumentList& args);

private:

  HeimdallControllerClientStub& mrHeimdall;
};


} // End namespace eterm

#endif // ETERM_COMMANDS_H


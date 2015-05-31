// KFoundation
#include <kfoundation/Ptr.h>

// KnoRBA
#include <knorba/Agent.h>
#include <knorba/type/all.h>
#include <knorba/Runtime.h>

// Self
#include "EchoAgent.h"

using namespace std;

EchoAgent::PConsole::PConsole(Agent* agent)
: ConsoleProtocolClient(agent)
{
  // Nothing;
}


void EchoAgent::PConsole::onInputReceived(PPtr<KString> input) {
  if(input->equals("quit")) {
    print("Bye!");
    _agent->quit();
  } else {
    print(input);
  }
}


EchoAgent::EchoAgent(Runtime& rt, const k_guid_t& guid)
: Agent(rt, guid),
_pConsole(this)
{
  _pConsole.setServer(getRuntime().getConsoleGuid());
  _pConsole.subscribe();
  _pConsole.print("Enter anything to echo. Enter \"quit\" to finish.");
}


KF_EXPORT
void init(Runtime& rt) {
  ConsoleProtocolClient::init(rt);
}


KF_EXPORT
Agent* instantiate(Runtime& rt, const k_guid_t& guid) {
  return new EchoAgent(rt, guid);
}
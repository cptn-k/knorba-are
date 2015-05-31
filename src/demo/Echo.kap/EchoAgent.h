#ifndef ARE_DEMO_ECHOAGENT
#define ARE_DEMO_ECHOAGENT

#include <knorba/protocol/ConsoleProtocolClient.h>

using namespace knorba;
using namespace knorba::type;
using namespace knorba::protocol;

class EchoAgent : public Agent {
  
  // --- NESTED TYPES --- //
  
  private: class PConsole : public ConsoleProtocolClient {
    public: PConsole(Agent* agent);
    public: void onInputReceived(PPtr<KString> input);
  };
  
  // --- FIELDS --- //
  
  private: PConsole _pConsole;
  
  
  // --- CONSTRUCTOR --- //
  
  public: EchoAgent(Runtime& rt, const k_guid_t& guid);
  
};

extern "C" void init(Runtime& rt);
extern "C" knorba::Agent* instantiate(Runtime& rt, const k_guid_t& guid);

#endif /* defined(ARE_DEMO_ECHOAGENT) */

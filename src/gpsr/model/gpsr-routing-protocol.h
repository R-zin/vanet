#ifndef GPSR_ROUTING_PROTOCOL_H

#define GPSR_ROUTING_PROTOCOL_H

#include "ns3/ipv4-routing-protocol.h"

#include "ns3/ipv4-address.h”

#include "ns3/timer.h"

#include "ns3/mobility-model.h"

#include "ns3/node.h"

#include "ns3/net-device.h"

#include "ns3/ipv4.h"

#include "ns3/ipv4-routing-table-entry.h"

#include <map>

namespace ns3 {

class GpsrRoutingProtocol : public Ipv4RoutingProtocol

{

public:

    static TypeId GetTypeId (void);

    GpsrRoutingProtocol ();

    virtual ~GpsrRoutingProtocol ();

    // Inherited from Ipv4RoutingProtocol

    virtual Ptr<Ipv4Route> RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);

    virtual bool RouteInput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,

                             UnicastForwardCallback ucb, MulticastForwardCallback mcb, LocalDeliverCallback lcb, ErrorCallback ecb);

    virtual void NotifyInterfaceUp (uint32_t interface);

    virtual void NotifyInterfaceDown (uint32_t interface);

    virtual void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address);

    virtual void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address);

    virtual void SetIpv4 (Ptr<Ipv4> ipv4);

    virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const;

private:
    Ptr<Ipv4> m_ipv4;
    std::map<Ipv4Address, Ipv4RoutingTableEntry> m_routingTable;
    Ptr<NetDevice> GetNetDevice (uint32_t interface);
    Ptr<MobilityModel> GetMobilityModel (Ptr<NetDevice> netDevice);
    Ipv4Address GetNextHop (Ipv4Address dest);

};
} // namespace ns3
#endif /* GPSR_ROUTING_PROTOCOL_H */

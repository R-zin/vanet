#include "gpsr-routing-protocol.h"
#include <limits>
#include "ns3/log.h"
#include "ns3/ipv4-route.h"
#include "ns3/simulator.h"
namespace ns3 {
NS_LOG_COMPONENT_DEFINE ("GpsrRoutingProtocol");
NS_OBJECT_ENSURE_REGISTERED (GpsrRoutingProtocol);
TypeId

GpsrRoutingProtocol::GetTypeId (void)

{

  static TypeId tid = TypeId ("ns3::GpsrRoutingProtocol")

    .SetParent<Ipv4RoutingProtocol> ()

    .SetGroupName("Internet")

    .AddConstructor<GpsrRoutingProtocol> ();

  return tid;

}

GpsrRoutingProtocol::GpsrRoutingProtocol ()

{

}

GpsrRoutingProtocol::~GpsrRoutingProtocol ()

{

}

void

GpsrRoutingProtocol::SetIpv4 (Ptr<Ipv4> ipv4)

{

  m_ipv4 = ipv4;

}

Ptr<NetDevice>

GpsrRoutingProtocol::GetNetDevice (uint32_t interface)

{

  return m_ipv4->GetNetDevice (interface);

}

Ptr<MobilityModel>

GpsrRoutingProtocol::GetMobilityModel (Ptr<NetDevice> netDevice)

{

  return netDevice->GetNode ()->GetObject<MobilityModel> ();

}

Ipv4Address

GpsrRoutingProtocol::GetNextHop (Ipv4Address dest)

{

  // Simplified example of next hop selection based on GPSR

  Ipv4Address nextHop = Ipv4Address::GetBroadcast ();

  double minDistance = std::numeric_limits<double>::max ();

  Ptr<MobilityModel> destMobility = GetMobilityModel (m_ipv4->GetNetDevice (m_ipv4->GetInterfaceForAddress (dest)));

  Vector destPosition = destMobility->GetPosition ();

  for (auto const &entry : m_routingTable)

    {

      Ptr<MobilityModel> neighborMobility = GetMobilityModel (m_ipv4->GetNetDevice (entry.second.GetInterface ()));

      Vector neighborPosition = neighborMobility->GetPosition ();

      double distance = CalculateDistance (neighborPosition, destPosition);

      if (distance < minDistance)

        {

          minDistance = distance;

          nextHop = entry.first;

        }

    }

  return nextHop;

}

Ptr<Ipv4Route>

GpsrRoutingProtocol::RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)

{

  Ipv4Address dest = header.GetDestination ();

  Ipv4Address nextHop = GetNextHop (dest);

  if (nextHop == Ipv4Address::GetBroadcast ())

    {

      sockerr = Socket::ERROR_NOROUTETOHOST;

      return nullptr;

    }

  Ptr<Ipv4Route> route = Create<Ipv4Route> ();

  route->SetDestination (dest);

  route->SetGateway (nextHop);

  route->SetOutputDevice (m_ipv4->GetNetDevice (m_ipv4->GetInterfaceForAddress (nextHop)));

  return route;

}

bool

GpsrRoutingProtocol::RouteInput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,

                                 UnicastForwardCallback ucb, MulticastForwardCallback mcb, LocalDeliverCallback lcb, ErrorCallback ecb)

{

  Ipv4Address dest = header.GetDestination ();

  Ipv4Address nextHop = GetNextHop (dest);

  if (nextHop == Ipv4Address::GetBroadcast ())

    {

      ecb (p, header, Socket::ERROR_NOROUTETOHOST);

      return false;

    }

  Ptr<Ipv4Route> route = Create<Ipv4Route> ();

  route->SetDestination (dest);

  route->SetGateway (nextHop);

  route->SetOutputDevice (m_ipv4->GetNetDevice (m_ipv4->GetInterfaceForAddress (nextHop)));

 

  ucb (route, p, header);

  return true;

}

void

GpsrRoutingProtocol::NotifyInterfaceUp (uint32_t interface)

{

}

void

GpsrRoutingProtocol::NotifyInterfaceDown (uint32_t interface)

{

}

void

GpsrRoutingProtocol::NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address)

{

}

void

GpsrRoutingProtocol::NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address)

{

}

void

GpsrRoutingProtocol::PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const

{

  *stream->GetStream () << "GPSR Routing Table" << std::endl;

  for (auto const &entry : m_routingTable)

    {

      *stream->GetStream () << entry.first << " -> " << entry.second.GetGateway () << " via " << entry.second.GetInterface () << std::endl;
    }
}
} // namespace ns3
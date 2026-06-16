#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/internet-module.h"
#include "ns3/aodv-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include  "ns3/yans-wifi-helper.h"


#include <fstream>
#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("VanetAodvCsv");

double g_firstTxTime = -1.0;
double g_firstRxTime = -1.0;
void FirstTxCallback(Ptr<const Packet> p)
{
    if (g_firstTxTime <0.0)
    {
        g_firstTxTime = Simulator::Now().GetSeconds();
    }
}
void FirstRxCallback(Ptr<const Packet> p,const Address &addr)
{
    if (g_firstRxTime <0.0)
    {
        g_firstRxTime = Simulator::Now().GetSeconds();
    }
}

uint32_t routingTxPackets = 0;

void RoutingTxTrace(const Ipv4Header &header,Ptr<const Packet> p,uint32_t interface)
{
    routingTxPackets++;
}

int main(int argc, char *argv[])
{
    uint32_t numNodes = 20;
    uint32_t numFlows = 5;
    double simTime = 100.0;
    std::string mobilityTrace = "kochivanet.ns_movements";
    CommandLine cmd;
    cmd.AddValue("numNodes", "Number of nodes", numNodes);
    cmd.AddValue("simTime", "Simulation time", simTime);
    cmd.AddValue("mobilityTrace",
                 "SUMO mobility trace converted to ns-2 format",
                 mobilityTrace);
    cmd.Parse(argc, argv);

    NodeContainer nodes;
    nodes.Create(numNodes);

    // Import mobility generated from SUMO (converted to ns-2 trace format)
    Ns2MobilityHelper ns2 = Ns2MobilityHelper(mobilityTrace);
    ns2.Install();

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211p);

    YansWifiChannelHelper channel;
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    AodvHelper aodv;
    aodv.Set("EnableHello",BooleanValue(false));
    InternetStackHelper internet;
    internet.SetRoutingHelper(aodv);
    internet.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.0.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    for (uint32_t i = 0; i < nodes.GetN(); i++)
    {
        nodes.Get(i)->GetObject<Ipv4>()->TraceConnectWithoutContext("SendOutgoing",MakeCallback(&RoutingTxTrace));
    }

    struct FlowPair {uint32_t src; uint32_t dst;};
    std::vector<FlowPair> pairs;
    for (uint32_t i = 0; i < numNodes; i++)
    {
        for (uint32_t j = 0; j < numNodes; j++)
        {
            if (i == j)
            {
                continue;
            }
            pairs.push_back({i,j});
        }
    }
    uint32_t totalflow = pairs.size();

    uint16_t basePort = 9999;
    ApplicationContainer sinkApps, clientApps;

    double startSpread = std::min(10.0,simTime/4.0);
    double startStep = (totalflow > 1) ? startSpread/(double)(totalflow-1) : 0.0;

    for (uint32_t i = 0; i < totalflow; i++)
    {
        uint32_t src = pairs[i].src;
        uint32_t dst = pairs[i].dst;
        uint16_t port = basePort + i;

        PacketSinkHelper sink("ns3::UdpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(), port));
        auto s = sink.Install(nodes.Get(dst));
        s.Start(Seconds(0.0));
        s.Stop(Seconds(simTime));
        sinkApps.Add(s);

        double startTime = 1.0 + i * startSpread;

        OnOffHelper onoff("ns3::UdpSocketFactory",InetSocketAddress(interfaces.GetAddress(dst), port));
        onoff.SetAttribute("DataRate",   StringValue("64Kbps"));
        onoff.SetAttribute("PacketSize", UintegerValue(512));
        onoff.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onoff.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        auto c = onoff.Install(nodes.Get(src));
        c.Start(Seconds(startTime));
        c.Stop(Seconds(simTime));
        clientApps.Add(c);

    }


    Ptr<OnOffApplication> onoffApp = DynamicCast<OnOffApplication>(clientApps.Get(0));
    onoffApp->TraceConnectWithoutContext("Tx",MakeCallback(&FirstTxCallback));

    Ptr<PacketSink> Sink = DynamicCast<PacketSink>(sinkApps.Get(0));
    Sink->TraceConnectWithoutContext("Rx",MakeCallback(&FirstRxCallback));

    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());

    auto stats = monitor->GetFlowStats();

    std::ofstream csv("aodv_metrics.csv");
    csv << "FlowId,Source,SrcPort,Destination,DstPort,"
    << "TxPackets,RxPackets,LostPackets,PDR(%),AvgDelay(ms),"
    << "Jitter(ms),Throughput(Mbps)\n";

    double totalTxpkt= 0.0;


    for (const auto &flow : stats)
    {
        auto tuple = classifier->FindFlow(flow.first);

        if (tuple.destinationPort==654||tuple.sourcePort==654)
        {
            continue; // skips aodv control Flows
        }
        totalTxpkt += flow.second.txPackets;

        double pdr = 0.0;
        if (flow.second.txPackets > 0)
        {
            pdr = (double)flow.second.rxPackets /
                  (double)flow.second.txPackets * 100.0;
        }

        double avgDelayMs = (flow.second.rxPackets > 0)
            ? flow.second.delaySum.GetSeconds() / flow.second.rxPackets * 1000.0
            : 0.0;
        double jitterMs = (flow.second.rxPackets > 1)
            ? flow.second.jitterSum.GetSeconds()
              / (flow.second.rxPackets - 1) * 1000.0
            : 0.0;
        double duration = flow.second.timeLastRxPacket.GetSeconds() - flow.second.timeFirstTxPacket.GetSeconds();
        double throughput = (duration>0)?(flow.second.rxBytes*8.0)/duration/1000000:0.0;
        csv << flow.first                  << ","
            << tuple.sourceAddress         << ","
            << tuple.sourcePort            << ","
            << tuple.destinationAddress    << ","
            << tuple.destinationPort       << ","
            << flow.second.txPackets       << ","
            << flow.second.rxPackets       << ","
            << flow.second.lostPackets     << ","
            << std::fixed << std::setprecision(4)
            << pdr                         << ","
            << avgDelayMs                  << ","
            << jitterMs                    << ","
            << throughput                  << "\n";
    }

    double ctrlPkts  = (double)routingTxPackets - totalTxpkt;
    double overhead  = (routingTxPackets > 0)
        ? ctrlPkts / (double)routingTxPackets * 100.0
        : 0.0;
    double rdt = (g_firstTxTime >= 0.0 && g_firstRxTime >= 0.0)
        ? g_firstRxTime - g_firstTxTime
        : -1.0;
    csv << "\n# Summary\n";
    csv << "RoutingOverhead(%),"     << std::setprecision(2) << overhead << "\n";
    csv << "RouteDiscoveryTime(s),"  << std::setprecision(4) << rdt      << "\n";
    csv << "TotalRoutingPackets,"    << routingTxPackets               << "\n";
    csv << "TotalDataTxPackets,"     << (uint32_t)totalTxpkt        << "\n";
    csv.close();
    std::cout << "Task finished Successfully";
    Simulator::Destroy();
    return 0;
}
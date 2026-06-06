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

int main(int argc, char *argv[])
{
    uint32_t numNodes = 20;
    double simTime = 300.0;
    std::string mobilityTrace = "sumo.tcl";
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
    wifi.SetStandard(WIFI_STANDARD_80211g);

    YansWifiChannelHelper channel;
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    AodvHelper aodv;
    InternetStackHelper internet;
    internet.SetRoutingHelper(aodv);
    internet.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.0.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    uint16_t port = 9999;

    PacketSinkHelper sinkHelper(
        "ns3::UdpSocketFactory",
        InetSocketAddress(Ipv4Address::GetAny(), port));

    ApplicationContainer sinkApp = sinkHelper.Install(nodes.Get(numNodes - 1));
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(simTime));

    OnOffHelper onoff(
        "ns3::UdpSocketFactory",
        InetSocketAddress(interfaces.GetAddress(numNodes - 1), port));

    onoff.SetAttribute("DataRate", StringValue("1Mbps"));
    onoff.SetAttribute("PacketSize", UintegerValue(512));

    ApplicationContainer client = onoff.Install(nodes.Get(0));
    client.Start(Seconds(1.0));
    client.Stop(Seconds(simTime));

    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());

    auto stats = monitor->GetFlowStats();

    std::ofstream csv("aodv_metrics.csv");
    csv << "FlowId,Source,Destination,TxPackets,RxPackets,PDR,AvgDelaySeconds,ThroughputMbps\n";

    for (const auto &flow : stats)
    {
        auto tuple = classifier->FindFlow(flow.first);

        double pdr = 0.0;
        if (flow.second.txPackets > 0)
        {
            pdr = (double)flow.second.rxPackets /
                  (double)flow.second.txPackets * 100.0;
        }

        double avgDelay = 0.0;
        if (flow.second.rxPackets > 0)
        {
            avgDelay = flow.second.delaySum.GetSeconds() /
                       flow.second.rxPackets;
        }
        double throughput =
            (flow.second.rxBytes * 8.0) /
            simTime /
            1000000.0;
        csv << flow.first << ","
            << tuple.sourceAddress << ","
            << tuple.destinationAddress << ","
            << flow.second.txPackets << ","
            << flow.second.rxPackets << ","
            << std::fixed << std::setprecision(4)
            << pdr << ","
            << avgDelay << ","
            << throughput
            << "\n";
    }

    csv.close();

    Simulator::Destroy();
    return 0;
}

#include "ns3/yans-wifi-helper.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
using namespace ns3;
int main (int argc, char *argv[])
{
    uint32_t numNodes = 20;
    double simTIme = 300;
    std::string mobilityTrace = "sumo.tcl";

    CommandLine cmdline;
    cmdline.AddValue ("simTIme", simTime);
    cmdline.AddValue("mobilityTrace",mobilityTrace);
    cmdline.Parse (argc, argv);

    NodeContainer nodes;
    nodes.Create (numNodes);

    Ns2MobilityHelper mobility = Ns2MobilityHelper(mobilityTrace);
    mobility.Install();

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);

    YansWifiChannelHelper channel;
    channel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");
    channel.SetPropagationDelay("ns3::ConstantPropagationDelayModel");
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac_helper;
    mac_helper.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy,mac_helper,nodes);

    gpsrHelper gpsr;
    InternetStackHelper internet;
    internet.SetRoutingHelper(gpsr);
    internet.Install(nodes);








}
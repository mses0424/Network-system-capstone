#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/leo-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/nix-vector-helper.h"

using namespace ns3;
using namespace std;

// Satellite network setup
uint16_t port = 9;
NodeContainer satellites;
NodeContainer groundStations;
NetDeviceContainer utNet;
NetDeviceContainer islNet;
int Task = 2;


static void EchoMacTxRx(std::string context, const Ptr< const Packet > packet);
void SendPacket(Ptr<Node> src, Ptr<Node> dst);
void connect();
LeoLatLong Vec2LatLong(Vector pos);
LeoLatLong id2LatLong(const NodeContainer &satellites, int id);

static void
EchoMacTxRx (std::string context, const Ptr<const Packet> packet)
{
  uint32_t sz = packet->GetSize ();
  //判斷方法直接看size 把小的都過濾掉
  if (sz < 150) return;
  //context會等於/NodeList/36/DeviceList/0/$ns3::MockNetDevice/MacRx
  //要拿出nodeID從NodeList與DeviceList中間拿
  size_t off = context.find("/NodeList/") + 10;
  size_t end = context.find("/DeviceList", off);
  uint32_t nodeId = std::stoul(context.substr(off, end - off));
  //看是Tx還是Rx
  bool isTx = (context.find("MacTx") != std::string::npos);
  double t_ns = Now().GetNanoSeconds ();
  std::cout << (isTx ? "MacTx" : "MacRx") << " at node: " << nodeId
    << ", now=+" << std::scientific << t_ns << " ns" << std::endl;
}


Ptr<PacketSink>
SendPacket (int srcId, int dstId, uint32_t maxBytes){
    // Task 2.1: Complete this function
    // Task 2.1: Set MaxBytes to 512 & Task 3.1: Set MaxBytes to 0
    Ptr<Node> src = NodeList::GetNode (srcId); //取出起點終點的node
    Ptr<Node> dst = NodeList::GetNode (dstId);
    Ptr<Ipv4> ipv4 = dst->GetObject<Ipv4> (); //取終點的ip address
    Ipv4Address dstAddr = ipv4->GetAddress (1, 0).GetLocal ();
    //在終點裝PacketSink用來接收tcp流量
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
    ApplicationContainer sinkApps = sinkHelper.Install(dst);
    sinkApps.Start (Seconds (0.0));
    Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(0));
    //max=512,sendsize=512 裝BulkSendApplication到起點src上用來做tcp傳送
    BulkSendHelper bulkSender ("ns3::TcpSocketFactory",InetSocketAddress (dstAddr, port));
    bulkSender.SetAttribute ("MaxBytes", UintegerValue (maxBytes)); //task2,3修改maxbyte
    bulkSender.SetAttribute ("SendSize",  UintegerValue (512));
    ApplicationContainer srcApps = bulkSender.Install(src);
    srcApps.Start (Seconds (0.0));
    return sink;
}

void connect() {
    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::MockNetDevice/MacTx", MakeCallback (&EchoMacTxRx));
    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::MockNetDevice/MacRx", MakeCallback (&EchoMacTxRx));
}

// Vec2LatLong function change xyz coordinate to latitude and longtitude
LeoLatLong Vec2LatLong(Vector pos){
    double r = sqrt (pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
    double lat = asin (pos.z / r) * 180.0 / M_PI;
    double longit = atan2 (pos.y, pos.x) * 180 / M_PI;
    return LeoLatLong(lat, longit);
}

LeoLatLong id2LatLong(const NodeContainer &satellites, int id){
    Vector pos = satellites.Get(id)->GetObject<MobilityModel>()->GetPosition();
    return Vec2LatLong(pos);
}

NS_LOG_COMPONENT_DEFINE ("Lab6");

int main (int argc, char *argv[]) {
    CommandLine cmd;
    string constellation = "TelesatGateway";
    double duration = 100;
    string inputFile = "<Your Input File>";
    string outputFile;

    cmd.AddValue("in", "Input File", inputFile);
    cmd.AddValue("out", "Output File", outputFile);
    cmd.AddValue("Task", "2 or 3", Task);
    cmd.Parse (argc, argv);

    // Redirect cout if outputFile is specified
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::ofstream out;
    out.open (outputFile);
    if (out.is_open ()) {
        std::cout.rdbuf(out.rdbuf());
    }
    
    if(Task != 2 && Task != 3){
        cerr<<"Error: Task is not 2 or 3"<<endl;
        exit(1);
    }

    // Default setting
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(512));
    Config::SetDefault("ns3::TcpSocketBase::MinRto", TimeValue(Seconds(2.0)));

    // Satellite
    LeoOrbitNodeHelper orbit;
    satellites = orbit.Install ({ LeoOrbit (1200, 20, 6, 6) });

    // Ground station
    LeoGndNodeHelper ground;
    groundStations.Add(ground.Install(id2LatLong(satellites, 0), id2LatLong(satellites, 2)));  // node id=36, 37
    groundStations.Add(ground.Install(id2LatLong(satellites, 3), id2LatLong(satellites, 9)));  // node id=38, 39
    groundStations.Add(ground.Install(id2LatLong(satellites, 20), id2LatLong(satellites, 34))); // node id=40, 41

    // Set network
    LeoChannelHelper utCh;
    utCh.SetConstellation (constellation);
    utCh.SetGndDeviceAttribute("DataRate", StringValue("11kbps"));
    utNet = utCh.Install (satellites, groundStations);
    IslHelper islCh;
    islNet = islCh.Install (satellites);

    InternetStackHelper stack;
    // Set NVR as routing protocol 
    Ipv4NixVectorHelper nixRouting;
    nixRouting.SetPathFile(inputFile); // Implement the function
    stack.SetRoutingHelper(nixRouting);
    // Install internet stack on nodes
    stack.Install (satellites);
    stack.Install (groundStations);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.0.0", "255.255.0.0");
    ipv4.Assign (utNet);
    ipv4.SetBase ("10.2.0.0", "255.255.0.0");
    ipv4.Assign (islNet);

    // Task 2.2 & Task 3.2 : Call SendPacket()
    vector<pair<int,int>> flows = {{36,38},{37,40},{39,41}};
    
    // Task 3.3 : Install sink applications for each destination
    vector<Ptr<PacketSink>> sinks;
    sinks.reserve(flows.size());
    if(Task == 2) {
      Simulator::Schedule(Seconds(1e-9), &connect);
      sinks.push_back(SendPacket(flows[0].first, flows[0].second, 512));
    }
    else {
      for (auto &f : flows) {
        sinks.push_back(SendPacket(f.first, f.second, 0));
      }
    }

    Simulator::Stop (Seconds (duration));
    Simulator::Run ();
    Simulator::Destroy ();

    // Task 3.4 : Calculate throughput
    if(Task == 3) {
      uint64_t totalBytes = 0;
      for(size_t i = 0; i < flows.size(); ++i) {
        uint64_t bytesRx = sinks[i]->GetTotalRx();
        std::cout << flows[i].first << "->" << flows[i].second << ": " << bytesRx << std::endl;
        totalBytes += bytesRx;
      }
      std::cout << "Total throughput: " << totalBytes << std::endl;
    }
    out.close ();
    std::cout.rdbuf(coutbuf);

    return 0;
}

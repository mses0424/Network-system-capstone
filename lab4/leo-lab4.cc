#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/leo-module.h"
#include "ns3/network-module.h"
#include "ns3/aodv-module.h"
#include "ns3/udp-server.h"

using namespace ns3;


std::map<std::pair<int,int>, int> linkDataRate; //存GS_ID、SAT_ID對應的data rate
std::vector<int> gsToSat; //存GS_ID會傳的SAT_ID 大小是GS數量
std::vector<bool> gsStarted; //用來存GS有沒有傳送封包了
std::vector<double> gsStartTime; //存GS的傳輸開始時間
std::vector<int> satCompletedCount; //跟衛星完成傳輸的地面站數量
Ipv4InterfaceContainer utInterfaces; //存IP
std::vector<double> gsEndTime;  //存GS_ID 傳輸結束時間
std::vector<bool>   satIdle;  //存SAT_ID 是否空閒
std::vector<double> satTotalTime; //存SAT_ID 總共的時間


// Satellite network setup
int port = 9;
NodeContainer satellites;
NodeContainer groundStations;
NetDeviceContainer utNet;
ApplicationContainer sinkApps;

static void EchoRx(std::string context, const Ptr< const Packet > packet, const TcpHeader &header, const Ptr< const TcpSocketBase > socket);
void SendPacket(int gsId, int satId);
string GetNodeId(string str);
void Connect();

static void EchoRx(std::string context, const Ptr< const Packet > packet, const TcpHeader &header, const Ptr< const TcpSocketBase > socket){	
  // Task 3: Complete this function
  int nodeId = std::stoi(GetNodeId(context)); //可以拿到現在node的index
  int numSat = satellites.GetN(); 
  if(nodeId >= numSat) return; //如果index大於衛星數量，代表是GS的index不用考慮
  int satId = nodeId;
  int completed = satCompletedCount[satId];

  uint64_t threshold = 125000 * (completed + 1);
  Ptr<PacketSink> sinkApp = DynamicCast<PacketSink>(sinkApps.Get(satId));
  if (sinkApp->GetTotalRx() < threshold) {
      return;
  }

  int finishedGs = -1; //存結束傳輸的gs index
  for(int gsId = 0; gsId < (int)gsToSat.size(); ++gsId){
    if(gsToSat[gsId] == satId && gsStarted[gsId] && gsEndTime[gsId] == 0.0){
      finishedGs = gsId;  //找GS傳輸的SAT ID有沒有一樣、有沒有開始傳輸跟結束時間=0
      break;
    }
  }
  if(finishedGs < 0){
    return;
  }
  gsEndTime[finishedGs] = Simulator::Now().GetSeconds(); //紀錄結束傳輸時間
  satTotalTime[satId] += gsEndTime[finishedGs] - gsStartTime[finishedGs];
  satIdle[satId] = true;
  satCompletedCount[satId]++;
  for(int gsId = 0; gsId < (int)gsToSat.size(); ++gsId){
    if(!gsStarted[gsId] && gsToSat[gsId] == satId && satIdle[satId]){
      SendPacket(gsId, satId); //如果GS還沒開始傳輸跟衛星是idle的就進行傳輸
      satIdle[satId] = false;
      break;
    }
  }
}

static void HandlePacketTx(int gsId, Ptr<const Packet> packet) {
  if (!gsStarted[gsId]) {
      gsStartTime[gsId] = Simulator::Now().GetSeconds();
      gsStarted[gsId] = true;
  }
}

void SendPacket(int gsId, int satId){
  // Task 2.1: Complete this function
  int datarate = linkDataRate[{gsId, satId}]; //取出現在GS跟SAT連線的data rate 
  std::ostringstream oss;
  oss << (uint64_t)datarate << "kbps";  
  uint32_t gsindex = satellites.GetN() + gsId; //GS的index
  utNet.Get(satId)->GetObject<MockNetDevice>()->SetDataRate(DataRate(oss.str()));
  utNet.Get(gsindex)->GetObject<MockNetDevice>()->SetDataRate(DataRate(oss.str()));
  Ipv4Address satAddr = utInterfaces.GetAddress(satId); //SAT的IP
  
  BulkSendHelper bulkSender("ns3::TcpSocketFactory",InetSocketAddress(satAddr, port));
  bulkSender.SetAttribute("MaxBytes", UintegerValue(125000)); //傳輸到達最大值會停止
  bulkSender.SetAttribute("SendSize", UintegerValue(512));  
  ApplicationContainer srcApp = bulkSender.Install(groundStations.Get(gsId));
  srcApp.Start(Seconds(0.0)); //在GS_ID node上用tcp連到衛星ip，並開始傳輸
  //用bulkApp取出srcApp第一個應用(現在只有一個應用即傳輸)，當真正有呼叫tx把封包傳出去再紀錄時間
  Ptr<BulkSendApplication> bulkApp = DynamicCast<BulkSendApplication>(srcApp.Get(0));
  bulkApp->TraceConnectWithoutContext("Tx", MakeBoundCallback(&HandlePacketTx, gsId));
}

string GetNodeId(string str) {
  // Which node
  size_t pos1 = str.find("/", 0);           // The first "/"
  size_t pos2 = str.find("/", pos1 + 1);    // The second "/"
  size_t pos3 = str.find("/", pos2 + 1);    // The third "/"
  return str.substr(pos2 + 1, pos3 - pos2 - 1); // Node id
}

void Connect(){
  Config::Connect ("/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/Rx", MakeCallback (&EchoRx));
}

NS_LOG_COMPONENT_DEFINE ("Lab4");

int main(int argc, char *argv[]){

  CommandLine cmd;
  string constellation = "TelesatGateway";
  double duration = 100;
  string inputFile = "network.ortools.out";
  string outputFile = "lab4.ortools.out";

  cmd.AddValue("duration", "Duration of the simulation in seconds", duration);
  cmd.AddValue("constellation", "LEO constellation link settings name", constellation);
  cmd.AddValue("inputFile", "Input file", inputFile);
  cmd.AddValue("outputFile", "Output file", outputFile);
  cmd.Parse (argc, argv);

  // Default setting
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(512));
  Config::SetDefault("ns3::TcpSocketBase::MinRto", TimeValue(Seconds(2.0)));

  // Satellite
  LeoOrbitNodeHelper orbit;
  satellites = orbit.Install({ LeoOrbit(1200, 20, 1, 60)});

  // Ground station
  LeoGndNodeHelper ground;
  ground.Add(groundStations, LeoLatLong(20, 4));
  ground.Add(groundStations, LeoLatLong(19, 12));
  ground.Add(groundStations, LeoLatLong(19, 10));
  ground.Add(groundStations, LeoLatLong(19, 19));
  ground.Add(groundStations, LeoLatLong(19, 20));
  ground.Add(groundStations, LeoLatLong(18, 20));
  ground.Add(groundStations, LeoLatLong(18, 22));
  ground.Add(groundStations, LeoLatLong(17, 26));
  ground.Add(groundStations, LeoLatLong(18, 30));
  ground.Add(groundStations, LeoLatLong(15, 40));
  ground.Add(groundStations, LeoLatLong(14, 25));
  ground.Add(groundStations, LeoLatLong(14, 30));
  ground.Add(groundStations, LeoLatLong(14, 40));
  ground.Add(groundStations, LeoLatLong(14, 50));
  ground.Add(groundStations, LeoLatLong(14, 52));
  ground.Add(groundStations, LeoLatLong(13, 50));
  ground.Add(groundStations, LeoLatLong(13, 48));
  ground.Add(groundStations, LeoLatLong(12, 50));
  ground.Add(groundStations, LeoLatLong(13, 52));
  ground.Add(groundStations, LeoLatLong(15, 30));

  // Set network
  LeoChannelHelper utCh;
  utCh.SetConstellation (constellation);
  utNet = utCh.Install (satellites, groundStations);

  AodvHelper aodv;
  aodv.Set ("EnableHello", BooleanValue (false));
  
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv);
  stack.Install (satellites);
  stack.Install (groundStations);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.0.0", "255.255.0.0");
  utInterfaces = ipv4.Assign(utNet); //把ip存取出來

  // Receiver: satellites
  PacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  for(int i=0; i<60; i++){
    sinkApps.Add(sink.Install(satellites.Get(i)));
  }

  // Task 1: Input File
  
  std::ifstream fin("network.graph");
  int numGs, numSat, numLinks;
  fin >> numGs >> numSat >> numLinks;
  for (int i = 0; i < numLinks; ++i){
    int gsId, satId, rate;
    fin >> gsId >> satId >> rate;
    linkDataRate[{gsId, satId}] = rate;
  }
  fin.close();
  
  std::ifstream fin2(inputFile);
  double totalTime;
  fin2 >> totalTime; //.out裡面第一行是時間 先讀出來
  gsToSat.resize(numGs);
  for (int i = 0; i < numGs; ++i) {
    int gsId, satId;
    fin2 >> gsId >> satId;
    gsToSat[gsId] = satId;
  }
  fin2.close();
  
  // Task 2.2: Call SendPacket()
  gsStarted.assign(numGs, false); //預設一開始GS都還沒發送封包
  gsStartTime.assign(numGs, 0.0); //預設GS傳輸開始時間=0
  gsEndTime.assign(numGs, 0.0); //預設GS傳輸結束時間=0
  satIdle.assign(numSat, true); //預設衛星一開始都是空閒的
  satTotalTime.assign(numSat, 0.0); //預設衛星總時間=0
  satCompletedCount.assign(numSat, 0); //衛星完成接受的次數=0

  for (int gsId = 0; gsId < numGs; ++gsId) {
    int satId = gsToSat[gsId];
    if (!gsStarted[gsId] && satIdle[satId]) { //如果GS還沒發送跟衛星是空閒的就發送封包
      SendPacket(gsId, satId);  
      satIdle[satId] = false;  //把衛星改成非idle
    }
  }

  Simulator::Schedule(Seconds(1e-7), &Connect);
  Simulator::Stop (Seconds (duration));
  Simulator::Run ();
  Simulator::Destroy ();

  // Task 4: Output File
  std::ofstream fout(outputFile);
  double totalCollectionTime = 0.0;
  for(double t : satTotalTime){ //找全部中最大的傳輸時間
    if(t > totalCollectionTime){
      totalCollectionTime = t;
    }
  }
  fout << totalCollectionTime << "\n";

  for(int satId = 0; satId < (int)satTotalTime.size(); ++satId){
    fout << satId << " " << satTotalTime[satId] << "\n";
  }

  for(int gsId = 0; gsId < (int)gsToSat.size(); ++gsId){
    fout << gsId << " " << gsStartTime[gsId] << " " << gsEndTime[gsId] << "\n";
  }
  fout.close();

  return 0;
}

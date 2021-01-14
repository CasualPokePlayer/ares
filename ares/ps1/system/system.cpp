#include <ps1/ps1.hpp>

namespace ares::PlayStation {

auto load(Node::System& node, string name) -> bool {
  return system.load(node, name);
}

System system;
#include "serialization.cpp"

auto System::game() -> string {
  if(disc.cd) {
    return disc.name();
  }

  return "(no disc inserted)";
}

auto System::run() -> void {
  while(!gpu.refreshed) cpu.main();
  gpu.refreshed = false;
}

auto System::load(Node::System& root, string name) -> bool {
  if(node) unload();

  information = {};

  node = Node::System::create(name);
  node->setGame({&System::game, this});
  node->setRun({&System::run, this});
  node->setPower({&System::power, this});
  node->setSave({&System::save, this});
  node->setUnload({&System::unload, this});
  node->setSerialize({&System::serialize, this});
  node->setUnserialize({&System::unserialize, this});
  root = node;

  regionNode = node->append<Node::Setting::String>("Region", "NTSC-J → NTSC-U → PAL");
  regionNode->setAllowedValues({
    "NTSC-J → NTSC-U → PAL",
    "NTSC-U → NTSC-J → PAL",
    "PAL → NTSC-J → NTSC-U",
    "PAL → NTSC-U → NTSC-J",
    "NTSC-J",
    "NTSC-U",
    "PAL"
  });

  fastBoot = node->append<Node::Setting::Boolean>("Fast Boot", false);

  memory.load(node);
  cpu.load(node);
  gpu.load(node);
  spu.load(node);
  mdec.load(node);
  disc.load(node);
  controllerPort1.load(node);
  memoryCardPort1.load(node);
  controllerPort2.load(node);
  memoryCardPort2.load(node);
  interrupt.load(node);
  peripheral.load(node);
  dma.load(node);
  timer.load(node);
  return true;
}

auto System::unload() -> void {
  if(!node) return;
  save();
  memory.unload();
  cpu.unload();
  gpu.unload();
  spu.unload();
  mdec.unload();
  disc.unload();
  controllerPort1.unload();
  memoryCardPort1.unload();
  controllerPort2.unload();
  memoryCardPort2.unload();
  interrupt.unload();
  peripheral.unload();
  dma.unload();
  timer.unload();
  fastBoot.reset();
  regionNode.reset();
  node.reset();
}

auto System::save() -> void {
  if(!node) return;
}

auto System::power(bool reset) -> void {
  for(auto& setting : node->find<Node::Setting::Setting>()) setting->setLatch();

  auto setRegion = [&](string region) {
    if(region == "NTSC-J") {
      information.region = Region::NTSCJ;
    }
    if(region == "NTSC-U") {
      information.region = Region::NTSCU;
    }
    if(region == "PAL") {
      information.region = Region::PAL;
    }
  };
  auto regionsHave = regionNode->latch().split("→").strip();
  setRegion(regionsHave.first());
  for(auto& have : reverse(regionsHave)) {
    if(have == disc.region()) setRegion(have);
  }

  bios.allocate(512_KiB);
  bios.setWaitStates(6, 12, 24);
  if(auto fp = platform->open(node, "bios.rom", File::Read, File::Required)) {
    bios.load(fp);
  }

  //zero-initialization
  if(!reset) {
    serializer s;
    s.setReading();
    serialize(s, true);
  }

  memory.power(reset);
  cpu.power(reset);
  gpu.power(reset);
  spu.power(reset);
  mdec.power(reset);
  disc.power(reset);
  interrupt.power(reset);
  peripheral.power(reset);
  dma.power(reset);
  timer.power(reset);
}

}
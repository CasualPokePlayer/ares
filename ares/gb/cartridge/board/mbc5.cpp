struct MBC5 : Interface {
  using Interface::Interface;
  Memory::Readable<uint8> rom;
  Memory::Writable<uint8> ram;
  Node::Input::Rumble rumble;

  auto load(Markup::Node document) -> void override {
    auto board = document["game/board"];
    Interface::load(rom, board["memory(type=ROM,content=Program)"]);
    Interface::load(ram, board["memory(type=RAM,content=Save)"]);
    rumble = cartridge.node->append<Node::Input::Rumble>("Rumble");
  }

  auto save(Markup::Node document) -> void override {
    auto board = document["game/board"];
    Interface::save(ram, board["memory(type=RAM,content=Save)"]);
  }

  auto unload() -> void override {
    cartridge.node->remove(rumble);
  }

  auto read(uint16 address, uint8 data) -> uint8 override {
    if(address >= 0x0000 && address <= 0x3fff) {
      return rom.read((uint14)address);
    }

    if(address >= 0x4000 && address <= 0x7fff) {
      return rom.read(io.rom.bank << 14 | (uint14)address);
    }

    if(address >= 0xa000 && address <= 0xbfff) {
      if(!ram || !io.ram.enable) return 0xff;
      return ram.read(io.ram.bank << 13 | (uint13)address);
    }

    return data;
  }

  auto write(uint16 address, uint8 data) -> void override {
    if(address >= 0x0000 && address <= 0x1fff) {
      io.ram.enable = data.bit(0,3) == 0x0a;
      return;
    }

    if(address >= 0x2000 && address <= 0x2fff) {
      io.rom.bank.bit(0,7) = data.bit(0,7);
      return;
    }

    if(address >= 0x3000 && address <= 0x3fff) {
      io.rom.bank.bit(8) = data.bit(0);
      return;
    }

    if(address >= 0x4000 && address <= 0x5fff) {
      //todo: add rumble timeout?
      rumble->setEnable(data.bit(3));
      platform->input(rumble);
      io.ram.bank = data.bit(0,3);
      return;
    }

    if(address >= 0xa000 && address <= 0xbfff) {
      if(!ram || !io.ram.enable) return;
      return ram.write(io.ram.bank << 13 | (uint13)address, data);
    }
  }

  auto power() -> void override {
    io = {};
  }

  auto serialize(serializer& s) -> void override {
    s(ram);
    s(io.rom.bank);
    s(io.ram.enable);
    s(io.ram.bank);
  }

  struct IO {
    struct ROM {
      uint9 bank = 0x01;
    } rom;
    struct RAM {
      uint1 enable;
      uint4 bank;
    } ram;
  } io;
};
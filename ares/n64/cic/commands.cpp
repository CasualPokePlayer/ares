
auto CIC::cmdCompare() -> void {   
}

auto CIC::challenge(n4 mem[30]) -> void {
  static n4 lut[32] = {
    0x4, 0x7, 0xa, 0x7, 0xe, 0x5, 0xe, 0x1,
    0xc, 0xf, 0x8, 0xf, 0x6, 0x3, 0x6, 0x9,
    0x4, 0x1, 0xa, 0x7, 0xe, 0x5, 0xe, 0x1,
    0xc, 0x9, 0x8, 0x5, 0x6, 0x3, 0xc, 0x9,
  };

  n4 key = 0xb;
  n1 sel = 0;
  for(u32 address : range(30)) {
    n4 data = key + 5 * mem[address];
    mem[address] = data;
    key = lut[sel << 4 | data];
    n1 mod = data >> 3;
    n3 mag = data >> 0;
    if(mod) mag = ~mag;
    if(mag % 3 != 1) mod = !mod;
    if(sel) {
      if(data == 0x1 || data == 0x9) mod = 1;
      if(data == 0xb || data == 0xe) mod = 0;
    }
    sel = mod;
  }
}

auto CIC::cmdChallenge() -> void {
  if(state == Run) {
    fifo.write(0xa);
    fifo.write(0xa);
    state = Challenge;
  }
  if(state == Challenge && fifo.size() == 30) {
    n4 data[30];
    for (auto i : range(30)) data[i] = fifo.read();
    challenge(data);
    fifo.write(0); // write 0 bit
    for (auto i : range(30)) fifo.write(data[i]);
    state = Run;
  }
}

auto CIC::cmdDie() -> void {
  debug(unusual, "[CIC::cmdDie] die command received by PIF");
  state = Dead;
}

auto CIC::cmdReset() -> void {
  debug(unimplemented, "[CIC::cmdReset]");
}

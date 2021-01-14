#pragma once
//started: 2020-06-17

#include <ares/ares.hpp>
#include <nall/hashset.hpp>
#include <nall/recompiler/amd64/amd64.hpp>

namespace ares::PlayStation {
  auto load(Node::System& node, string name) -> bool;

  enum : bool { Read = 0, Write = 1 };
  enum : uint { Byte = 1, Half = 2, Word = 4 };

  struct Region {
    inline static auto NTSCJ() -> bool;
    inline static auto NTSCU() -> bool;
    inline static auto PAL() -> bool;
  };

  struct Thread {
    auto reset() -> void {
      clock = 0;
    }

    auto serialize(serializer& s) -> void {
      s(clock);
    }

    s64 clock;
  };

  #include <ps1/accuracy.hpp>
  #include <ps1/memory/memory.hpp>
  #include <ps1/system/system.hpp>
  #include <ps1/disc/disc.hpp>
  #include <ps1/cpu/cpu.hpp>
  #include <ps1/gpu/gpu.hpp>
  #include <ps1/spu/spu.hpp>
  #include <ps1/mdec/mdec.hpp>
  #include <ps1/interrupt/interrupt.hpp>
  #include <ps1/peripheral/peripheral.hpp>
  #include <ps1/dma/dma.hpp>
  #include <ps1/timer/timer.hpp>
  #include <ps1/memory/bus.hpp>
}
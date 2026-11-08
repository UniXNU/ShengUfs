// SPDX-License-Identifier: BSD-2-Clause-Patent
// DMA region layout. Offset 0 holds the UTP transfer request list (UTRLBA). Command slot 0 keeps the original layout (its command
// descriptor at kUcdOffset, data at kDataOffset); slots 1..kSlots-1 follow, each with its own descriptor and data area, so commands
// run concurrently. Slot s uses request list entry 2s: the stream is not cache coherent, and with 32-byte entries two neighbours
// share a 64-byte line, so the CPU's clean of one entry could write a stale copy over the controller's completion of the other.
enum : uint32_t {
  kUcdOffset = 4096,
  kRspOffset = 512,
  kPrdtOffset = 1024,
  kDataOffset = 8192,
  kDataMax = 131072,
  kSlots = 8,
  kUcdStride = 2048,
  kExtraUcdOffset = kDataOffset + kDataMax,
  kExtraDataOffset = (kExtraUcdOffset + (kSlots - 1) * kUcdStride + 4095) & ~4095U,
  kDmaBytes = kExtraDataOffset + (kSlots - 1) * kDataMax,
  kDmaBytesOneSlot = kDataOffset + kDataMax,
};
static constexpr uint32_t ufsSlotEntry (unsigned slot) { return 2 * slot; }
static constexpr uint32_t ufsSlotUtrdOffset (unsigned slot) { return 32 * ufsSlotEntry (slot); }
static constexpr uint32_t ufsSlotUcdOffset (unsigned slot) { return slot ? kExtraUcdOffset + (slot - 1) * kUcdStride : kUcdOffset; }
static constexpr uint32_t ufsSlotDataOffset (unsigned slot) { return slot ? kExtraDataOffset + (slot - 1) * kDataMax : kDataOffset; }
static_assert (ufsSlotUcdOffset (kSlots - 1) + kPrdtOffset + 16 <= kExtraDataOffset, "slot descriptors overlap data");
static_assert (ufsSlotUtrdOffset (kSlots - 1) + 32 <= kUcdOffset, "request list overlaps slot 0's descriptor");

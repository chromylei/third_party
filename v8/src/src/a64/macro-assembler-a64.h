// Copyright 2013 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef V8_A64_MACRO_ASSEMBLER_A64_H_
#define V8_A64_MACRO_ASSEMBLER_A64_H_

#include <vector>

#include "v8globals.h"
#include "globals.h"

#include "a64/assembler-a64-inl.h"

namespace v8 {
namespace internal {

#define LS_MACRO_LIST(V)                                      \
  V(Ldrb, Register&, rt, LDRB_w)                              \
  V(Strb, Register&, rt, STRB_w)                              \
  V(Ldrsb, Register&, rt, rt.Is64Bits() ? LDRSB_x : LDRSB_w)  \
  V(Ldrh, Register&, rt, LDRH_w)                              \
  V(Strh, Register&, rt, STRH_w)                              \
  V(Ldrsh, Register&, rt, rt.Is64Bits() ? LDRSH_x : LDRSH_w)  \
  V(Ldr, CPURegister&, rt, LoadOpFor(rt))                     \
  V(Str, CPURegister&, rt, StoreOpFor(rt))                    \
  V(Ldrsw, Register&, rt, LDRSW_x)


// ----------------------------------------------------------------------------
// Static helper functions

// Generate a MemOperand for loading a field from an object.
inline MemOperand FieldMemOperand(Register object, int offset);
inline MemOperand UntagSmiFieldMemOperand(Register object, int offset);

// Generate a MemOperand for loading a SMI from memory.
inline MemOperand UntagSmiMemOperand(Register object, int offset);


// ----------------------------------------------------------------------------
// MacroAssembler

enum BranchType {
  // Copies of architectural conditions.
  // The associated conditions can be used in place of those, the code will
  // take care of reinterpreting them with the correct type.
  integer_eq = eq,
  integer_ne = ne,
  integer_hs = hs,
  integer_lo = lo,
  integer_mi = mi,
  integer_pl = pl,
  integer_vs = vs,
  integer_vc = vc,
  integer_hi = hi,
  integer_ls = ls,
  integer_ge = ge,
  integer_lt = lt,
  integer_gt = gt,
  integer_le = le,
  integer_al = al,
  integer_nv = nv,

  // These two are *different* from the architectural codes al and nv.
  // 'always' is used to generate unconditional branches.
  // 'never' is used to not generate a branch (generally as the inverse
  // branch type of 'always).
  always, never,
  // cbz and cbnz
  reg_zero, reg_not_zero,
  // tbz and tbnz
  reg_bit_clear, reg_bit_set,

  // Aliases.
  kBranchTypeFirstCondition = eq,
  kBranchTypeLastCondition = nv,
  kBranchTypeFirstUsingReg = reg_zero,
  kBranchTypeFirstUsingBit = reg_bit_clear
};

inline BranchType InvertBranchType(BranchType type) {
  if (kBranchTypeFirstCondition <= type && type <= kBranchTypeLastCondition) {
    return static_cast<BranchType>(
        InvertCondition(static_cast<Condition>(type)));
  } else {
    return static_cast<BranchType>(type ^ 1);
  }
}

enum RememberedSetAction { EMIT_REMEMBERED_SET, OMIT_REMEMBERED_SET };
enum SmiCheck { INLINE_SMI_CHECK, OMIT_SMI_CHECK };
enum LinkRegisterStatus { kLRHasNotBeenSaved, kLRHasBeenSaved };
enum TargetAddressStorageMode {
  CAN_INLINE_TARGET_ADDRESS,
  NEVER_INLINE_TARGET_ADDRESS
};
enum UntagMode { kNotSpeculativeUntag, kSpeculativeUntag };
enum ArrayHasHoles { kArrayCantHaveHoles, kArrayCanHaveHoles };
enum CopyHint { kCopyUnknown, kCopyShort, kCopyLong };
enum DiscardMoveMode { kDontDiscardForSameWReg, kDiscardForSameWReg };
enum SeqStringSetCharCheckIndexType { kIndexIsSmi, kIndexIsInteger32 };

class MacroAssembler : public Assembler {
 public:
  MacroAssembler(Isolate* isolate, byte * buffer, unsigned buffer_size);

  inline Handle<Object> CodeObject();

  // Instruction set functions ------------------------------------------------
  // Logical macros.
  inline void And(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Ands(const Register& rd,
                   const Register& rn,
                   const Operand& operand);
  inline void Bic(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Bics(const Register& rd,
                   const Register& rn,
                   const Operand& operand);
  inline void Orr(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Orn(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Eor(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Eon(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Tst(const Register& rn, const Operand& operand);
  void LogicalMacro(const Register& rd,
                    const Register& rn,
                    const Operand& operand,
                    LogicalOp op);

  // Add and sub macros.
  inline void Add(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Adds(const Register& rd,
                   const Register& rn,
                   const Operand& operand);
  inline void Sub(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Subs(const Register& rd,
                   const Register& rn,
                   const Operand& operand);
  inline void Cmn(const Register& rn, const Operand& operand);
  inline void Cmp(const Register& rn, const Operand& operand);
  inline void Neg(const Register& rd,
                  const Operand& operand);
  inline void Negs(const Register& rd,
                   const Operand& operand);

  void AddSubMacro(const Register& rd,
                   const Register& rn,
                   const Operand& operand,
                   FlagsUpdate S,
                   AddSubOp op);

  // Add/sub with carry macros.
  inline void Adc(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Adcs(const Register& rd,
                   const Register& rn,
                   const Operand& operand);
  inline void Sbc(const Register& rd,
                  const Register& rn,
                  const Operand& operand);
  inline void Sbcs(const Register& rd,
                   const Register& rn,
                   const Operand& operand);
  inline void Ngc(const Register& rd,
                  const Operand& operand);
  inline void Ngcs(const Register& rd,
                   const Operand& operand);
  void AddSubWithCarryMacro(const Register& rd,
                            const Register& rn,
                            const Operand& operand,
                            FlagsUpdate S,
                            AddSubWithCarryOp op);

  // Move macros.
  void Mov(const Register& rd,
           const Operand& operand,
           DiscardMoveMode discard_mode = kDontDiscardForSameWReg);
  void Mov(const Register& rd, uint64_t imm);
  inline void Mvn(const Register& rd, uint64_t imm);
  void Mvn(const Register& rd, const Operand& operand);
  static bool IsImmMovn(uint64_t imm, unsigned reg_size);
  static bool IsImmMovz(uint64_t imm, unsigned reg_size);
  static unsigned CountClearHalfWords(uint64_t imm, unsigned reg_size);

  // Conditional macros.
  inline void Ccmp(const Register& rn,
                   const Operand& operand,
                   StatusFlags nzcv,
                   Condition cond);
  inline void Ccmn(const Register& rn,
                   const Operand& operand,
                   StatusFlags nzcv,
                   Condition cond);
  void ConditionalCompareMacro(const Register& rn,
                               const Operand& operand,
                               StatusFlags nzcv,
                               Condition cond,
                               ConditionalCompareOp op);
  void Csel(const Register& rd,
            const Register& rn,
            const Operand& operand,
            Condition cond);

  // Load/store macros.
#define DECLARE_FUNCTION(FN, REGTYPE, REG, OP) \
  inline void FN(const REGTYPE REG, const MemOperand& addr);
  LS_MACRO_LIST(DECLARE_FUNCTION)
#undef DECLARE_FUNCTION

  void LoadStoreMacro(const CPURegister& rt,
                      const MemOperand& addr,
                      LoadStoreOp op);

  // V8-specific load/store helpers.
  void Load(const Register& rt, const MemOperand& addr, Representation r);
  void Store(const Register& rt, const MemOperand& addr, Representation r);

  // Remaining instructions are simple pass-through calls to the assembler.
  inline void Adr(const Register& rd, Label* label);
  inline void Asr(const Register& rd, const Register& rn, unsigned shift);
  inline void Asr(const Register& rd, const Register& rn, const Register& rm);

  // Branch type inversion relies on these relations.
  STATIC_ASSERT((reg_zero      == (reg_not_zero ^ 1)) &&
                (reg_bit_clear == (reg_bit_set ^ 1)) &&
                (always        == (never ^ 1)));

  void B(Label* label, BranchType type, Register reg = NoReg, int bit = -1);

  inline void B(Label* label);
  inline void B(Condition cond, Label* label);
  void B(Label* label, Condition cond);
  inline void Bfi(const Register& rd,
                  const Register& rn,
                  unsigned lsb,
                  unsigned width);
  inline void Bfxil(const Register& rd,
                    const Register& rn,
                    unsigned lsb,
                    unsigned width);
  inline void Bind(Label* label);
  inline void Bl(Label* label);
  inline void Blr(const Register& xn);
  inline void Br(const Register& xn);
  inline void Brk(int code);
  void Cbnz(const Register& rt, Label* label);
  void Cbz(const Register& rt, Label* label);
  inline void Cinc(const Register& rd, const Register& rn, Condition cond);
  inline void Cinv(const Register& rd, const Register& rn, Condition cond);
  inline void Cls(const Register& rd, const Register& rn);
  inline void Clz(const Register& rd, const Register& rn);
  inline void Cneg(const Register& rd, const Register& rn, Condition cond);
  inline void CzeroX(const Register& rd, Condition cond);
  inline void CmovX(const Register& rd, const Register& rn, Condition cond);
  inline void Cset(const Register& rd, Condition cond);
  inline void Csetm(const Register& rd, Condition cond);
  inline void Csinc(const Register& rd,
                    const Register& rn,
                    const Register& rm,
                    Condition cond);
  inline void Csinv(const Register& rd,
                    const Register& rn,
                    const Register& rm,
                    Condition cond);
  inline void Csneg(const Register& rd,
                    const Register& rn,
                    const Register& rm,
                    Condition cond);
  inline void Dmb(BarrierDomain domain, BarrierType type);
  inline void Dsb(BarrierDomain domain, BarrierType type);
  inline void Debug(const char* message, uint32_t code, Instr params = BREAK);
  inline void Extr(const Register& rd,
                   const Register& rn,
                   const Register& rm,
                   unsigned lsb);
  inline void Fabs(const FPRegister& fd, const FPRegister& fn);
  inline void Fadd(const FPRegister& fd,
                   const FPRegister& fn,
                   const FPRegister& fm);
  inline void Fccmp(const FPRegister& fn,
                    const FPRegister& fm,
                    StatusFlags nzcv,
                    Condition cond);
  inline void Fcmp(const FPRegister& fn, const FPRegister& fm);
  inline void Fcmp(const FPRegister& fn, double value);
  inline void Fcsel(const FPRegister& fd,
                    const FPRegister& fn,
                    const FPRegister& fm,
                    Condition cond);
  inline void Fcvt(const FPRegister& fd, const FPRegister& fn);
  inline void Fcvtas(const Register& rd, const FPRegister& fn);
  inline void Fcvtau(const Register& rd, const FPRegister& fn);
  inline void Fcvtms(const Register& rd, const FPRegister& fn);
  inline void Fcvtmu(const Register& rd, const FPRegister& fn);
  inline void Fcvtns(const Register& rd, const FPRegister& fn);
  inline void Fcvtnu(const Register& rd, const FPRegister& fn);
  inline void Fcvtzs(const Register& rd, const FPRegister& fn);
  inline void Fcvtzu(const Register& rd, const FPRegister& fn);
  inline void Fdiv(const FPRegister& fd,
                   const FPRegister& fn,
                   const FPRegister& fm);
  inline void Fmadd(const FPRegister& fd,
                    const FPRegister& fn,
                    const FPRegister& fm,
                    const FPRegister& fa);
  inline void Fmax(const FPRegister& fd,
                   const FPRegister& fn,
                   const FPRegister& fm);
  inline void Fmaxnm(const FPRegister& fd,
                     const FPRegister& fn,
                     const FPRegister& fm);
  inline void Fmin(const FPRegister& fd,
                   const FPRegister& fn,
                   const FPRegister& fm);
  inline void Fminnm(const FPRegister& fd,
                     const FPRegister& fn,
                     const FPRegister& fm);
  inline void Fmov(FPRegister fd, FPRegister fn);
  inline void Fmov(FPRegister fd, Register rn);
  inline void Fmov(FPRegister fd, double imm);
  inline void Fmov(Register rd, FPRegister fn);
  inline void Fmsub(const FPRegister& fd,
                    const FPRegister& fn,
                    const FPRegister& fm,
                    const FPRegister& fa);
  inline void Fmul(const FPRegister& fd,
                   const FPRegister& fn,
                   const FPRegister& fm);
  inline void Fneg(const FPRegister& fd, const FPRegister& fn);
  inline void Fnmadd(const FPRegister& fd,
                     const FPRegister& fn,
                     const FPRegister& fm,
                     const FPRegister& fa);
  inline void Fnmsub(const FPRegister& fd,
                     const FPRegister& fn,
                     const FPRegister& fm,
                     const FPRegister& fa);
  inline void Frinta(const FPRegister& fd, const FPRegister& fn);
  inline void Frintn(const FPRegister& fd, const FPRegister& fn);
  inline void Frintz(const FPRegister& fd, const FPRegister& fn);
  inline void Fsqrt(const FPRegister& fd, const FPRegister& fn);
  inline void Fsub(const FPRegister& fd,
                   const FPRegister& fn,
                   const FPRegister& fm);
  inline void Hint(SystemHint code);
  inline void Hlt(int code);
  inline void Isb();
  inline void Ldnp(const CPURegister& rt,
                   const CPURegister& rt2,
                   const MemOperand& src);
  inline void Ldp(const CPURegister& rt,
                  const CPURegister& rt2,
                  const MemOperand& src);
  inline void Ldpsw(const Register& rt,
                    const Register& rt2,
                    const MemOperand& src);
  inline void Ldr(const FPRegister& ft, double imm);
  inline void Ldr(const Register& rt, uint64_t imm);
  inline void Lsl(const Register& rd, const Register& rn, unsigned shift);
  inline void Lsl(const Register& rd, const Register& rn, const Register& rm);
  inline void Lsr(const Register& rd, const Register& rn, unsigned shift);
  inline void Lsr(const Register& rd, const Register& rn, const Register& rm);
  inline void Madd(const Register& rd,
                   const Register& rn,
                   const Register& rm,
                   const Register& ra);
  inline void Mneg(const Register& rd, const Register& rn, const Register& rm);
  inline void Mov(const Register& rd, const Register& rm);
  inline void Movk(const Register& rd, uint64_t imm, int shift = -1);
  inline void Mrs(const Register& rt, SystemRegister sysreg);
  inline void Msr(SystemRegister sysreg, const Register& rt);
  inline void Msub(const Register& rd,
                   const Register& rn,
                   const Register& rm,
                   const Register& ra);
  inline void Mul(const Register& rd, const Register& rn, const Register& rm);
  inline void Nop() { nop(); }
  inline void Rbit(const Register& rd, const Register& rn);
  inline void Ret(const Register& xn = lr);
  inline void Rev(const Register& rd, const Register& rn);
  inline void Rev16(const Register& rd, const Register& rn);
  inline void Rev32(const Register& rd, const Register& rn);
  inline void Ror(const Register& rd, const Register& rs, unsigned shift);
  inline void Ror(const Register& rd, const Register& rn, const Register& rm);
  inline void Sbfiz(const Register& rd,
                    const Register& rn,
                    unsigned lsb,
                    unsigned width);
  inline void Sbfx(const Register& rd,
                   const Register& rn,
                   unsigned lsb,
                   unsigned width);
  inline void Scvtf(const FPRegister& fd,
                    const Register& rn,
                    unsigned fbits = 0);
  inline void Sdiv(const Register& rd, const Register& rn, const Register& rm);
  inline void Smaddl(const Register& rd,
                     const Register& rn,
                     const Register& rm,
                     const Register& ra);
  inline void Smsubl(const Register& rd,
                     const Register& rn,
                     const Register& rm,
                     const Register& ra);
  inline void Smull(const Register& rd,
                    const Register& rn,
                    const Register& rm);
  inline void Smulh(const Register& rd,
                    const Register& rn,
                    const Register& rm);
  inline void Stnp(const CPURegister& rt,
                   const CPURegister& rt2,
                   const MemOperand& dst);
  inline void Stp(const CPURegister& rt,
                  const CPURegister& rt2,
                  const MemOperand& dst);
  inline void Sxtb(const Register& rd, const Register& rn);
  inline void Sxth(const Register& rd, const Register& rn);
  inline void Sxtw(const Register& rd, const Register& rn);
  void Tbnz(const Register& rt, unsigned bit_pos, Label* label);
  void Tbz(const Register& rt, unsigned bit_pos, Label* label);
  inline void Ubfiz(const Register& rd,
                    const Register& rn,
                    unsigned lsb,
                    unsigned width);
  inline void Ubfx(const Register& rd,
                   const Register& rn,
                   unsigned lsb,
                   unsigned width);
  inline void Ucvtf(const FPRegister& fd,
                    const Register& rn,
                    unsigned fbits = 0);
  inline void Udiv(const Register& rd, const Register& rn, const Register& rm);
  inline void Umaddl(const Register& rd,
                     const Register& rn,
                     const Register& rm,
                     const Register& ra);
  inline void Umsubl(const Register& rd,
                     const Register& rn,
                     const Register& rm,
                     const Register& ra);
  inline void Uxtb(const Register& rd, const Register& rn);
  inline void Uxth(const Register& rd, const Register& rn);
  inline void Uxtw(const Register& rd, const Register& rn);

  // Pseudo-instructions ------------------------------------------------------

  // Compute rd = abs(rm).
  // This function clobbers the condition flags.
  //
  // If rm is the minimum representable value, the result is not representable.
  // Handlers for each case can be specified using the relevant labels.
  void Abs(const Register& rd, const Register& rm,
           Label * is_not_representable = NULL,
           Label * is_representable = NULL);

  // Push or pop up to 4 registers of the same width to or from the stack,
  // using the current stack pointer as set by SetStackPointer.
  //
  // If an argument register is 'NoReg', all further arguments are also assumed
  // to be 'NoReg', and are thus not pushed or popped.
  //
  // Arguments are ordered such that "Push(a, b);" is functionally equivalent
  // to "Push(a); Push(b);".
  //
  // It is valid to push the same register more than once, and there is no
  // restriction on the order in which registers are specified.
  //
  // It is not valid to pop into the same register more than once in one
  // operation, not even into the zero register.
  //
  // If the current stack pointer (as set by SetStackPointer) is csp, then it
  // must be aligned to 16 bytes on entry and the total size of the specified
  // registers must also be a multiple of 16 bytes.
  //
  // Even if the current stack pointer is not the system stack pointer (csp),
  // Push (and derived methods) will still modify the system stack pointer in
  // order to comply with ABI rules about accessing memory below the system
  // stack pointer.
  //
  // Other than the registers passed into Pop, the stack pointer and (possibly)
  // the system stack pointer, these methods do not modify any other registers.
  // Scratch registers such as Tmp0() and Tmp1() are preserved.
  void Push(const CPURegister& src0, const CPURegister& src1 = NoReg,
            const CPURegister& src2 = NoReg, const CPURegister& src3 = NoReg);
  void Pop(const CPURegister& dst0, const CPURegister& dst1 = NoReg,
           const CPURegister& dst2 = NoReg, const CPURegister& dst3 = NoReg);

  // Alternative forms of Push and Pop, taking a RegList or CPURegList that
  // specifies the registers that are to be pushed or popped. Higher-numbered
  // registers are associated with higher memory addresses (as in the A32 push
  // and pop instructions).
  //
  // (Push|Pop)SizeRegList allow you to specify the register size as a
  // parameter. Only kXRegSize, kWRegSize, kDRegSize and kSRegSize are
  // supported.
  //
  // Otherwise, (Push|Pop)(CPU|X|W|D|S)RegList is preferred.
  void PushCPURegList(CPURegList registers);
  void PopCPURegList(CPURegList registers);

  inline void PushSizeRegList(RegList registers, unsigned reg_size,
      CPURegister::RegisterType type = CPURegister::kRegister) {
    PushCPURegList(CPURegList(type, reg_size, registers));
  }
  inline void PopSizeRegList(RegList registers, unsigned reg_size,
      CPURegister::RegisterType type = CPURegister::kRegister) {
    PopCPURegList(CPURegList(type, reg_size, registers));
  }
  inline void PushXRegList(RegList regs) {
    PushSizeRegList(regs, kXRegSize);
  }
  inline void PopXRegList(RegList regs) {
    PopSizeRegList(regs, kXRegSize);
  }
  inline void PushWRegList(RegList regs) {
    PushSizeRegList(regs, kWRegSize);
  }
  inline void PopWRegList(RegList regs) {
    PopSizeRegList(regs, kWRegSize);
  }
  inline void PushDRegList(RegList regs) {
    PushSizeRegList(regs, kDRegSize, CPURegister::kFPRegister);
  }
  inline void PopDRegList(RegList regs) {
    PopSizeRegList(regs, kDRegSize, CPURegister::kFPRegister);
  }
  inline void PushSRegList(RegList regs) {
    PushSizeRegList(regs, kSRegSize, CPURegister::kFPRegister);
  }
  inline void PopSRegList(RegList regs) {
    PopSizeRegList(regs, kSRegSize, CPURegister::kFPRegister);
  }

  // Push the specified register 'count' times.
  void PushMultipleTimes(CPURegister src, Register count);
  void PushMultipleTimes(CPURegister src, int count);

  // This is a convenience method for pushing a single Handle<Object>.
  inline void Push(Handle<Object> handle);
  void Push(Smi* smi) { Push(Handle<Smi>(smi, isolate())); }

  // Aliases of Push and Pop, required for V8 compatibility.
  inline void push(Register src) {
    Push(src);
  }
  inline void pop(Register dst) {
    Pop(dst);
  }

  // Sometimes callers need to push or pop multiple registers in a way that is
  // difficult to structure efficiently for fixed Push or Pop calls. This scope
  // allows push requests to be queued up, then flushed at once. The
  // MacroAssembler will try to generate the most efficient sequence required.
  //
  // Unlike the other Push and Pop macros, PushPopQueue can handle mixed sets of
  // register sizes and types.
  class PushPopQueue {
   public:
    explicit PushPopQueue(MacroAssembler* masm) : masm_(masm), size_(0) { }

    ~PushPopQueue() {
      ASSERT(queued_.empty());
    }

    void Queue(const CPURegister& rt) {
      size_ += rt.SizeInBytes();
      queued_.push_back(rt);
    }

    void PushQueued();
    void PopQueued();

   private:
    MacroAssembler* masm_;
    int size_;
    std::vector<CPURegister> queued_;
  };

  // Poke 'src' onto the stack. The offset is in bytes.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then
  // csp must be aligned to 16 bytes.
  void Poke(const CPURegister& src, const Operand& offset);

  // Peek at a value on the stack, and put it in 'dst'. The offset is in bytes.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then
  // csp must be aligned to 16 bytes.
  void Peek(const CPURegister& dst, const Operand& offset);

  // Poke 'src1' and 'src2' onto the stack. The values written will be adjacent
  // with 'src2' at a higher address than 'src1'. The offset is in bytes.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then
  // csp must be aligned to 16 bytes.
  void PokePair(const CPURegister& src1, const CPURegister& src2, int offset);

  // Peek at two values on the stack, and put them in 'dst1' and 'dst2'. The
  // values peeked will be adjacent, with the value in 'dst2' being from a
  // higher address than 'dst1'. The offset is in bytes.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then
  // csp must be aligned to 16 bytes.
  void PeekPair(const CPURegister& dst1, const CPURegister& dst2, int offset);

  // Claim or drop stack space without actually accessing memory.
  //
  // In debug mode, both of these will write invalid data into the claimed or
  // dropped space.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then it
  // must be aligned to 16 bytes and the size claimed or dropped must be a
  // multiple of 16 bytes.
  //
  // Note that unit_size must be specified in bytes. For variants which take a
  // Register count, the unit size must be a power of two.
  inline void Claim(uint64_t count, uint64_t unit_size = kXRegSizeInBytes);
  inline void Claim(const Register& count,
                    uint64_t unit_size = kXRegSizeInBytes);
  inline void Drop(uint64_t count, uint64_t unit_size = kXRegSizeInBytes);
  inline void Drop(const Register& count,
                   uint64_t unit_size = kXRegSizeInBytes);

  // Variants of Claim and Drop, where the 'count' parameter is a SMI held in a
  // register.
  inline void ClaimBySMI(const Register& count_smi,
                         uint64_t unit_size = kXRegSizeInBytes);
  inline void DropBySMI(const Register& count_smi,
                        uint64_t unit_size = kXRegSizeInBytes);

  // Compare a register with an operand, and branch to label depending on the
  // condition. May corrupt the status flags.
  inline void CompareAndBranch(const Register& lhs,
                               const Operand& rhs,
                               Condition cond,
                               Label* label);

  // Test the bits of register defined by bit_pattern, and branch if ANY of
  // those bits are set. May corrupt the status flags.
  inline void TestAndBranchIfAnySet(const Register& reg,
                                    const uint64_t bit_pattern,
                                    Label* label);

  // Test the bits of register defined by bit_pattern, and branch if ALL of
  // those bits are clear (ie. not set.) May corrupt the status flags.
  inline void TestAndBranchIfAllClear(const Register& reg,
                                      const uint64_t bit_pattern,
                                      Label* label);

  // Insert one or more instructions into the instruction stream that encode
  // some caller-defined data. The instructions used will be executable with no
  // side effects.
  inline void InlineData(uint64_t data);

  // Insert an instrumentation enable marker into the instruction stream.
  inline void EnableInstrumentation();

  // Insert an instrumentation disable marker into the instruction stream.
  inline void DisableInstrumentation();

  // Insert an instrumentation event marker into the instruction stream. These
  // will be picked up by the instrumentation system to annotate an instruction
  // profile. The argument marker_name must be a printable two character string;
  // it will be encoded in the event marker.
  inline void AnnotateInstrumentation(const char* marker_name);

  // If emit_debug_code() is true, emit a run-time check to ensure that
  // StackPointer() does not point below the system stack pointer.
  //
  // Whilst it is architecturally legal for StackPointer() to point below csp,
  // it can be evidence of a potential bug because the ABI forbids accesses
  // below csp.
  //
  // If emit_debug_code() is false, this emits no code.
  //
  // If StackPointer() is the system stack pointer, this emits no code.
  void AssertStackConsistency();

  // Preserve the callee-saved registers (as defined by AAPCS64).
  //
  // Higher-numbered registers are pushed before lower-numbered registers, and
  // thus get higher addresses.
  // Floating-point registers are pushed before general-purpose registers, and
  // thus get higher addresses.
  //
  // Note that registers are not checked for invalid values. Use this method
  // only if you know that the GC won't try to examine the values on the stack.
  //
  // This method must not be called unless the current stack pointer (as set by
  // SetStackPointer) is the system stack pointer (csp), and is aligned to
  // ActivationFrameAlignment().
  void PushCalleeSavedRegisters();

  // Restore the callee-saved registers (as defined by AAPCS64).
  //
  // Higher-numbered registers are popped after lower-numbered registers, and
  // thus come from higher addresses.
  // Floating-point registers are popped after general-purpose registers, and
  // thus come from higher addresses.
  //
  // This method must not be called unless the current stack pointer (as set by
  // SetStackPointer) is the system stack pointer (csp), and is aligned to
  // ActivationFrameAlignment().
  void PopCalleeSavedRegisters();

  // Set the current stack pointer, but don't generate any code.
  inline void SetStackPointer(const Register& stack_pointer) {
    ASSERT(!AreAliased(stack_pointer, Tmp0(), Tmp1()));
    sp_ = stack_pointer;
  }

  // Return the current stack pointer, as set by SetStackPointer.
  inline const Register& StackPointer() const {
    return sp_;
  }

  // Align csp for a frame, as per ActivationFrameAlignment, and make it the
  // current stack pointer.
  inline void AlignAndSetCSPForFrame() {
    int sp_alignment = ActivationFrameAlignment();
    // AAPCS64 mandates at least 16-byte alignment.
    ASSERT(sp_alignment >= 16);
    ASSERT(IsPowerOf2(sp_alignment));
    Bic(csp, StackPointer(), sp_alignment - 1);
    SetStackPointer(csp);
  }

  // Push the system stack pointer (csp) down to allow the same to be done to
  // the current stack pointer (according to StackPointer()). This must be
  // called _before_ accessing the memory.
  //
  // This is necessary when pushing or otherwise adding things to the stack, to
  // satisfy the AAPCS64 constraint that the memory below the system stack
  // pointer is not accessed.
  //
  // This method asserts that StackPointer() is not csp, since the call does
  // not make sense in that context.
  //
  // TODO(jbramley): Currently, this method can only accept values of 'space'
  // that can be encoded in one instruction. Refer to the implementation for
  // details.
  inline void BumpSystemStackPointer(const Operand& space);

  // Helpers ------------------------------------------------------------------
  // Root register.
  inline void InitializeRootRegister();

  // Load an object from the root table.
  void LoadRoot(Register destination,
                Heap::RootListIndex index);
  // Store an object to the root table.
  void StoreRoot(Register source,
                 Heap::RootListIndex index);

  // Load both TrueValue and FalseValue roots.
  void LoadTrueFalseRoots(Register true_root, Register false_root);

  void LoadHeapObject(Register dst, Handle<HeapObject> object);

  void LoadObject(Register result, Handle<Object> object) {
    AllowDeferredHandleDereference heap_object_check;
    if (object->IsHeapObject()) {
      LoadHeapObject(result, Handle<HeapObject>::cast(object));
    } else {
      ASSERT(object->IsSmi());
      Mov(result, Operand(object));
    }
  }

  static int SafepointRegisterStackIndex(int reg_code);

  // This is required for compatibility with architecture independant code.
  // Remove if not needed.
  inline void Move(Register dst, Register src) { Mov(dst, src); }

  void LoadInstanceDescriptors(Register map,
                               Register descriptors);
  void EnumLengthUntagged(Register dst, Register map);
  void EnumLengthSmi(Register dst, Register map);
  void NumberOfOwnDescriptors(Register dst, Register map);

  template<typename Field>
  void DecodeField(Register reg) {
    static const uint64_t shift = Field::kShift + kSmiShift;
    static const uint64_t setbits = CountSetBits(Field::kMask, 32);
    Ubfx(reg, reg, shift, setbits);
  }

  // ---- SMI and Number Utilities ----

  inline void SmiTag(Register dst, Register src);
  inline void SmiTag(Register smi);
  inline void SmiUntag(Register dst, Register src);
  inline void SmiUntag(Register smi);
  inline void SmiUntagToDouble(FPRegister dst,
                               Register src,
                               UntagMode mode = kNotSpeculativeUntag);
  inline void SmiUntagToFloat(FPRegister dst,
                              Register src,
                              UntagMode mode = kNotSpeculativeUntag);

  // Compute the absolute value of 'smi' and leave the result in 'smi'
  // register. If 'smi' is the most negative SMI, the absolute value cannot
  // be represented as a SMI and a jump to 'slow' is done.
  void SmiAbs(const Register& smi, Label* slow);

  inline void JumpIfSmi(Register value,
                        Label* smi_label,
                        Label* not_smi_label = NULL);
  inline void JumpIfNotSmi(Register value, Label* not_smi_label);
  inline void JumpIfBothSmi(Register value1,
                            Register value2,
                            Label* both_smi_label,
                            Label* not_smi_label = NULL);
  inline void JumpIfEitherSmi(Register value1,
                              Register value2,
                              Label* either_smi_label,
                              Label* not_smi_label = NULL);
  inline void JumpIfEitherNotSmi(Register value1,
                                 Register value2,
                                 Label* not_smi_label);
  inline void JumpIfBothNotSmi(Register value1,
                               Register value2,
                               Label* not_smi_label);

  // Abort execution if argument is a smi, enabled via --debug-code.
  void AssertNotSmi(Register object, BailoutReason reason = kOperandIsASmi);
  void AssertSmi(Register object, BailoutReason reason = kOperandIsNotASmi);

  // Abort execution if argument is not a name, enabled via --debug-code.
  void AssertName(Register object);

  // Abort execution if argument is not a string, enabled via --debug-code.
  void AssertString(Register object);

  void JumpForHeapNumber(Register object,
                         Register heap_number_map,
                         Label* on_heap_number,
                         Label* on_not_heap_number = NULL);
  void JumpIfHeapNumber(Register object,
                        Label* on_heap_number,
                        Register heap_number_map = NoReg);
  void JumpIfNotHeapNumber(Register object,
                           Label* on_not_heap_number,
                           Register heap_number_map = NoReg);

  // Jump to label if the input double register contains -0.0.
  void JumpIfMinusZero(DoubleRegister input, Label* on_negative_zero);

  // Generate code to do a lookup in the number string cache. If the number in
  // the register object is found in the cache the generated code falls through
  // with the result in the result register. The object and the result register
  // can be the same. If the number is not found in the cache the code jumps to
  // the label not_found with only the content of register object unchanged.
  void LookupNumberStringCache(Register object,
                               Register result,
                               Register scratch1,
                               Register scratch2,
                               Register scratch3,
                               Label* not_found);

  // Saturate a signed 32-bit integer in input to an unsigned 8-bit integer in
  // output.
  void ClampInt32ToUint8(Register in_out);
  void ClampInt32ToUint8(Register output, Register input);

  // Saturate a double in input to an unsigned 8-bit integer in output.
  void ClampDoubleToUint8(Register output,
                          DoubleRegister input,
                          DoubleRegister dbl_scratch);

  // Try to convert a double to a signed 32-bit int.
  // This succeeds if the result compares equal to the input, so inputs of -0.0
  // are converted to 0 and handled as a success.
  void TryConvertDoubleToInt32(Register as_int,
                               FPRegister value,
                               FPRegister scratch_d,
                               Label* on_successful_conversion,
                               Label* on_failed_conversion = NULL) {
    ASSERT(as_int.Is32Bits());
    TryConvertDoubleToInt(as_int, value, scratch_d, on_successful_conversion,
                          on_failed_conversion);
  }

  // Try to convert a double to a signed 64-bit int.
  // This succeeds if the result compares equal to the input, so inputs of -0.0
  // are converted to 0 and handled as a success.
  void TryConvertDoubleToInt64(Register as_int,
                               FPRegister value,
                               FPRegister scratch_d,
                               Label* on_successful_conversion,
                               Label* on_failed_conversion = NULL) {
    ASSERT(as_int.Is64Bits());
    TryConvertDoubleToInt(as_int, value, scratch_d, on_successful_conversion,
                          on_failed_conversion);
  }

  // ---- Object Utilities ----

  // Copy fields from 'src' to 'dst', where both are tagged objects.
  // The 'temps' list is a list of X registers which can be used for scratch
  // values. The temps list must include at least one register, and it must not
  // contain Tmp0() or Tmp1().
  //
  // Currently, CopyFields cannot make use of more than three registers from
  // the 'temps' list.
  //
  // As with several MacroAssembler methods, Tmp0() and Tmp1() will be used.
  void CopyFields(Register dst, Register src, CPURegList temps, unsigned count);

  // Copies a number of bytes from src to dst. All passed registers are
  // clobbered. On exit src and dst will point to the place just after where the
  // last byte was read or written and length will be zero. Hint may be used to
  // determine which is the most efficient algorithm to use for copying.
  void CopyBytes(Register dst,
                 Register src,
                 Register length,
                 Register scratch,
                 CopyHint hint = kCopyUnknown);

  // Initialize fields with filler values. Fields starting at start_offset not
  // including end_offset are overwritten with the value in filler. At the end
  // of the loop, start_offset takes the value of end_offset.
  void InitializeFieldsWithFiller(Register start_offset,
                                  Register end_offset,
                                  Register filler);

  // ---- String Utilities ----


  // Jump to label if either object is not a sequential ASCII string.
  // Optionally perform a smi check on the objects first.
  void JumpIfEitherIsNotSequentialAsciiStrings(
      Register first,
      Register second,
      Register scratch1,
      Register scratch2,
      Label* failure,
      SmiCheckType smi_check = DO_SMI_CHECK);

  // Check if instance type is sequential ASCII string and jump to label if
  // it is not.
  void JumpIfInstanceTypeIsNotSequentialAscii(Register type,
                                              Register scratch,
                                              Label* failure);

  // Checks if both instance types are sequential ASCII strings and jumps to
  // label if either is not.
  void JumpIfEitherInstanceTypeIsNotSequentialAscii(
      Register first_object_instance_type,
      Register second_object_instance_type,
      Register scratch1,
      Register scratch2,
      Label* failure);

  // Checks if both instance types are sequential ASCII strings and jumps to
  // label if either is not.
  void JumpIfBothInstanceTypesAreNotSequentialAscii(
      Register first_object_instance_type,
      Register second_object_instance_type,
      Register scratch1,
      Register scratch2,
      Label* failure);

  void JumpIfNotUniqueName(Register type, Label* not_unique_name);

  // ---- Calling / Jumping helpers ----

  // This is required for compatibility in architecture indepenedant code.
  inline void jmp(Label* L) { B(L); }

  // Passes thrown value to the handler of top of the try handler chain.
  // Register value must be x0.
  void Throw(Register value,
             Register scratch1,
             Register scratch2,
             Register scratch3,
             Register scratch4);

  // Propagates an uncatchable exception to the top of the current JS stack's
  // handler chain. Register value must be x0.
  void ThrowUncatchable(Register value,
                        Register scratch1,
                        Register scratch2,
                        Register scratch3,
                        Register scratch4);

  // Throw a message string as an exception.
  void Throw(BailoutReason reason);

  // Throw a message string as an exception if a condition is not true.
  void ThrowIf(Condition cc, BailoutReason reason);

  // Throw a message string as an exception if the value is a smi.
  void ThrowIfSmi(const Register& value, BailoutReason reason);

  void CallStub(CodeStub* stub, TypeFeedbackId ast_id = TypeFeedbackId::None());
  void TailCallStub(CodeStub* stub);

  void CallRuntime(const Runtime::Function* f,
                   int num_arguments,
                   SaveFPRegsMode save_doubles = kDontSaveFPRegs);

  void CallRuntime(Runtime::FunctionId id,
                   int num_arguments,
                   SaveFPRegsMode save_doubles = kDontSaveFPRegs) {
    CallRuntime(Runtime::FunctionForId(id), num_arguments, save_doubles);
  }

  // TODO(all): Why does this variant save FP regs unconditionally?
  void CallRuntimeSaveDoubles(Runtime::FunctionId id) {
    const Runtime::Function* function = Runtime::FunctionForId(id);
    CallRuntime(function, function->nargs, kSaveFPRegs);
  }

  void TailCallRuntime(Runtime::FunctionId fid,
                       int num_arguments,
                       int result_size);

  int ActivationFrameAlignment();

  // Calls a C function.
  // The called function is not allowed to trigger a
  // garbage collection, since that might move the code and invalidate the
  // return address (unless this is somehow accounted for by the called
  // function).
  void CallCFunction(ExternalReference function,
                     int num_reg_arguments);
  void CallCFunction(ExternalReference function,
                     int num_reg_arguments,
                     int num_double_arguments);
  void CallCFunction(Register function,
                     int num_reg_arguments,
                     int num_double_arguments);

  // Calls an API function. Allocates HandleScope, extracts returned value
  // from handle and propagates exceptions.
  // 'stack_space' is the space to be unwound on exit (includes the call JS
  // arguments space and the additional space allocated for the fast call).
  // 'spill_offset' is the offset from the stack pointer where
  // CallApiFunctionAndReturn can spill registers.
  void CallApiFunctionAndReturn(Register function_address,
                                ExternalReference thunk_ref,
                                int stack_space,
                                int spill_offset,
                                MemOperand return_value_operand,
                                MemOperand* context_restore_operand);

  // The number of register that CallApiFunctionAndReturn will need to save on
  // the stack. The space for these registers need to be allocated in the
  // ExitFrame before calling CallApiFunctionAndReturn.
  static const int kCallApiFunctionSpillSpace = 4;

  // Jump to a runtime routine.
  void JumpToExternalReference(const ExternalReference& builtin);
  // Tail call of a runtime routine (jump).
  // Like JumpToExternalReference, but also takes care of passing the number
  // of parameters.
  void TailCallExternalReference(const ExternalReference& ext,
                                 int num_arguments,
                                 int result_size);
  void CallExternalReference(const ExternalReference& ext,
                             int num_arguments);


  // Invoke specified builtin JavaScript function. Adds an entry to
  // the unresolved list if the name does not resolve.
  void InvokeBuiltin(Builtins::JavaScript id,
                     InvokeFlag flag,
                     const CallWrapper& call_wrapper = NullCallWrapper());

  // Store the code object for the given builtin in the target register and
  // setup the function in x1.
  // TODO(all): Can we use another register than x1?
  void GetBuiltinEntry(Register target, Builtins::JavaScript id);

  // Store the function for the given builtin in the target register.
  void GetBuiltinFunction(Register target, Builtins::JavaScript id);

  void Jump(Register target);
  void Jump(Address target, RelocInfo::Mode rmode);
  void Jump(Handle<Code> code, RelocInfo::Mode rmode);
  void Jump(intptr_t target, RelocInfo::Mode rmode);

  void Call(Register target);
  void Call(Label* target);
  void Call(Address target, RelocInfo::Mode rmode);
  void Call(Handle<Code> code,
            RelocInfo::Mode rmode = RelocInfo::CODE_TARGET,
            TypeFeedbackId ast_id = TypeFeedbackId::None());

  // For every Call variant, there is a matching CallSize function that returns
  // the size (in bytes) of the call sequence.
  static int CallSize(Register target);
  static int CallSize(Label* target);
  static int CallSize(Address target, RelocInfo::Mode rmode);
  static int CallSize(Handle<Code> code,
                      RelocInfo::Mode rmode = RelocInfo::CODE_TARGET,
                      TypeFeedbackId ast_id = TypeFeedbackId::None());

  // Registers used through the invocation chain are hard-coded.
  // We force passing the parameters to ensure the contracts are correctly
  // honoured by the caller.
  // 'function' must be x1.
  // 'actual' must use an immediate or x0.
  // 'expected' must use an immediate or x2.
  // 'call_kind' must be x5.
  void InvokePrologue(const ParameterCount& expected,
                      const ParameterCount& actual,
                      Handle<Code> code_constant,
                      Register code_reg,
                      Label* done,
                      InvokeFlag flag,
                      bool* definitely_mismatches,
                      const CallWrapper& call_wrapper);
  void InvokeCode(Register code,
                  const ParameterCount& expected,
                  const ParameterCount& actual,
                  InvokeFlag flag,
                  const CallWrapper& call_wrapper);
  // Invoke the JavaScript function in the given register.
  // Changes the current context to the context in the function before invoking.
  void InvokeFunction(Register function,
                      const ParameterCount& actual,
                      InvokeFlag flag,
                      const CallWrapper& call_wrapper);
  void InvokeFunction(Register function,
                      const ParameterCount& expected,
                      const ParameterCount& actual,
                      InvokeFlag flag,
                      const CallWrapper& call_wrapper);
  void InvokeFunction(Handle<JSFunction> function,
                      const ParameterCount& expected,
                      const ParameterCount& actual,
                      InvokeFlag flag,
                      const CallWrapper& call_wrapper);


  // ---- Floating point helpers ----


  // Performs a truncating conversion of a floating point number as used by
  // the JS bitwise operations. See ECMA-262 9.5: ToInt32. Goes to 'done' if it
  // succeeds, otherwise falls through if result is saturated. On return
  // 'result' either holds answer, or is clobbered on fall through.
  //
  // Only public for the test code in test-code-stubs-a64.cc.
  void TryInlineTruncateDoubleToI(Register result,
                                  DoubleRegister input,
                                  Label* done);

  // Performs a truncating conversion of a floating point number as used by
  // the JS bitwise operations. See ECMA-262 9.5: ToInt32.
  // Exits with 'result' holding the answer.
  void TruncateDoubleToI(Register result, DoubleRegister double_input);

  // Performs a truncating conversion of a heap number as used by
  // the JS bitwise operations. See ECMA-262 9.5: ToInt32. 'result' and 'input'
  // must be different registers.  Exits with 'result' holding the answer.
  void TruncateHeapNumberToI(Register result, Register object);

  // Converts the smi or heap number in object to an int32 using the rules
  // for ToInt32 as described in ECMAScript 9.5.: the value is truncated
  // and brought into the range -2^31 .. +2^31 - 1. 'result' and 'input' must be
  // different registers.
  void TruncateNumberToI(Register object,
                         Register result,
                         Register heap_number_map,
                         Label* not_int32);

  // ---- Code generation helpers ----

  void set_generating_stub(bool value) { generating_stub_ = value; }
  bool generating_stub() const { return generating_stub_; }
#if DEBUG
  void set_allow_macro_instructions(bool value) {
    allow_macro_instructions_ = value;
  }
  bool allow_macro_instructions() const { return allow_macro_instructions_; }
#endif
  bool use_real_aborts() const { return use_real_aborts_; }
  void set_has_frame(bool value) { has_frame_ = value; }
  bool has_frame() const { return has_frame_; }
  bool AllowThisStubCall(CodeStub* stub);

  class NoUseRealAbortsScope {
   public:
    explicit NoUseRealAbortsScope(MacroAssembler* masm) :
        saved_(masm->use_real_aborts_), masm_(masm) {
      masm_->use_real_aborts_ = false;
    }
    ~NoUseRealAbortsScope() {
      masm_->use_real_aborts_ = saved_;
    }
   private:
    bool saved_;
    MacroAssembler* masm_;
  };

#ifdef ENABLE_DEBUGGER_SUPPORT
  // ---------------------------------------------------------------------------
  // Debugger Support

  void DebugBreak();
#endif
  // ---------------------------------------------------------------------------
  // Exception handling

  // Push a new try handler and link into try handler chain.
  void PushTryHandler(StackHandler::Kind kind, int handler_index);

  // Unlink the stack handler on top of the stack from the try handler chain.
  // Must preserve the result register.
  void PopTryHandler();


  // ---------------------------------------------------------------------------
  // Allocation support

  // Allocate an object in new space or old pointer space. The object_size is
  // specified either in bytes or in words if the allocation flag SIZE_IN_WORDS
  // is passed. The allocated object is returned in result.
  //
  // If the new space is exhausted control continues at the gc_required label.
  // In this case, the result and scratch registers may still be clobbered.
  // If flags includes TAG_OBJECT, the result is tagged as as a heap object.
  void Allocate(Register object_size,
                Register result,
                Register scratch1,
                Register scratch2,
                Label* gc_required,
                AllocationFlags flags);

  void Allocate(int object_size,
                Register result,
                Register scratch1,
                Register scratch2,
                Label* gc_required,
                AllocationFlags flags);

  // Undo allocation in new space. The object passed and objects allocated after
  // it will no longer be allocated. The caller must make sure that no pointers
  // are left to the object(s) no longer allocated as they would be invalid when
  // allocation is undone.
  void UndoAllocationInNewSpace(Register object, Register scratch);

  void AllocateTwoByteString(Register result,
                             Register length,
                             Register scratch1,
                             Register scratch2,
                             Register scratch3,
                             Label* gc_required);
  void AllocateAsciiString(Register result,
                           Register length,
                           Register scratch1,
                           Register scratch2,
                           Register scratch3,
                           Label* gc_required);
  void AllocateTwoByteConsString(Register result,
                                 Register length,
                                 Register scratch1,
                                 Register scratch2,
                                 Label* gc_required);
  void AllocateAsciiConsString(Register result,
                               Register length,
                               Register scratch1,
                               Register scratch2,
                               Label* gc_required);
  void AllocateTwoByteSlicedString(Register result,
                                   Register length,
                                   Register scratch1,
                                   Register scratch2,
                                   Label* gc_required);
  void AllocateAsciiSlicedString(Register result,
                                 Register length,
                                 Register scratch1,
                                 Register scratch2,
                                 Label* gc_required);

  // Allocates a heap number or jumps to the gc_required label if the young
  // space is full and a scavenge is needed.
  // All registers are clobbered.
  // If no heap_number_map register is provided, the function will take care of
  // loading it.
  void AllocateHeapNumber(Register result,
                          Label* gc_required,
                          Register scratch1,
                          Register scratch2,
                          Register heap_number_map = NoReg);
  void AllocateHeapNumberWithValue(Register result,
                                   DoubleRegister value,
                                   Label* gc_required,
                                   Register scratch1,
                                   Register scratch2,
                                   Register heap_number_map = NoReg);

  // ---------------------------------------------------------------------------
  // Support functions.

  // Try to get function prototype of a function and puts the value in the
  // result register. Checks that the function really is a function and jumps
  // to the miss label if the fast checks fail. The function register will be
  // untouched; the other registers may be clobbered.
  enum BoundFunctionAction {
    kMissOnBoundFunction,
    kDontMissOnBoundFunction
  };

  void TryGetFunctionPrototype(Register function,
                               Register result,
                               Register scratch,
                               Label* miss,
                               BoundFunctionAction action =
                                 kDontMissOnBoundFunction);

  // Compare object type for heap object.  heap_object contains a non-Smi
  // whose object type should be compared with the given type.  This both
  // sets the flags and leaves the object type in the type_reg register.
  // It leaves the map in the map register (unless the type_reg and map register
  // are the same register).  It leaves the heap object in the heap_object
  // register unless the heap_object register is the same register as one of the
  // other registers.
  void CompareObjectType(Register heap_object,
                         Register map,
                         Register type_reg,
                         InstanceType type);


  // Compare object type for heap object, and branch if equal (or not.)
  // heap_object contains a non-Smi whose object type should be compared with
  // the given type.  This both sets the flags and leaves the object type in
  // the type_reg register. It leaves the map in the map register (unless the
  // type_reg and map register are the same register).  It leaves the heap
  // object in the heap_object register unless the heap_object register is the
  // same register as one of the other registers.
  void JumpIfObjectType(Register object,
                        Register map,
                        Register type_reg,
                        InstanceType type,
                        Label* if_cond_pass,
                        Condition cond = eq);

  void JumpIfNotObjectType(Register object,
                           Register map,
                           Register type_reg,
                           InstanceType type,
                           Label* if_not_object);

  // Compare instance type in a map.  map contains a valid map object whose
  // object type should be compared with the given type.  This both
  // sets the flags and leaves the object type in the type_reg register.
  void CompareInstanceType(Register map,
                           Register type_reg,
                           InstanceType type);

  // Compare an object's map with the specified map. Condition flags are set
  // with result of map compare.
  void CompareMap(Register obj,
                  Register scratch,
                  Handle<Map> map);

  // As above, but the map of the object is already loaded into the register
  // which is preserved by the code generated.
  void CompareMap(Register obj_map,
                  Handle<Map> map);

  // Check if the map of an object is equal to a specified map and branch to
  // label if not. Skip the smi check if not required (object is known to be a
  // heap object). If mode is ALLOW_ELEMENT_TRANSITION_MAPS, then also match
  // against maps that are ElementsKind transition maps of the specified map.
  void CheckMap(Register obj,
                Register scratch,
                Handle<Map> map,
                Label* fail,
                SmiCheckType smi_check_type);


  void CheckMap(Register obj,
                Register scratch,
                Heap::RootListIndex index,
                Label* fail,
                SmiCheckType smi_check_type);

  // As above, but the map of the object is already loaded into obj_map, and is
  // preserved.
  void CheckMap(Register obj_map,
                Handle<Map> map,
                Label* fail,
                SmiCheckType smi_check_type);

  // Check if the map of an object is equal to a specified map and branch to a
  // specified target if equal. Skip the smi check if not required (object is
  // known to be a heap object)
  void DispatchMap(Register obj,
                   Register scratch,
                   Handle<Map> map,
                   Handle<Code> success,
                   SmiCheckType smi_check_type);

  // Test the bitfield of the heap object map with mask and set the condition
  // flags. The object register is preserved.
  void TestMapBitfield(Register object, uint64_t mask);

  // Load the elements kind field of an object, and return it in the result
  // register.
  void LoadElementsKind(Register result, Register object);

  // Compare the object in a register to a value from the root list.
  // Uses the Tmp0() register as scratch.
  void CompareRoot(const Register& obj, Heap::RootListIndex index);

  // Compare the object in a register to a value and jump if they are equal.
  void JumpIfRoot(const Register& obj,
                  Heap::RootListIndex index,
                  Label* if_equal);

  // Compare the object in a register to a value and jump if they are not equal.
  void JumpIfNotRoot(const Register& obj,
                     Heap::RootListIndex index,
                     Label* if_not_equal);

  // Load and check the instance type of an object for being a unique name.
  // Loads the type into the second argument register.
  // The object and type arguments can be the same register; in that case it
  // will be overwritten with the type.
  // Fall-through if the object was a string and jump on fail otherwise.
  inline void IsObjectNameType(Register object, Register type, Label* fail);

  inline void IsObjectJSObjectType(Register heap_object,
                                   Register map,
                                   Register scratch,
                                   Label* fail);

  // Check the instance type in the given map to see if it corresponds to a
  // JS object type. Jump to the fail label if this is not the case and fall
  // through otherwise. However if fail label is NULL, no branch will be
  // performed and the flag will be updated. You can test the flag for "le"
  // condition to test if it is a valid JS object type.
  inline void IsInstanceJSObjectType(Register map,
                                     Register scratch,
                                     Label* fail);

  // Load and check the instance type of an object for being a string.
  // Loads the type into the second argument register.
  // The object and type arguments can be the same register; in that case it
  // will be overwritten with the type.
  // Jumps to not_string or string appropriate. If the appropriate label is
  // NULL, fall through.
  inline void IsObjectJSStringType(Register object, Register type,
                                   Label* not_string, Label* string = NULL);

  // Compare the contents of a register with an operand, and branch to true,
  // false or fall through, depending on condition.
  void CompareAndSplit(const Register& lhs,
                       const Operand& rhs,
                       Condition cond,
                       Label* if_true,
                       Label* if_false,
                       Label* fall_through);

  // Test the bits of register defined by bit_pattern, and branch to
  // if_any_set, if_all_clear or fall_through accordingly.
  void TestAndSplit(const Register& reg,
                    uint64_t bit_pattern,
                    Label* if_all_clear,
                    Label* if_any_set,
                    Label* fall_through);

  // Check if a map for a JSObject indicates that the object has fast elements.
  // Jump to the specified label if it does not.
  void CheckFastElements(Register map,
                         Register scratch,
                         Label* fail);

  // Check if a map for a JSObject indicates that the object can have both smi
  // and HeapObject elements.  Jump to the specified label if it does not.
  void CheckFastObjectElements(Register map,
                               Register scratch,
                               Label* fail);

  // Check if a map for a JSObject indicates that the object has fast smi only
  // elements. Jump to the specified label if it does not.
  void CheckFastSmiElements(Register map, Register scratch, Label* fail);

  // Check to see if number can be stored as a double in FastDoubleElements.
  // If it can, store it at the index specified by key_reg in the array,
  // otherwise jump to fail.
  void StoreNumberToDoubleElements(Register value_reg,
                                   Register key_reg,
                                   Register elements_reg,
                                   Register scratch1,
                                   FPRegister fpscratch1,
                                   FPRegister fpscratch2,
                                   Label* fail,
                                   int elements_offset = 0);

  // Picks out an array index from the hash field.
  // Register use:
  //   hash - holds the index's hash. Clobbered.
  //   index - holds the overwritten index on exit.
  void IndexFromHash(Register hash, Register index);

  // ---------------------------------------------------------------------------
  // Inline caching support.

  void EmitSeqStringSetCharCheck(Register string,
                                 Register index,
                                 SeqStringSetCharCheckIndexType index_type,
                                 Register scratch,
                                 uint32_t encoding_mask);

  // Generate code for checking access rights - used for security checks
  // on access to global objects across environments. The holder register
  // is left untouched, whereas both scratch registers are clobbered.
  void CheckAccessGlobalProxy(Register holder_reg,
                              Register scratch,
                              Label* miss);

  // Hash the interger value in 'key' register.
  // It uses the same algorithm as ComputeIntegerHash in utils.h.
  void GetNumberHash(Register key, Register scratch);

  // Load value from the dictionary.
  //
  // elements - holds the slow-case elements of the receiver on entry.
  //            Unchanged unless 'result' is the same register.
  //
  // key      - holds the smi key on entry.
  //            Unchanged unless 'result' is the same register.
  //
  // result   - holds the result on exit if the load succeeded.
  //            Allowed to be the same as 'key' or 'result'.
  //            Unchanged on bailout so 'key' or 'result' can be used
  //            in further computation.
  void LoadFromNumberDictionary(Label* miss,
                                Register elements,
                                Register key,
                                Register result,
                                Register scratch0,
                                Register scratch1,
                                Register scratch2,
                                Register scratch3);

  // ---------------------------------------------------------------------------
  // Frames.

  // Activation support.
  // Note that Tmp0() and Tmp1() are used as a scratch registers. This is safe
  // because these methods are not used in Crankshaft.
  void EnterFrame(StackFrame::Type type);
  void LeaveFrame(StackFrame::Type type);

  // Returns map with validated enum cache in object register.
  void CheckEnumCache(Register object,
                      Register null_value,
                      Register scratch0,
                      Register scratch1,
                      Register scratch2,
                      Register scratch3,
                      Label* call_runtime);

  // AllocationMemento support. Arrays may have an associated
  // AllocationMemento object that can be checked for in order to pretransition
  // to another type.
  // On entry, receiver should point to the array object.
  // If allocation info is present, the Z flag is set (so that the eq
  // condition will pass).
  void TestJSArrayForAllocationMemento(Register receiver,
                                       Register scratch1,
                                       Register scratch2,
                                       Label* no_memento_found);

  void JumpIfJSArrayHasAllocationMemento(Register receiver,
                                         Register scratch1,
                                         Register scratch2,
                                         Label* memento_found) {
    Label no_memento_found;
    TestJSArrayForAllocationMemento(receiver, scratch1, scratch2,
                                    &no_memento_found);
    B(eq, memento_found);
    Bind(&no_memento_found);
  }

  // The stack pointer has to switch between csp and jssp when setting up and
  // destroying the exit frame. Hence preserving/restoring the registers is
  // slightly more complicated than simple push/pop operations.
  void ExitFramePreserveFPRegs();
  void ExitFrameRestoreFPRegs();

  // Generates function and stub prologue code.
  void Prologue(PrologueFrameMode frame_mode);

  // Enter exit frame. Exit frames are used when calling C code from generated
  // (JavaScript) code.
  //
  // The stack pointer must be jssp on entry, and will be set to csp by this
  // function. The frame pointer is also configured, but the only other
  // registers modified by this function are the provided scratch register, and
  // jssp.
  //
  // The 'extra_space' argument can be used to allocate some space in the exit
  // frame that will be ignored by the GC. This space will be reserved in the
  // bottom of the frame immediately above the return address slot.
  //
  // Set up a stack frame and registers as follows:
  //         fp[8]: CallerPC (lr)
  //   fp -> fp[0]: CallerFP (old fp)
  //         fp[-8]: SPOffset (new csp)
  //         fp[-16]: CodeObject()
  //         fp[-16 - fp-size]: Saved doubles, if saved_doubles is true.
  //         csp[8]: Memory reserved for the caller if extra_space != 0.
  //                 Alignment padding, if necessary.
  //  csp -> csp[0]: Space reserved for the return address.
  //
  // This function also stores the new frame information in the top frame, so
  // that the new frame becomes the current frame.
  void EnterExitFrame(bool save_doubles,
                      const Register& scratch,
                      int extra_space = 0);

  // Leave the current exit frame, after a C function has returned to generated
  // (JavaScript) code.
  //
  // This effectively unwinds the operation of EnterExitFrame:
  //  * Preserved doubles are restored (if restore_doubles is true).
  //  * The frame information is removed from the top frame.
  //  * The exit frame is dropped.
  //  * The stack pointer is reset to jssp.
  //
  // The stack pointer must be csp on entry.
  void LeaveExitFrame(bool save_doubles,
                      const Register& scratch,
                      bool restore_context);

  void LoadContext(Register dst, int context_chain_length);

  // ---------------------------------------------------------------------------
  // StatsCounter support

  void SetCounter(StatsCounter* counter, int value, Register scratch1,
                  Register scratch2);
  void IncrementCounter(StatsCounter* counter, int value, Register scratch1,
                        Register scratch2);
  void DecrementCounter(StatsCounter* counter, int value, Register scratch1,
                        Register scratch2);

  // ---------------------------------------------------------------------------
  // Garbage collector support (GC).

  enum RememberedSetFinalAction {
    kReturnAtEnd,
    kFallThroughAtEnd
  };

  // Record in the remembered set the fact that we have a pointer to new space
  // at the address pointed to by the addr register. Only works if addr is not
  // in new space.
  void RememberedSetHelper(Register object,  // Used for debug code.
                           Register addr,
                           Register scratch,
                           SaveFPRegsMode save_fp,
                           RememberedSetFinalAction and_then);

  // Push and pop the registers that can hold pointers, as defined by the
  // RegList constant kSafepointSavedRegisters.
  void PushSafepointRegisters();
  void PopSafepointRegisters();

  void PushSafepointFPRegisters();
  void PopSafepointFPRegisters();

  // Store value in register src in the safepoint stack slot for register dst.
  void StoreToSafepointRegisterSlot(Register src, Register dst) {
    Poke(src, SafepointRegisterStackIndex(dst.code()) * kPointerSize);
  }

  // Load the value of the src register from its safepoint stack slot
  // into register dst.
  void LoadFromSafepointRegisterSlot(Register dst, Register src) {
    Peek(src, SafepointRegisterStackIndex(dst.code()) * kPointerSize);
  }

  void CheckPageFlagSet(const Register& object,
                        const Register& scratch,
                        int mask,
                        Label* if_any_set);

  void CheckPageFlagClear(const Register& object,
                          const Register& scratch,
                          int mask,
                          Label* if_all_clear);

  void CheckMapDeprecated(Handle<Map> map,
                          Register scratch,
                          Label* if_deprecated);

  // Check if object is in new space and jump accordingly.
  // Register 'object' is preserved.
  void JumpIfNotInNewSpace(Register object,
                           Label* branch) {
    InNewSpace(object, ne, branch);
  }

  void JumpIfInNewSpace(Register object,
                        Label* branch) {
    InNewSpace(object, eq, branch);
  }

  // Notify the garbage collector that we wrote a pointer into an object.
  // |object| is the object being stored into, |value| is the object being
  // stored.  value and scratch registers are clobbered by the operation.
  // The offset is the offset from the start of the object, not the offset from
  // the tagged HeapObject pointer.  For use with FieldOperand(reg, off).
  void RecordWriteField(
      Register object,
      int offset,
      Register value,
      Register scratch,
      LinkRegisterStatus lr_status,
      SaveFPRegsMode save_fp,
      RememberedSetAction remembered_set_action = EMIT_REMEMBERED_SET,
      SmiCheck smi_check = INLINE_SMI_CHECK);

  // As above, but the offset has the tag presubtracted. For use with
  // MemOperand(reg, off).
  inline void RecordWriteContextSlot(
      Register context,
      int offset,
      Register value,
      Register scratch,
      LinkRegisterStatus lr_status,
      SaveFPRegsMode save_fp,
      RememberedSetAction remembered_set_action = EMIT_REMEMBERED_SET,
      SmiCheck smi_check = INLINE_SMI_CHECK) {
    RecordWriteField(context,
                     offset + kHeapObjectTag,
                     value,
                     scratch,
                     lr_status,
                     save_fp,
                     remembered_set_action,
                     smi_check);
  }

  // For a given |object| notify the garbage collector that the slot |address|
  // has been written.  |value| is the object being stored. The value and
  // address registers are clobbered by the operation.
  void RecordWrite(
      Register object,
      Register address,
      Register value,
      LinkRegisterStatus lr_status,
      SaveFPRegsMode save_fp,
      RememberedSetAction remembered_set_action = EMIT_REMEMBERED_SET,
      SmiCheck smi_check = INLINE_SMI_CHECK);

  // Checks the color of an object. If the object is already grey or black
  // then we just fall through, since it is already live. If it is white and
  // we can determine that it doesn't need to be scanned, then we just mark it
  // black and fall through. For the rest we jump to the label so the
  // incremental marker can fix its assumptions.
  void EnsureNotWhite(Register object,
                      Register scratch1,
                      Register scratch2,
                      Register scratch3,
                      Register scratch4,
                      Label* object_is_white_and_not_data);

  // Detects conservatively whether an object is data-only, i.e. it does need to
  // be scanned by the garbage collector.
  void JumpIfDataObject(Register value,
                        Register scratch,
                        Label* not_data_object);

  // Helper for finding the mark bits for an address.
  // Note that the behaviour slightly differs from other architectures.
  // On exit:
  //  - addr_reg is unchanged.
  //  - The bitmap register points at the word with the mark bits.
  //  - The shift register contains the index of the first color bit for this
  //    object in the bitmap.
  inline void GetMarkBits(Register addr_reg,
                          Register bitmap_reg,
                          Register shift_reg);

  // Check if an object has a given incremental marking color.
  void HasColor(Register object,
                Register scratch0,
                Register scratch1,
                Label* has_color,
                int first_bit,
                int second_bit);

  void JumpIfBlack(Register object,
                   Register scratch0,
                   Register scratch1,
                   Label* on_black);


  // Get the location of a relocated constant (its address in the constant pool)
  // from its load site.
  void GetRelocatedValueLocation(Register ldr_location,
                                 Register result);


  // ---------------------------------------------------------------------------
  // Debugging.

  // Calls Abort(msg) if the condition cond is not satisfied.
  // Use --debug_code to enable.
  void Assert(Condition cond, BailoutReason reason);
  void AssertRegisterIsClear(Register reg, BailoutReason reason);
  void AssertRegisterIsRoot(
      Register reg,
      Heap::RootListIndex index,
      BailoutReason reason = kRegisterDidNotMatchExpectedRoot);
  void AssertFastElements(Register elements);

  // Abort if the specified register contains the invalid color bit pattern.
  // The pattern must be in bits [1:0] of 'reg' register.
  //
  // If emit_debug_code() is false, this emits no code.
  void AssertHasValidColor(const Register& reg);

  // Abort if 'object' register doesn't point to a string object.
  //
  // If emit_debug_code() is false, this emits no code.
  void AssertIsString(const Register& object);

  // Like Assert(), but always enabled.
  void Check(Condition cond, BailoutReason reason);
  void CheckRegisterIsClear(Register reg, BailoutReason reason);

  // Print a message to stderr and abort execution.
  void Abort(BailoutReason reason);

  // Conditionally load the cached Array transitioned map of type
  // transitioned_kind from the native context if the map in register
  // map_in_out is the cached Array map in the native context of
  // expected_kind.
  void LoadTransitionedArrayMapConditional(
      ElementsKind expected_kind,
      ElementsKind transitioned_kind,
      Register map_in_out,
      Register scratch,
      Label* no_map_match);

  void LoadArrayFunction(Register function);
  void LoadGlobalFunction(int index, Register function);

  // Load the initial map from the global function. The registers function and
  // map can be the same, function is then overwritten.
  void LoadGlobalFunctionInitialMap(Register function,
                                    Register map,
                                    Register scratch);

  // --------------------------------------------------------------------------
  // Set the registers used internally by the MacroAssembler as scratch
  // registers. These registers are used to implement behaviours which are not
  // directly supported by A64, and where an intermediate result is required.
  //
  // Both tmp0 and tmp1 may be set to any X register except for xzr, sp,
  // and StackPointer(). Also, they must not be the same register (though they
  // may both be NoReg).
  //
  // It is valid to set either or both of these registers to NoReg if you don't
  // want the MacroAssembler to use any scratch registers. In a debug build, the
  // Assembler will assert that any registers it uses are valid. Be aware that
  // this check is not present in release builds. If this is a problem, use the
  // Assembler directly.
  void SetScratchRegisters(const Register& tmp0, const Register& tmp1) {
    // V8 assumes the macro assembler uses ip0 and ip1 as temp registers.
    ASSERT(tmp0.IsNone() || tmp0.Is(ip0));
    ASSERT(tmp1.IsNone() || tmp1.Is(ip1));

    ASSERT(!AreAliased(xzr, csp, tmp0, tmp1));
    ASSERT(!AreAliased(StackPointer(), tmp0, tmp1));
    tmp0_ = tmp0;
    tmp1_ = tmp1;
  }

  const Register& Tmp0() const {
    return tmp0_;
  }

  const Register& Tmp1() const {
    return tmp1_;
  }

  const Register WTmp0() const {
    return Register::Create(tmp0_.code(), kWRegSize);
  }

  const Register WTmp1() const {
    return Register::Create(tmp1_.code(), kWRegSize);
  }

  void SetFPScratchRegister(const FPRegister& fptmp0) {
    fptmp0_ = fptmp0;
  }

  const FPRegister& FPTmp0() const {
    return fptmp0_;
  }

  const Register AppropriateTempFor(
      const Register& target,
      const CPURegister& forbidden = NoCPUReg) const {
    Register candidate = forbidden.Is(Tmp0()) ? Tmp1() : Tmp0();
    ASSERT(!candidate.Is(target));
    return Register::Create(candidate.code(), target.SizeInBits());
  }

  const FPRegister AppropriateTempFor(
      const FPRegister& target,
      const CPURegister& forbidden = NoCPUReg) const {
    USE(forbidden);
    FPRegister candidate = FPTmp0();
    ASSERT(!candidate.Is(forbidden));
    ASSERT(!candidate.Is(target));
    return FPRegister::Create(candidate.code(), target.SizeInBits());
  }

  // Like printf, but print at run-time from generated code.
  //
  // The caller must ensure that arguments for floating-point placeholders
  // (such as %e, %f or %g) are FPRegisters, and that arguments for integer
  // placeholders are Registers.
  //
  // A maximum of four arguments may be given to any single Printf call. The
  // arguments must be of the same type, but they do not need to have the same
  // size.
  //
  // The following registers cannot be printed:
  //    Tmp0(), Tmp1(), StackPointer(), csp.
  //
  // This function automatically preserves caller-saved registers so that
  // calling code can use Printf at any point without having to worry about
  // corruption. The preservation mechanism generates a lot of code. If this is
  // a problem, preserve the important registers manually and then call
  // PrintfNoPreserve. Callee-saved registers are not used by Printf, and are
  // implicitly preserved.
  //
  // Unlike many MacroAssembler functions, x8 and x9 are guaranteed to be
  // preserved, and can be printed. This allows Printf to be used during debug
  // code.
  //
  // This function assumes (and asserts) that the current stack pointer is
  // callee-saved, not caller-saved. This is most likely the case anyway, as a
  // caller-saved stack pointer doesn't make a lot of sense.
  void Printf(const char * format,
              const CPURegister& arg0 = NoCPUReg,
              const CPURegister& arg1 = NoCPUReg,
              const CPURegister& arg2 = NoCPUReg,
              const CPURegister& arg3 = NoCPUReg);

  // Like Printf, but don't preserve any caller-saved registers, not even 'lr'.
  //
  // The return code from the system printf call will be returned in x0.
  void PrintfNoPreserve(const char * format,
                        const CPURegister& arg0 = NoCPUReg,
                        const CPURegister& arg1 = NoCPUReg,
                        const CPURegister& arg2 = NoCPUReg,
                        const CPURegister& arg3 = NoCPUReg);

  // Code ageing support functions.

  // Code ageing on A64 works similarly to on ARM. When V8 wants to mark a
  // function as old, it replaces some of the function prologue (generated by
  // FullCodeGenerator::Generate) with a call to a special stub (ultimately
  // generated by GenerateMakeCodeYoungAgainCommon). The stub restores the
  // function prologue to its initial young state (indicating that it has been
  // recently run) and continues. A young function is therefore one which has a
  // normal frame setup sequence, and an old function has a code age sequence
  // which calls a code ageing stub.

  // Set up a basic stack frame for young code (or code exempt from ageing) with
  // type FUNCTION. It may be patched later for code ageing support. This is
  // done by to Code::PatchPlatformCodeAge and EmitCodeAgeSequence.
  //
  // This function takes an Assembler so it can be called from either a
  // MacroAssembler or a PatchingAssembler context.
  static void EmitFrameSetupForCodeAgePatching(Assembler* assm);

  // Call EmitFrameSetupForCodeAgePatching from a MacroAssembler context.
  void EmitFrameSetupForCodeAgePatching();

  // Emit a code age sequence that calls the relevant code age stub. The code
  // generated by this sequence is expected to replace the code generated by
  // EmitFrameSetupForCodeAgePatching, and represents an old function.
  //
  // If stub is NULL, this function generates the code age sequence but omits
  // the stub address that is normally embedded in the instruction stream. This
  // can be used by debug code to verify code age sequences.
  static void EmitCodeAgeSequence(Assembler* assm, Code* stub);

  // Call EmitCodeAgeSequence from a MacroAssembler context.
  void EmitCodeAgeSequence(Code* stub);

  // Return true if the sequence is a young sequence geneated by
  // EmitFrameSetupForCodeAgePatching. Otherwise, this method asserts that the
  // sequence is a code age sequence (emitted by EmitCodeAgeSequence).
  static bool IsYoungSequence(byte* sequence);

#ifdef DEBUG
  // Return true if the sequence is a code age sequence generated by
  // EmitCodeAgeSequence.
  static bool IsCodeAgeSequence(byte* sequence);
#endif

  // Jumps to found label if a prototype map has dictionary elements.
  void JumpIfDictionaryInPrototypeChain(Register object, Register scratch0,
                                        Register scratch1, Label* found);

 private:
  // Helpers for CopyFields.
  // These each implement CopyFields in a different way.
  void CopyFieldsLoopPairsHelper(Register dst, Register src, unsigned count,
                                 Register scratch1, Register scratch2,
                                 Register scratch3);
  void CopyFieldsUnrolledPairsHelper(Register dst, Register src, unsigned count,
                                     Register scratch1, Register scratch2);
  void CopyFieldsUnrolledHelper(Register dst, Register src, unsigned count,
                                Register scratch1);

  // The actual Push and Pop implementations. These don't generate any code
  // other than that required for the push or pop. This allows
  // (Push|Pop)CPURegList to bundle together run-time assertions for a large
  // block of registers.
  //
  // Note that size is per register, and is specified in bytes.
  void PushHelper(int count, int size,
                  const CPURegister& src0, const CPURegister& src1,
                  const CPURegister& src2, const CPURegister& src3);
  void PopHelper(int count, int size,
                 const CPURegister& dst0, const CPURegister& dst1,
                 const CPURegister& dst2, const CPURegister& dst3);

  // Perform necessary maintenance operations before a push or pop.
  //
  // Note that size is specified in bytes.
  void PrepareForPush(Operand total_size);
  void PrepareForPop(Operand total_size);

  void PrepareForPush(int count, int size) { PrepareForPush(count * size); }
  void PrepareForPop(int count, int size) { PrepareForPop(count * size); }

  // Call Printf. On a native build, a simple call will be generated, but if the
  // simulator is being used then a suitable pseudo-instruction is used. The
  // arguments and stack (csp) must be prepared by the caller as for a normal
  // AAPCS64 call to 'printf'.
  //
  // The 'type' argument specifies the type of the optional arguments.
  void CallPrintf(CPURegister::RegisterType type = CPURegister::kNoRegister);

  // Helper for throwing exceptions.  Compute a handler address and jump to
  // it.  See the implementation for register usage.
  void JumpToHandlerEntry(Register exception,
                          Register object,
                          Register state,
                          Register scratch1,
                          Register scratch2);

  // Helper for implementing JumpIfNotInNewSpace and JumpIfInNewSpace.
  void InNewSpace(Register object,
                  Condition cond,  // eq for new space, ne otherwise.
                  Label* branch);

  // Try to convert a double to an int so that integer fast-paths may be
  // used. Not every valid integer value is guaranteed to be caught.
  // It supports both 32-bit and 64-bit integers depending whether 'as_int'
  // is a W or X register.
  //
  // This does not distinguish between +0 and -0, so if this distinction is
  // important it must be checked separately.
  void TryConvertDoubleToInt(Register as_int,
                             FPRegister value,
                             FPRegister scratch_d,
                             Label* on_successful_conversion,
                             Label* on_failed_conversion = NULL);

  bool generating_stub_;
#if DEBUG
  // Tell whether any of the macro instruction can be used. When false the
  // MacroAssembler will assert if a method which can emit a variable number
  // of instructions is called.
  bool allow_macro_instructions_;
#endif
  bool has_frame_;

  // The Abort method should call a V8 runtime function, but the CallRuntime
  // mechanism depends on CEntryStub. If use_real_aborts is false, Abort will
  // use a simpler abort mechanism that doesn't depend on CEntryStub.
  //
  // The purpose of this is to allow Aborts to be compiled whilst CEntryStub is
  // being generated.
  bool use_real_aborts_;

  // This handle will be patched with the code object on installation.
  Handle<Object> code_object_;

  // The register to use as a stack pointer for stack operations.
  Register sp_;

  // Scratch registers used internally by the MacroAssembler.
  Register tmp0_;
  Register tmp1_;
  FPRegister fptmp0_;

  void InitializeNewString(Register string,
                           Register length,
                           Heap::RootListIndex map_index,
                           Register scratch1,
                           Register scratch2);

 public:
  // Far branches resolving.
  //
  // The various classes of branch instructions with immediate offsets have
  // different ranges. While the Assembler will fail to assemble a branch
  // exceeding its range, the MacroAssembler offers a mechanism to resolve
  // branches to too distant targets, either by tweaking the generated code to
  // use branch instructions with wider ranges or generating veneers.
  //
  // Currently branches to distant targets are resolved using unconditional
  // branch isntructions with a range of +-128MB. If that becomes too little
  // (!), the mechanism can be extended to generate special veneers for really
  // far targets.

  // Returns true if we should emit a veneer as soon as possible for a branch
  // which can at most reach to specified pc.
  bool ShouldEmitVeneer(int max_reachable_pc,
                        int margin = kVeneerDistanceMargin);

  // The maximum code size generated for a veneer. Currently one branch
  // instruction. This is for code size checking purposes, and can be extended
  // in the future for example if we decide to add nops between the veneers.
  static const int kMaxVeneerCodeSize = 1 * kInstructionSize;

  // Emits veneers for branches that are approaching their maximum range.
  // If need_protection is true, the veneers are protected by a branch jumping
  // over the code.
  void EmitVeneers(bool need_protection);
  void EmitVeneersGuard();
  // Checks wether veneers need to be emitted at this point.
  void CheckVeneers(bool need_protection);

  // Helps resolve branching to labels potentially out of range.
  // If the label is not bound, it registers the information necessary to later
  // be able to emit a veneer for this branch if necessary.
  // If the label is bound, it returns true if the label (or the previous link
  // in the label chain) is out of range. In that case the caller is responsible
  // for generating appropriate code.
  // Otherwise it returns false.
  // This function also checks wether veneers need to be emitted.
  bool NeedExtraInstructionsOrRegisterBranch(Label *label,
                                             ImmBranchType branch_type);

 private:
  // We generate a veneer for a branch if we reach within this distance of the
  // limit of the range.
  static const int kVeneerDistanceMargin = 2 * KB;
  int unresolved_branches_first_limit() const {
    ASSERT(!unresolved_branches_.empty());
    return unresolved_branches_.begin()->first;
  }
};


// Use this scope when you need a one-to-one mapping bewteen methods and
// instructions. This scope prevents the MacroAssembler from being called and
// literal pools from being emitted. It also asserts the number of instructions
// emitted is what you specified when creating the scope.
class InstructionAccurateScope BASE_EMBEDDED {
 public:
  InstructionAccurateScope(MacroAssembler* masm, size_t count = 0)
      : masm_(masm)
#ifdef DEBUG
        ,
        size_(count * kInstructionSize)
#endif
  {
    // Before blocking the const pool, see if it needs to be emitted.
    masm_->CheckConstPool(false, true);

    masm_->StartBlockConstPool();
#ifdef DEBUG
    if (count != 0) {
      masm_->bind(&start_);
    }
    previous_allow_macro_instructions_ = masm_->allow_macro_instructions();
    masm_->set_allow_macro_instructions(false);
#endif
  }

  ~InstructionAccurateScope() {
    masm_->EndBlockConstPool();
#ifdef DEBUG
    if (start_.is_bound()) {
      ASSERT(masm_->SizeOfCodeGeneratedSince(&start_) == size_);
    }
    masm_->set_allow_macro_instructions(previous_allow_macro_instructions_);
#endif
  }

 private:
  MacroAssembler* masm_;
#ifdef DEBUG
  size_t size_;
  Label start_;
  bool previous_allow_macro_instructions_;
#endif
};


inline MemOperand ContextMemOperand(Register context, int index) {
  return MemOperand(context, Context::SlotOffset(index));
}

inline MemOperand GlobalObjectMemOperand() {
  return ContextMemOperand(cp, Context::GLOBAL_OBJECT_INDEX);
}


// Encode and decode information about patchable inline SMI checks.
class InlineSmiCheckInfo {
 public:
  explicit InlineSmiCheckInfo(Address info);

  bool HasSmiCheck() const {
    return smi_check_ != NULL;
  }

  const Register& SmiRegister() const {
    return reg_;
  }

  Instruction* SmiCheck() const {
    return smi_check_;
  }

  // Use MacroAssembler::InlineData to emit information about patchable inline
  // SMI checks. The caller may specify 'reg' as NoReg and an unbound 'site' to
  // indicate that there is no inline SMI check. Note that 'reg' cannot be csp.
  //
  // The generated patch information can be read using the InlineSMICheckInfo
  // class.
  static void Emit(MacroAssembler* masm, const Register& reg,
                   const Label* smi_check);

  // Emit information to indicate that there is no inline SMI check.
  static void EmitNotInlined(MacroAssembler* masm) {
    Label unbound;
    Emit(masm, NoReg, &unbound);
  }

 private:
  Register reg_;
  Instruction* smi_check_;

  // Fields in the data encoded by InlineData.

  // A width of 5 (Rd_width) for the SMI register preclues the use of csp,
  // since kSPRegInternalCode is 63. However, csp should never hold a SMI or be
  // used in a patchable check. The Emit() method checks this.
  //
  // Note that the total size of the fields is restricted by the underlying
  // storage size handled by the BitField class, which is a uint32_t.
  class RegisterBits : public BitField<unsigned, 0, 5> {};
  class DeltaBits : public BitField<uint32_t, 5, 32-5> {};
};

} }  // namespace v8::internal

#ifdef GENERATED_CODE_COVERAGE
#error "Unsupported option"
#define CODE_COVERAGE_STRINGIFY(x) #x
#define CODE_COVERAGE_TOSTRING(x) CODE_COVERAGE_STRINGIFY(x)
#define __FILE_LINE__ __FILE__ ":" CODE_COVERAGE_TOSTRING(__LINE__)
#define ACCESS_MASM(masm) masm->stop(__FILE_LINE__); masm->
#else
#define ACCESS_MASM(masm) masm->
#endif

#endif  // V8_A64_MACRO_ASSEMBLER_A64_H_

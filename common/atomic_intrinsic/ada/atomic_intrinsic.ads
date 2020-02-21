with System;
with Interfaces.C;
use Interfaces.C;

package Atomic_Intrinsic is
   pragma Preelaborate;
   pragma Elaborate_Body; --Needed for CMake

   -- GCC Intrinsic for atomic exchange of 4-byte variable and return old value
   function TAS_4(
      current: access Unsigned;
      Newval: Unsigned) return Unsigned;
   pragma Import (Intrinsic, TAS_4, "__sync_lock_test_and_set_4");

   -- GCC Intrinsic for atomic release of 4-byte lock
   procedure Lock_Release_4(current : access Unsigned);
   pragma Import(Intrinsic, Lock_Release_4, "__sync_lock_release_4");

   -- Bindings for GCC atomic intrinsics
   type MemOrder_Type is (RLX, CON, ACQ, REL, AR, SC);
   type C_Memorder is new Interfaces.C.Int;

   function Memory_Order (M : MemOrder_Type) return C_Memorder is (MemOrder_Type'Pos(M));

   procedure Atomic_Load(Ptr: System.Address; Ret: System.Address; M: C_Memorder; Size: Integer);
   pragma Import (C, Atomic_Load, "gcc_atomic_load");

   procedure Atomic_Store(Ptr: System.Address; Val: System.Address; M: C_Memorder; Size: Integer);
   pragma Import (C, Atomic_Store, "gcc_atomic_store");

   function Atomic_Compare_Exchange(
      Current : System.Address;
      Expected: System.Address;
      Desired : System.Address;
      Size    : Integer;
      Weak    : Boolean := True;
      Succ    : C_Memorder := Memory_Order(SC);
      Fail    : C_Memorder := Memory_Order(SC)) return Boolean;
   pragma Import (Intrinsic, Atomic_Compare_Exchange, "gcc_atomic_compare_exchange");

end Atomic_Intrinsic;

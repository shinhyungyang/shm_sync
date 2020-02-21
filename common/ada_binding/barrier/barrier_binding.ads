package Barrier_Binding is
   pragma Elaborate_Body; --Needed for CMake

   procedure Barrier_Init (Nr_Threads : Integer);
   pragma Import (C, Barrier_Init, "barrier_init");

   procedure Barrier_Wait (Tid : Integer);
   pragma Import (C, Barrier_Wait, "barrier_wait");
end Barrier_Binding;

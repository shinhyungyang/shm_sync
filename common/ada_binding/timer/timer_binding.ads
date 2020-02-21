package Timer_Binding is
   pragma Elaborate_Body; --Needed for CMake

   procedure Useful_Work_Ns (Sec : Integer);
   pragma Import (C, Useful_Work_Ns, "useful_work_ns");

   procedure Useful_Work_Us (Sec : Integer);
   pragma Import (C, Useful_Work_Us, "useful_work_us");

   procedure Calibrate;
   pragma Import (C, Calibrate, "calibrate_delay");
end Timer_Binding;

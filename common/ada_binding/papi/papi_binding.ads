package PAPI_Binding is

   procedure PAPI_Start;
   procedure PAPI_Finish;

   procedure PAPI_measure_start(tid : in Integer; use_TSX : Boolean := False);
   procedure PAPI_measure_end(tid : in Integer; use_TSX : Boolean := False);
   function PAPI_get_measured(tid : in Integer) return Long_Integer;

private
   type u64 is mod 2**64;
   elapsed : u64;

   type PAPI_Event is access Integer;

   procedure PAPI_Setup;
      pragma Import (C, PAPI_setup, "PAPI_setup");

   function PAPI_Measure return u64;
      pragma Import (C, PAPI_measure, "PAPI_measure");

   procedure PAPI_Start_nsec(tid : in Integer);
      pragma Import (C, PAPI_Start_nsec, "PAPI_start_nsec");

   procedure PAPI_End_nsec(tid : in Integer);
      pragma Import (C, PAPI_End_nsec, "PAPI_end_nsec");

   function PAPI_Get_nsec(tid : in Integer) return Long_Integer;
      pragma Import (C, PAPI_Get_nsec, "PAPI_get_nsec");

   procedure PAPI_Start_TSX (Event_Set : PAPI_Event);
      pragma Import (C, PAPI_Start_TSX, "PAPI_start_tsx");

   procedure PAPI_End_TSX (Event_Set : PAPI_Event);
      pragma Import (C, PAPI_End_TSX, "PAPI_end_tsx");

end PAPI_Binding;

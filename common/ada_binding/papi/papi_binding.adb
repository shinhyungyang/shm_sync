with Ada.Text_IO; use Ada.Text_IO;

package body PAPI_Binding is
   Event : PAPI_Event := new Integer;

   procedure PAPI_Start is
   begin
      PAPI_Setup;
   end PAPI_Start;

   procedure PAPI_Finish is
   begin
      elapsed := PAPI_Measure;
      Put_Line ("elapsed(ns): "& u64'Image (elapsed));
   end PAPI_Finish;

   procedure PAPI_measure_start (tid : in Integer; use_TSX : Boolean := False) is
   begin
      PAPI_Start_nsec(tid);
      if use_TSX then
         PAPI_Start_TSX (Event);
      end if;
   end PAPI_measure_start;

   procedure PAPI_measure_end(tid : in Integer; use_TSX : Boolean := False) is
   begin
      PAPI_End_nsec(tid);
      if use_TSX then
         PAPI_End_TSX (Event);
      end if;
   end PAPI_measure_end;

   function PAPI_Get_Measured(tid : in Integer) return Long_Integer is
   begin
      return PAPI_Get_nsec(tid);
   end PAPI_Get_Measured;
end PAPI_Binding;

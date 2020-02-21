with Interfaces.C;
use Interfaces.C;

package Stats_Binding is

   procedure InitStats (Nr_Threads : Integer; Nr_Exec : Integer; Nr_Runs : Integer);
   pragma Import (C, InitStats, "initStats");

   procedure EnterSample (s : Long_Integer; Tid : Integer; Nr_Execution : Integer; Run : Integer);
   pragma Import (C, EnterSample, "enterSample");

   procedure DumpSingleData (Ovw_Only : Integer);
   pragma Import (C, DumpSingleData, "dumpSingle");

   procedure DumpPair (Ovw_Only : Integer; Nr_Pairs : Integer; B_Name : char_array; Sc_Runs : Integer);
   pragma Import (C, DumpPair, "dumpPair");

   procedure DumpPairData (Ovw_Only : Integer; Nr_Pairs : Integer; B_Name : String := ""; Sc_Runs : Integer);
end Stats_Binding;

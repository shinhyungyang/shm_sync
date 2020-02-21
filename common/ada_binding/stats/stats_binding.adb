with Interfaces.C;
use Interfaces.C;

package body Stats_Binding is
   procedure DumpPairData (Ovw_Only : Integer; Nr_Pairs : Integer; B_Name : String := ""; Sc_Runs : Integer) is
   begin
      DumpPair(Ovw_Only, Nr_Pairs, To_C(B_Name), Sc_Runs);
   end DumpPairData;
end Stats_Binding;

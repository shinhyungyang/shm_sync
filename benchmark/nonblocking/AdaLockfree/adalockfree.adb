with Ada.Text_IO;
use Ada.Text_IO;
with GNAT.Command_line;
use GNAT.Command_Line;
with Driver_Adalockfree;

procedure Adalockfree is

   procedure Print_Help is
   begin
      Put_Line("Ada Lock-free queue alogrithm");
      Put_Line("Usage:");
      Put_Line("  driver_adalockfree [OPTION...] positional parameters");
      New_Line;
      Put_Line("  -e, --executions arg   Number of executions");
      Put_Line("  -i, --iterations arg   Number of iterations");
      Put_Line("  -t, --threadpairs arg  Number of prod/cons thread-pairs");
      Put_Line("  -r, --runs             Scaling runs, 1..t thread-pairs");
      Put_Line("  -o, --ovw              Overview statistics only");
      Put_Line("  -h, --help             Usage information");
      Put_Line("  -y, --useful work in            Usage information");
   end Print_Help;

   Nr_Exec : Integer := 1;
   Nr_Iter : Integer := 10000000;
   Nr_Pairs : Integer := 1;
   Sc_Runs : Boolean := False;
   Ovw_Stats : Boolean := False;

   Show_Help : Boolean := False;
   Uout : Integer := 0;

begin
   loop
      case GetOpt("e: i: t: u: y: r o h -execution: -iterations: -threadpairs: -uout -runs -ovw -help") is
         when 'e' => Nr_Exec := Integer'Value (Parameter);
         when 'i' => Nr_Iter := Integer'Value (Parameter);
         when 't' => Nr_Pairs := Integer'Value (Parameter);
         when 'y' => Uout := Integer'Value (Parameter);
         when 'r' => Sc_Runs := True; 
         when 'o' => Ovw_Stats := True;
         when 'h' => Show_Help := True;
         when '-' =>
            if Full_Switch = "-execution" then
               Nr_Exec := Integer'Value (Parameter);
            elsif Full_Switch = "-iterations" then
               Nr_Iter := Integer'Value (Parameter);
            elsif Full_Switch = "-threadpairs" then
               Nr_Pairs := Integer'Value (Parameter);
            elsif Full_Switch = "-runs" then
               Sc_Runs := True; 
            elsif Full_Switch = "-ovw" then
               Ovw_Stats := True;
            elsif Full_Switch = "-help" then
               Show_Help := True;
            elsif Full_Switch = "-uout" then
               Uout := Integer'Value (Parameter);
            end if;
         when others =>
            exit;
      end case;
   end loop;

   if Show_Help then
      Print_Help;
      return;
   end if;

   Driver_AdaLockfree.Driver_AdaLockfree (Nr_Exec, Nr_Iter, Nr_Pairs, Sc_Runs, Ovw_Stats, Uout); -- Uin isn't inserted as an argument
end Adalockfree;

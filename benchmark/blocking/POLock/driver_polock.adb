with GNAT.Command_line;
use GNAT.Command_Line;
with Papi_Binding;
with Barrier_Binding;
with Stats_Binding;
with Timer_Binding;
with PO_Lock;

with Ada.Text_IO; use Ada.Text_IO;
procedure Driver_POLock is

   polock : PO_Lock.Lock;

   task type Thread_F is
      entry Start (id : Integer; Nr_Th : Integer; Nr_It : Integer; Nr_Ex : Integer; Sc_R : Boolean; Uout : Integer; Uin : Integer);
   end Thread_F;
   task body Thread_F is
      Nr_Thrds_Lower_Bound : Integer := 1;
      Tid, Nr_Threads, Nr_Iter, Nr_Exec, Uout_delay, Uin_delay, Iter_Per_Threads, Remainder : Integer;
      Sc_Runs : Boolean;
   begin
      accept Start (id : Integer; Nr_Th : Integer; Nr_It : Integer; Nr_Ex : Integer; Sc_R : Boolean; Uout : Integer; Uin : Integer) do
         Tid := id;
         Nr_Threads := Nr_Th;
         Nr_Iter := Nr_It;
         Nr_Exec := Nr_Ex;
         Sc_Runs := Sc_R;
         Uout_delay := Uout;
         Uin_delay := Uin;
      end Start;
      
      if not Sc_Runs then
         Nr_Thrds_Lower_Bound := Nr_Threads;
         Iter_Per_Threads := Nr_Iter / Nr_Threads;
         Remainder := Nr_Iter mod Nr_Threads;
         if Tid < Remainder then
            Iter_Per_Threads := Iter_Per_Threads + 1;
         end if;
      end if;
      for run in 0 .. Nr_Threads - Nr_Thrds_Lower_Bound loop
         if Sc_Runs then
            Iter_Per_Threads := Nr_Iter / (run + 1);
            Remainder := Nr_Iter mod (run + 1);
            if Tid < Remainder then
               Iter_Per_Threads := Iter_Per_Threads + 1;
            end if;
         end if;
         for exec in 0 .. Nr_Exec-1 loop
            Barrier_Binding.Barrier_Wait (Tid);
            if not Sc_Runs or Tid <= run then
               Papi_Binding.PAPI_Measure_Start (Tid);

               for i in 1 .. Iter_Per_Threads loop
                  polock.CriticalSection(Uin_delay);
                  Timer_Binding.Useful_Work_Ns(Uout_delay); -- Useful work (Nano second)
               end loop;

               Papi_Binding.PAPI_Measure_End (Tid);
               Stats_Binding.EnterSample (Papi_Binding.PAPI_Get_Measured (Tid), Tid, exec, run);
            end if;
         end loop;
      end loop;
   end Thread_F;

   procedure Print_Help is
   begin
      Put_Line("Ada's lock implementation by protected objects");
      Put_Line("Usage:");
      Put_Line("  driver_polock [OPTION...] positional parameters");
      New_Line;
      Put_Line("  -e, --executions arg  Number of executions");
      Put_Line("  -i, --iterations arg  Number of iterations");
      Put_Line("  -t, --threads arg     Number of threads");
      Put_Line("  -r, --runs            Scaling runs, 1..t threads");
      Put_Line("  -o, --ovw             Overview statistics only");
      Put_Line("  -h, --help            Usage information");
      Put_Line("  -u, --useful work in  Busy loop inside critical section");
      Put_Line("  -y, --useful work out Busy loop between iterations");
   end Print_Help;

   Nr_Exec : Integer := 1;
   Nr_Iter : Integer := 10000000;
   Nr_Threads : Integer := 1;
   Sc_Runs : Boolean := False;
   Ovw_Stats : Boolean := False;
   Show_Help : Boolean := False;
   Nr_Runs : Integer;
   Uout : Integer := 0;
   Uin : Integer := 0;
begin
   loop
     case GetOpt("e: i: t: u: y: r o h -execution: -iterations: -threads: -runs -uout -uin -ovw -help") is
         when 'e' => Nr_Exec := Integer'Value (Parameter);
         when 'i' => Nr_Iter := Integer'Value (Parameter);
         when 't' => Nr_Threads := Integer'Value (Parameter);
         when 'u' => Uin := Integer'Value (Parameter);
         when 'y' => Uout := Integer'Value (Parameter);
         when 'r' => Sc_Runs := True; 
         when 'o' => Ovw_Stats := True;
         when 'h' => Show_Help := True;
         when '-' =>
            if Full_Switch = "-execution" then
               Nr_Exec := Integer'Value (Parameter);
            elsif Full_Switch = "-iterations" then
               Nr_Iter := Integer'Value (Parameter);
            elsif Full_Switch = "-threads" then
               Nr_Threads := Integer'Value (Parameter);
            elsif Full_Switch = "-runs" then
               Sc_Runs := True; 
            elsif Full_Switch = "-ovw" then
               Ovw_Stats := True;
            elsif Full_Switch = "-help" then
               Show_Help := True;
            elsif Full_Switch = "-uin" then
               Nr_Threads := Integer'Value (Parameter);
            elsif Full_Switch = "-uout" then
               Nr_Threads := Integer'Value (Parameter);
            end if;
         when others =>
            exit;
      end case;
   end loop;

   if Show_Help then
      Print_Help;
      return;
   end if;

   if Sc_Runs then Nr_Runs := Nr_Threads;
   else Nr_Runs := 1;
   end if;
   Stats_Binding.InitStats (Nr_Threads, Nr_Exec, Nr_Runs);
   Barrier_Binding.Barrier_Init (Nr_Threads);
   Timer_Binding.Calibrate;
   Papi_Binding.PAPI_Start;

   declare 
      Tids : array (0 .. Nr_Threads-1) of Thread_F;
   begin
      for tid in 0 .. Nr_Threads-1 loop
         Tids (tid).Start (tid, Nr_Threads, Nr_Iter, Nr_Exec, Sc_Runs, Uout, Uin);
      end loop;
   end;
   Stats_Binding.DumpSingleData (Integer(Boolean'Pos(Ovw_Stats)));
end Driver_POLock;

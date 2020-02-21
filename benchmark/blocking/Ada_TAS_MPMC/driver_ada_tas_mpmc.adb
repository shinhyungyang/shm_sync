with GNAT.Command_line;
use GNAT.Command_Line;
with Papi_Binding;
with Barrier_Binding;
with Stats_Binding;
with Timer_Binding;
with Ada.Text_IO;
use Ada.Text_IO;
with ADATAS;

procedure Driver_Ada_TAS_MPMC is

   task type Producer_F is
      entry Start (id : Integer; Nr_P : Integer; Nr_It : Integer; Nr_Ex : Integer; Sc_R : Boolean; Uout : Integer);
   end Producer_F;
   task body Producer_F is
      Nr_Runs, Tid, Nr_Pairs, Nr_Iter, Nr_Exec, Iter_Per_Threads, Remainder : Integer;
      Uout_delay : Integer;
      Sc_Runs : Boolean;
      Data : Integer := 100;
   begin
      accept Start (id : Integer; Nr_P : Integer; Nr_It : Integer; Nr_Ex : Integer; Sc_R : Boolean; Uout : Integer) do
         Tid := id;
         Nr_Pairs := Nr_P;
         Nr_Iter := Nr_It;
         Nr_Exec := Nr_Ex;
         Sc_Runs := Sc_R;
         Uout_delay := Uout;
      end Start;
      
      if Sc_Runs then
         Nr_Runs := Nr_Pairs;
      else
         Nr_Runs := 1;
         Iter_Per_Threads := Nr_Iter / Nr_Pairs;
         Remainder := Nr_Iter mod Nr_Pairs;
         if Tid < Remainder then
            Iter_Per_Threads := Iter_Per_Threads + 1;
         end if;
      end if;
      for run in 0 .. Nr_Runs-1 loop
         -- Fixed amount of iterations
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

               ADATAS.sync_Producer(Iter_Per_Threads, Uout_delay);

               Papi_Binding.PAPI_Measure_End (Tid);
               Stats_Binding.EnterSample (Papi_Binding.PAPI_Get_Measured (Tid), Tid, exec, run);
            end if;
         end loop;
      end loop;
   end Producer_F;

   task type Consumer_F is
      entry Start (id : Integer; Nr_P : Integer; Nr_It : Integer; Nr_Ex : Integer; Sc_R : Boolean; Uout : Integer);
   end Consumer_F;
   task body Consumer_F is
      Nr_Runs, Tid, Nr_Pairs, Nr_Iter, Nr_Exec, Uout_delay, Iter_Per_Threads, Remainder : Integer;
      Sc_Runs : Boolean;
      Data, iter_cnt: Integer := 0;
   begin
      accept Start (id : Integer; Nr_P : Integer; Nr_It : Integer; Nr_Ex : Integer; Sc_R : Boolean; Uout : Integer) do
         Tid := id + Nr_P;
         Nr_Pairs := Nr_P;
         Nr_Iter := Nr_It;
         Nr_Exec := Nr_Ex;
         Sc_Runs := Sc_R;
         Uout_delay := Uout;
      end Start;
      
      if Sc_Runs then
         Nr_Runs := Nr_Pairs;
      else
         Nr_Runs := 1;
         Iter_Per_Threads := Nr_Iter / Nr_Pairs;
         Remainder := Nr_Iter mod Nr_Pairs;
         if Tid < Remainder then
            Iter_Per_Threads := Iter_Per_Threads + 1;
         end if;
      end if;
      for run in 0 .. Nr_Runs-1 loop
         if Sc_Runs then
            Iter_Per_Threads := Nr_Iter / (run + 1);
            Remainder := Nr_Iter mod (run + 1);
            if (Tid - Nr_Pairs) < Remainder then
               Iter_Per_Threads := Iter_Per_Threads + 1;
            end if;
         end if;
         for exec in 0 .. Nr_Exec-1 loop
            iter_cnt := 0;
            Barrier_Binding.Barrier_Wait (Tid);
            if not Sc_Runs or Tid <= run + Nr_Pairs then
               Papi_Binding.PAPI_Measure_Start (Tid);

               ADATAS.sync_Consumer(Iter_Per_Threads, Uout_delay);

               Papi_Binding.PAPI_Measure_End (Tid);
               Stats_Binding.EnterSample (Papi_Binding.PAPI_Get_Measured (Tid), Tid, exec, run);
            end if;
         end loop;
      end loop;
   end Consumer_F;

   procedure Print_Help is
   begin
      Put_Line("Ada TAS MPMC queue alogrithm");
      Put_Line("Usage:");
      Put_Line("  driver_Ada_TAS_MPMC [OPTION...] positional parameters");
      New_Line;
      Put_Line("  -e, --executions arg   Number of executions");
      Put_Line("  -i, --iterations arg   Number of iterations");
      Put_Line("  -t, --threadpairs arg  Number of prod/cons thread-pairs");
      Put_Line("  -r, --runs             Scaling runs, 1..t thread-pairs");
      Put_Line("  -o, --ovw              Overview statistics only");
      Put_Line("  -h, --help             Usage information");
      Put_Line("  -y, --useful work out  Busy loop between iterations");
   end Print_Help;

   Nr_Exec : Integer := 1;
   Nr_Iter : Integer := 10000000;
   Nr_Pairs : Integer := 1;
   Sc_Runs : Boolean := False;
   Ovw_Stats : Boolean := False;
   Show_Help : Boolean := False;
   Nr_Runs : Integer;
   Uout : Integer := 0;

begin
   loop
   case GetOpt("e: i: t: y: r o h -execution: -iterations: -threadpairs: -runs -ovw -uout -help") is
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
            elsif Full_Switch = "-uout" then
               Uout := Integer'Value (Parameter);
            elsif Full_Switch = "-runs" then
               Sc_Runs := True; 
            elsif Full_Switch = "-ovw" then
               Ovw_Stats := True;
            elsif Full_Switch = "-help" then
               Show_Help := True;
            end if;
         when others =>
            exit;
      end case;
   end loop;

   if Show_Help then
      Print_Help;
      return;
   end if;


   if Sc_Runs then Nr_Runs := Nr_Pairs;
   else Nr_Runs := 1;
   end if;
   Stats_Binding.InitStats (2 * Nr_Pairs, Nr_Exec, Nr_Runs);
   Barrier_Binding.Barrier_Init (2 * Nr_Pairs);
   Papi_Binding.PAPI_Start;
   Timer_Binding.Calibrate;

   declare 
      P : array (0 .. Nr_Pairs-1) of Producer_F;
      C : array (0 .. Nr_Pairs-1) of Consumer_F;
   begin
      for tid in 0 .. Nr_Pairs-1 loop
         P (tid).Start (tid, Nr_Pairs, Nr_Iter, Nr_Exec, Sc_Runs, Uout);
         C (tid).Start (tid, Nr_Pairs, Nr_Iter, Nr_Exec, Sc_Runs, Uout);
      end loop;
   end;

   Stats_Binding.DumpPairData (Integer(Boolean'Pos(Ovw_Stats)), Nr_Pairs, "Ada_LockFree", Integer(Boolean'Pos(Sc_Runs)));
end Driver_Ada_TAS_MPMC;

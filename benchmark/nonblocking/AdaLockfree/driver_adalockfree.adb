with Queue_Interfaces;
with Unbounded_Queues;
with Papi_Binding;
with Barrier_Binding;
with Stats_Binding;
with Timer_Binding;
with Ada.Text_IO;
use Ada.Text_IO;

package body Driver_AdaLockFree is
   package My_SQI is new Queue_interfaces(Element_Type => Integer);
   package My_USQ is new Unbounded_Queues(QIs => My_SQI);
   Q: My_USQ.Queue;

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

               for i in 1 .. Iter_Per_Threads loop
                  Q.Enqueue(New_Item => Data);
                  Timer_Binding.Useful_Work_Ns(Uout_delay); -- Useful work (Nano second)
               end loop;

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
      Nr_Runs, Tid, Nr_Pairs, Nr_Iter, Nr_Exec, Iter_Per_Threads, Remainder : Integer;
      Uout_delay : Integer;
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
         -- Fixed amount of iterations
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

               while iter_cnt < Iter_Per_Threads loop
                  if Q.Dequeue(Element => Data) then
                     iter_cnt := iter_cnt + 1;
                     Timer_Binding.Useful_Work_Ns(Uout_delay); -- Useful work (nano second)
                  end if;
               end loop;

               Papi_Binding.PAPI_Measure_End (Tid);
               Stats_Binding.EnterSample (Papi_Binding.PAPI_Get_Measured (Tid), Tid, exec, run);
            end if;
         end loop; -- exec loop
      end loop; -- run loop
   end Consumer_F;

   -- Enqueue & Dequeue pair task : not included in the benchmark yet.
   task type Prod_Cons_F is
      entry Start (id : Integer; Nr_P : Integer; Nr_It : Integer; Nr_Ex : Integer; Sc_R : Boolean; Uout : Integer);
   end Prod_Cons_F;
   task body Prod_Cons_F is
      Nr_Runs, Tid, Nr_Pairs, Nr_Iter, Nr_Exec, Iter_Per_Threads, Remainder : Integer;
      Uout_delay : Integer;
      Sc_Runs : Boolean;
      Data, iter_cnt: Integer := 0;
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
      end if;

      for run in 0 .. Nr_Runs-1 loop
         if Sc_Runs then
            Iter_Per_Threads := Nr_Iter / Nr_Pairs;
            Remainder := Nr_Iter mod Nr_Pairs;
            if Tid < Remainder then
               Iter_Per_Threads := Iter_Per_Threads + 1;
            end if;
         end if;

         for exec in 0 .. Nr_Exec-1 loop

            Barrier_Binding.Barrier_Wait (Tid);
            if not Sc_Runs or Tid <= run then
               Papi_Binding.PAPI_Measure_Start (Tid);

               for i in 1 .. Iter_Per_Threads loop
                  Q.Enqueue(New_Item => Data);

                  Timer_Binding.Useful_Work_Ns(Uout_delay);

                  if Q.Dequeue(Element => Data) then
                    iter_cnt := iter_cnt + 1;
                  end if;

                  Timer_Binding.Useful_Work_Ns(Uout_delay);
               end loop;

               Papi_Binding.PAPI_Measure_End (Tid);
               Stats_Binding.EnterSample (Papi_Binding.PAPI_Get_Measured (Tid), Tid, exec, run);
            end if;
         end loop;
      end loop;
   end Prod_Cons_F;

   procedure Driver_AdaLockFree (Nr_Exec : Integer; Nr_Iter : Integer; Nr_Pairs : Integer; Sc_Runs : Boolean; Ovw_Stats : Boolean; Uout : Integer) is
      Nr_Runs : Integer;
   begin

      if Sc_Runs then Nr_Runs := Nr_Pairs;
      else Nr_Runs := 1;
      end if;
      Stats_Binding.InitStats (2 * Nr_Pairs, Nr_Exec, Nr_Runs);
      Barrier_Binding.Barrier_Init (2 * Nr_Pairs);
      Timer_Binding.Calibrate;
      Papi_Binding.PAPI_Start;

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
   end Driver_AdaLockFree;
end Driver_AdaLockFree;

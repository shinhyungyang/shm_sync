with Queue_Interfaces;
with Unbounded_Queues;
with Atomic_Intrinsic; use Atomic_Intrinsic;
with Interfaces.C; use Interfaces.C;
with Timer_Binding;

package body ADATAS is
  package My_SQI is new Queue_interfaces(Element_Type => Integer);
  package My_USQ is new Unbounded_Queues(QIs => My_SQI);
  Q: My_USQ.Queue;

  procedure sync_Producer 
    (Nr_iter : in Integer; Uout_delay : in Integer)
  is
  begin
    for I in 1..Nr_iter loop
      while TAS_4(Q.Locked'Access, 1)=1 loop  -- Acquire
        null;
      end loop;

      Q.Enqueue(New_Item => I);

      Lock_Release_4(Q.Locked'Access);  -- Release
      
      Timer_Binding.Useful_Work_Ns(Uout_delay); -- Useful work (nano second)
    end loop;
  end sync_Producer;

  procedure sync_Consumer 
    (Nr_iter : in Integer; Uout_delay : in Integer)
  is
  Data : Integer;
  begin

    for I in 1..Nr_iter loop
      loop
        begin
          Data := I;
          while TAS_4(Q.Locked'Access, 1)=1 loop  -- Acquire
            null;
          end loop;

          Q.Dequeue(Element => Data);

          Lock_Release_4(Q.Locked'Access); -- Release
          exit;
          exception when Constraint_Error =>
          -- if Queue is empty, redo inner loop
          Lock_Release_4(Q.Locked'Access);
        end;
      end loop;
    Timer_Binding.Useful_Work_Ns(Uout_delay); -- Useful work (nano second)
    end loop;
  end sync_Consumer;
end ADATAS;

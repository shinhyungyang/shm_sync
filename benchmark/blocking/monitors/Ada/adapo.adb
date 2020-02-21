with Ada.Containers.Synchronized_Queue_interfaces;
with Ada.Containers.Unbounded_Synchronized_Queues;

package body ADAPO is
  package My_SQI is new Ada.Containers.Synchronized_Queue_interfaces(
    Element_Type => Integer);
  package My_USQ is new
    Ada.Containers.Unbounded_Synchronized_Queues(
      Queue_Interfaces => My_SQI);
    Q: My_USQ.Queue;

  Iterations: constant Positive := 10_000_000;
  No_Users: constant Positive := 14;

  procedure sync_Producer
    (Nr_iter : in Integer)
  is
  begin

    for I in 1..Nr_iter loop
      Q.Enqueue(New_Item => I);
    end loop;

  end sync_Producer;

  procedure sync_Consumer
    (Nr_iter : in Integer)
  is
  Data : Integer;
  begin

    for I in 1..Nr_iter loop
      Data := I;
      Q.Dequeue(Element => Data);
    end loop;

  end sync_Consumer;
end ADAPO;

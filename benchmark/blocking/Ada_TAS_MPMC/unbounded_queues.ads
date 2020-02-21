with Queue_Interfaces;
with Ada.Finalization;
with Ada.Containers;
use Ada.Containers;
with Interfaces.C;  use Interfaces.C;
generic
   with package QIs is
     new Queue_Interfaces (<>);

package Unbounded_Queues is
   pragma Annotate (CodePeer, Skip_Analysis);
   pragma Preelaborate;

   package Implementation is

      --  All identifiers in this unit are implementation defined

      pragma Implementation_Defined;

      type List_Type is tagged limited private;

      procedure Enqueue
        (List     : in out List_Type;
         New_Item : QIs.Element_Type);

      procedure Dequeue
        (List    : in out List_Type;
         Element : out QIs.Element_Type);
  
      function Length (List : List_Type) return Count_Type;

      function Max_Length (List : List_Type) return Count_Type;

   private
      type Node_Type;
      type Node_Access is access Node_Type;

      type Node_Type is limited record
         Element : QIs.Element_Type;
         Next    : Node_Access;
      end record;

      type List_Type is new Ada.Finalization.Limited_Controlled with record
         First, Last : Node_Access;
         Length      : Count_Type := 0;
         Max_Length  : Count_Type := 0;
      end record;


      overriding procedure Finalize (List : in out List_Type);

   end Implementation;

   type List_Type_Ptr is access Implementation.List_Type;

   type Queue
   is new QIs.Queue with record
      List : List_Type_Ptr := new Implementation.List_Type;
      Locked : aliased Unsigned := 0;
   end record;

   overriding procedure Enqueue (Container : in out Queue; New_Item : QIs.Element_Type);

   overriding procedure Dequeue (Container : in out Queue; Element : out QIs.Element_Type);

   overriding function Current_Use (Container : Queue) return Count_Type;

   overriding function Peak_Use  (Container : Queue)return Count_Type;

end Unbounded_Queues;

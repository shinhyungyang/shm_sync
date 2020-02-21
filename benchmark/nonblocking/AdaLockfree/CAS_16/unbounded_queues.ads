with System;
with Queue_Interfaces;
with Ada.Finalization;
with Ada.Containers;
use Ada.Containers;

generic
   with package QIs is
     new Queue_Interfaces (<>);

   Default_Ceiling : System.Any_Priority := System.Priority'Last;

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

      function Dequeue
        (List    : in out List_Type;
         Element : out QIs.Element_Type) return Boolean;

      function Empty (List : List_Type) return Boolean;

      function Length (List : List_Type) return Count_Type;

      function Max_Length (List : List_Type) return Count_Type;

      function print_size (List : in out List_Type) return Integer;

   private
      type Count_Tag is mod 2**64;

      type Node_Type;
      type Node_Access is access Node_Type;

      type Tagged_Node is record
         Ptr : Node_Access := null;
         Counter : Count_Tag := 0;
      end record;
      for Tagged_Node'Alignment use 16;

      type Node_Type is limited record
         Element : QIs.Element_Type;
         Next    : aliased Tagged_Node;
      end record;

      type List_Type is new Ada.Finalization.Limited_Controlled with record
         First, Last : aliased Tagged_Node;
         Length      : Count_Type := 0;
         Max_Length  : Count_Type := 0;
      end record;

      overriding procedure Initialize (List : in out List_Type);
      overriding procedure Finalize (List : in out List_Type);

   end Implementation;

   type List_Type_Ptr is access Implementation.List_Type;

   type Queue
     --(Ceiling : System.Any_Priority := Default_Ceiling)
   --with
     --Priority => Ceiling
   is new QIs.Queue with record
      List : List_Type_Ptr := new Implementation.List_Type;
   end record;

   overriding procedure Enqueue (Container : in out Queue; New_Item : QIs.Element_Type);

   overriding function  Dequeue (Container : in out Queue; Element : out QIs.Element_Type) return Boolean;

   overriding function Current_Use (Container : Queue) return Count_Type;

   overriding function Peak_Use  (Container : Queue)return Count_Type;

   function print_size (Container : Queue) return Integer;

end Unbounded_Queues;

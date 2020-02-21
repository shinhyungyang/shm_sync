with Ada.Containers;
use Ada.Containers;

generic
   type Element_Type is private;

package Queue_Interfaces is
   pragma Pure;

   type Queue is interface;

   procedure Enqueue
     (Container : in out Queue;
      New_Item  : Element_Type) is abstract;

   function  Dequeue
     (Container : in out Queue;
      Element   : out Element_Type) return Boolean is abstract;

   function Current_Use (Container : Queue) return Count_Type is abstract;

   function Peak_Use (Container : Queue) return Count_Type is abstract;

end Queue_Interfaces;

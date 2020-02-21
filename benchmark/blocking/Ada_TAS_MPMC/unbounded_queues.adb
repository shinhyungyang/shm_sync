with Ada.Unchecked_Deallocation;

package body Unbounded_Queues is

   package body Implementation is

      -----------------------
      -- Local Subprograms --
      -----------------------

      procedure Free is
         new Ada.Unchecked_Deallocation (Node_Type, Node_Access);
    
      -------------
      -- Dequeue --
      -------------

      procedure Dequeue
        (List    : in out List_Type;
         Element : out QIs.Element_Type)
      is
         X : Node_Access;
      begin
        Element := List.First.Element;
        X := List.First;
        List.First := List.First.Next;

        if List.First = null then -- if there's only one element
          List.Last := null;
        end if;

        List.Length := List.Length - 1;
        Free (X);
      end Dequeue;

      -------------
      -- Enqueue --
      -------------

      procedure Enqueue
        (List     : in out List_Type;
         New_Item : QIs.Element_Type)
      is
         Node : Node_Access;
      begin

        Node := new Node_Type'(New_Item, null);

        if List.First = null then
          List.First := Node;
          List.Last := List.First;

        else
          List.Last.Next := Node;
          List.Last := Node;
        end if;

        List.Length := List.Length + 1;

        if List.Length > List.Max_Length then
          List.Max_Length := List.Length;
        end if;
     end Enqueue;

      --------------
      -- Finalize --
      --------------

      procedure Finalize (List : in out List_Type) is
         X : Node_Access;

      begin
         while List.First /= null loop
            X := List.First;
            List.First := List.First.Next;
            Free (X);
         end loop;
      end Finalize;

      ------------
      -- Length --
      ------------

      function Length (List : List_Type) return Count_Type is
      begin
         return List.Length;
      end Length;

      ----------------
      -- Max_Length --
      ----------------

      function Max_Length (List : List_Type) return Count_Type is
      begin
         return List.Max_length;
      end Max_Length;

   end Implementation;

   --protected body Queue is

      -----------------
      -- Current_Use --
      -----------------

      function Current_Use (Container : Queue) return Count_Type is
      begin
         return Container.List.Length;
      end Current_Use;

      -------------
      -- Dequeue --
      -------------

      procedure Dequeue (Container : in out Queue; Element : out QIs.Element_Type)
      is
      begin
         if Container.List.Length > 0 then
           Container.List.Dequeue (Element);
         else
           raise Constraint_Error;
         end if;
      end Dequeue;

      -------------
      -- Enqueue --
      -------------

      procedure Enqueue (Container : in out Queue; New_Item : QIs.Element_Type) is
      begin
         Container.List.Enqueue (New_Item);
      end Enqueue;

      --------------
      -- Peak_Use --
      --------------

      function Peak_Use (Container : Queue) return Count_Type is
      begin
         return Container.List.Max_Length;
      end Peak_Use;


   --end Queue;

end Unbounded_Queues;

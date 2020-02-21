with Ada.Unchecked_Deallocation;
with Atomic_Intrinsic;

package body Unbounded_Queues is

   package body Implementation is
      use Atomic_Intrinsic;

      -----------------------
      -- Local Subprograms --
      -----------------------

      procedure Free is
         new Ada.Unchecked_Deallocation (Node_Type, Node_Access);

      -------------
      -- Enqueue --
      -------------

      procedure Enqueue
        (List     : in out List_Type;
         New_Item : QIs.Element_Type)
      is
         Node : Node_Access;
         Last, Last2, Next : aliased Tagged_Node;
         Desired : Tagged_Node;
         Result : Boolean;
      begin
         Node := new Node_Type'(Element => New_Item, Next => Tagged_Node'(null, 0));

         loop
            Atomic_Load(List.Last'Address, Last'Address, Memory_Order(ACQ), 16);
            Atomic_Load(Last.Ptr.Next'Address, Next'Address, Memory_Order(ACQ), 16);

            Atomic_Load(List.Last'Address, Last2'Address, Memory_Order(ACQ), 16);
            if Last = Last2 then
               if Next.Ptr = null then
                  Desired := Tagged_Node'(Node, Next.Counter + 1);
                  if Atomic_Compare_Exchange(Last.Ptr.Next'Address, Next'Address, Desired'Address, 16) then
                     exit;
                  end if;
               else
                  Desired := Tagged_Node'(Next.Ptr, Last.Counter + 1);
                  Result := Atomic_Compare_Exchange(List.Last'Address, Last'Address, Desired'Address, 16);
               end if;
            end if;
         end loop;

         Desired := Tagged_Node'(Node, Last.Counter + 1);
         Result := Atomic_Compare_Exchange(List.Last'Address, Last'Address, Desired'Address, 16);
      end Enqueue;

      -------------
      -- Dequeue --
      -------------

      function Dequeue
        (List    : in out List_Type;
         Element : out QIs.Element_Type) return Boolean
      is
         First, Last, Next, First2 : aliased Tagged_Node;
         Desired : Tagged_Node;
         Result : Boolean;
      begin
         loop
            Atomic_Load(List.First'Address, First'Address, Memory_Order(ACQ), 16);
            Atomic_Load(List.Last'Address, Last'Address, Memory_Order(ACQ), 16);
            Atomic_Load(First.Ptr.Next'Address, Next'Address, Memory_Order(ACQ), 16);

            Atomic_Load(List.First'Address, First2'Address, Memory_Order(ACQ), 16);
            if First = First2 then
               if First.Ptr = Last.Ptr then
                  if Next.Ptr = null then
                     return False;
                  end if;
                  Desired := Tagged_Node'(Next.Ptr, Last.Counter + 1);
                  Result := Atomic_Compare_Exchange(List.Last'Address, Last'Address, Desired'Address, 16);
               else
                  if Next.Ptr /= null then
                     Element := Next.Ptr.Element;
                     Desired := Tagged_Node'(Next.Ptr, First.Counter + 1);
                     if Atomic_Compare_Exchange(List.First'Address, First'Address, Desired'Address, 16) then
                        exit;
                     end if;
                  end if;
               end if;
            end if;
         end loop;
         Free(First.Ptr);
         return True;
      end Dequeue;

      ----------------
      -- Initialize --
      ----------------

      procedure Initialize (List : in out List_Type) is
         Node : Node_Access;

      begin
         Node := new Node_Type;
         List.First.Ptr := Node;
         List.Last.Ptr := Node;
      end Initialize;

      --------------
      -- Finalize --
      --------------

      procedure Finalize (List : in out List_Type) is
         X : Node_Access;

      begin
         while List.First.Ptr /= List.Last.Ptr loop
            X := List.First.Ptr;
            List.First := List.First.Ptr.Next;
            Free (X);
         end loop;
      end Finalize;

      function Empty (List : List_Type) return Boolean is
      begin
         return List.First = List.Last;
      end Empty;

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
         return List.Max_Length;
      end Max_Length;

      function print_size (List : in out List_Type) return Integer is
         First, Last : Tagged_Node;
         C : Integer := 0;
      begin
         First := List.First;
         Last := List.Last;
         while First.Ptr /= Last.Ptr loop
            First := First.Ptr.Next;
            C := C + 1;
         end loop;
         return C;
      end print_size;
   end Implementation;

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

      function Dequeue (Container : in out Queue; Element : out QIs.Element_Type) return Boolean
      is
      begin
         if not Container.List.Empty then
           return Container.List.Dequeue (Element);
         else
           return False;
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

      function print_size (Container : Queue) return Integer is
      begin
         return Container.List.print_size;
      end print_size;
   --end Queue;

end Unbounded_Queues;

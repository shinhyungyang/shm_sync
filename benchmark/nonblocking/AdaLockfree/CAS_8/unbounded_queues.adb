with Ada.Unchecked_Deallocation;
with Atomic_Intrinsic;
use Atomic_Intrinsic;

package body Unbounded_Queues is

   package body Implementation is
      use Tagged_Pointer;

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
         Node := new Node_Type'(Element => New_Item, Next => null);

         loop
            Atomic_Load(List.Last'Address, Last'Address, Memory_Order(SC), 8);
            
            Atomic_Load(Ptr_To_NA(Get_Ptr(Last)).Next'Address, Next'Address, Memory_Order(SC), 8);

            Atomic_Load(List.Last'Address, Last2'Address, Memory_Order(SC), 8);
            if Last = Last2 then
               if Get_Ptr(Next) = 0 then
                  Desired := Create_Tagged_Ptr(NA_To_Ptr(Node), Get_Count(Next) + 1);
                  if Atomic_Compare_Exchange(Ptr_To_NA(Get_Ptr(Last)).Next'Address, Next'Address, Desired'Address, 8) then
                     exit;
                  end if;
               else
                  Desired := Create_Tagged_Ptr(Get_Ptr(Next), Get_Count(Last) + 1);
                  Result := Atomic_Compare_Exchange(List.Last'Address, Last'Address, Desired'Address, 8);
               end if;
            end if;
         end loop;

         Desired := Create_Tagged_Ptr(NA_To_Ptr(Node), Get_Count(Last) + 1);
         Result := Atomic_Compare_Exchange(List.Last'Address, Last'Address, Desired'Address, 8);
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
         X : Node_Access;
         Result : Boolean;
      begin
         loop
            Atomic_Load(List.First'Address, First'Address, Memory_Order(SC), 8);

            Atomic_Load(List.Last'Address, Last'Address, Memory_Order(SC), 8);

            Atomic_Load(Ptr_To_NA(Get_Ptr(First)).Next'Address, Next'Address, Memory_Order(SC), 8);

            Atomic_Load(List.First'Address, First2'Address, Memory_Order(SC), 8);
            if First = First2 then
               if Get_Ptr(First) = Get_Ptr(Last) then
                  if Get_Ptr(Next) = 0 then
                     return False;
                  end if;
                  Desired := Create_Tagged_Ptr(Get_Ptr(Next), Get_Count(Last) + 1);
                  Result := Atomic_Compare_Exchange(List.Last'Address, Last'Address, Desired'Address, 8);
               else
                  if Get_Ptr(Next) /= 0 then
                     Element := Ptr_To_NA(Get_Ptr(Next)).Element;
                     Desired := Create_Tagged_Ptr(Get_Ptr(Next), Get_Count(First) + 1);
                     if Atomic_Compare_Exchange(List.First'Address, First'Address, Desired'Address, 8) then
                        exit;
                     end if;
                  end if;
               end if;
            end if;
         end loop;
         X := Ptr_To_NA(Get_Ptr(First));
         Free(X);
         return True;
      end Dequeue;

      ----------------
      -- Initialize --
      ----------------

      procedure Initialize (List : in out List_Type) is
         Node : Node_Access;

      begin
         Node := new Node_Type;
         Set_Ptr (Node.Next'Access, Ptr(0));
         Set_Ptr (List.First'Access, NA_To_Ptr(Node));
         Set_Ptr (List.Last'Access, NA_To_Ptr(Node));
      end Initialize;

      --------------
      -- Finalize --
      --------------

      procedure Finalize (List : in out List_Type) is
         X : Node_Access;

      begin
         while Get_Ptr(List.First) /= Get_Ptr(List.Last) loop
            X := Ptr_To_NA(Get_Ptr(List.First));
            List.First := Ptr_To_NA(Get_Ptr(List.First)).Next;
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
         while Get_Ptr(First) /= Get_Ptr(Last) loop
            First := Ptr_To_NA(Get_Ptr(First)).Next;
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

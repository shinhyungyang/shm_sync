generic
   type Tagged_Ptr is private;
package Tagged_Pointers is 
   pragma Preelaborate;
   pragma Elaborate_Body; --Needed for CMake

   type Ptr is mod 2**64;
   type Count is mod 2**64;

   -- Create a tagged pointer with a pointer 'p' and a counter 'c'
   function Create_Tagged_Ptr (p : Ptr; c : Count) return Tagged_Ptr;
   pragma Import (C, Create_Tagged_Ptr, "create_tagged_ptr");

   -- Set the pointer value in a tagged pointer
   procedure Set_Ptr (tp : access Tagged_Ptr; p : Ptr);
   pragma Import (C, Set_Ptr, "set_ptr");

   -- Get the pointer value in a tagged pointer
   function Get_Ptr (p : Tagged_Ptr) return Ptr;
   pragma Import (C, Get_Ptr, "get_ptr");

   -- Get the counter in a tagged pointer
   function Get_Count (p : Tagged_Ptr) return Count;
   pragma Import (C, Get_Count, "get_count");

end Tagged_Pointers;

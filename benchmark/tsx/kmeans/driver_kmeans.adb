with GNAT.Command_Line; use GNAT.Command_Line;
with Ada.Text_IO; use Ada.Text_IO;
with Ada.Float_Text_IO;
with Types; use Types;
with Execute; use Execute;
with CPU_Affinity; use CPU_Affinity;
with PAPI_Binding; use PAPI_Binding;
with CMD_Args; use CMD_Args;

procedure Driver_Kmeans is

   --File inputs
   nObj, nAttr : Integer;
   Attributes : Float_Db_Arr_Access;

   --Result
   Best_nClusters : Integer;
   Cluster_Centers : Float_Db_Arr_Access;
   Cluster_Assign : int_arr_access;

begin
   if not Read_Args (KM) then
      return;
   end if;

   -- Read input file
   declare 
      File : File_Type;
      Filename : String := SB.To_String (Input_File);
      Buf_Float : Float;
   begin
      -- Count Objects
      nObj := 0;
      Open (File, In_File, Filename);
      while not End_OF_File (File) loop
         declare
            Buf_Str : String := Get_Line (File);
         begin
            nObj := nObj + 1;
         end;
      end loop;
      Close (File);

      -- Count Attributes
      nAttr := -1;
      Open (File, In_File, Filename);
      while not End_OF_Line (File) loop
         Ada.Float_Text_IO.Get (File, Buf_Float);
         nAttr := nAttr + 1;
      end loop;
      Close (File);

      -- Read Attributes
      Attributes := new Float_Db_Arr (1 .. nObj, 1 .. nAttr);
      Open (File, In_File, Filename);
      for i in 1 .. nObj loop
         Ada.Float_Text_IO.Get (File, Attributes(i, 1)); -- read and ignore.
         for j in 1 .. nAttr loop
            Ada.Float_Text_IO.Get (File, Attributes(i, j));
         end loop;
      end loop;
      Close (File);
   end;

   -- Start K-means clustering
   Cluster_Assign := new Int_Arr(1 .. nObj);

   for N_Task in Min_Task .. Max_Task loop
      Put_Line (Integer'image (N_Task) & " tasks.");
      PAPI_Start;
      PAPI_measure_start(0, True);
      Cluster_Exec (N_Task, nObj, nAttr, Attributes, False,
                     Min_nClusters, Max_nClusters, Threshold,
                     Best_nClusters, Cluster_Centers, Cluster_Assign);
      PAPI_measure_end(0, True);
      PAPI_Finish;
   end loop;
--   New_Line;

   Free_Float_Db_Arr (Attributes);
   Free_Float_Db_Arr (Cluster_Centers);
   Free_Int_Arr (Cluster_Assign);
end Driver_Kmeans;

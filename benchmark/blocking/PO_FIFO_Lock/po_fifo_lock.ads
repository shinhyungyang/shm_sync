package PO_FIFO_Lock is
   protected type FIFO_Lock is
     entry Lock;
     procedure Unlock;
   private
     Guard : Boolean := True;
   end FIFO_Lock;
end PO_FIFO_Lock;

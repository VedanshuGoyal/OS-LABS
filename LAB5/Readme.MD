Explanation:
Implemented TLB as an LRU.
Page Table is an array of M size in which each index specifies the Physical Address of that Virtual Address(index).
Free Frame List also implemented as an LRU.


Assumptions:
Also make a reverse mapping array from Frame Physical Address -> Frame valid bit address
because if there is a page fault & there is no free frame we have to invert the valid bit of Least Recently Use Frame in the Page Table.
And also not updating FrameList LRU structure when there is TLB hit because of fast accessing of data otherwise the advantage of TLB vanishes.



To run the program give the following arguments...
demand_paging <# of process> <virtual address space> <physical adress space> <size of TLB>
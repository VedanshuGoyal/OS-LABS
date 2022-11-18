In this we implemented client-server communication through TCP socket programming.

And then to see interactions
(for different versions just change Server => Server1, Server2, Server3 )...
 	./Server 5555

And in another Terminal
	./Client 127.0.0.1 5555

Then just enter commands in the Client window and you will get required ans.

If you want multiple Clients open another terminals and execute the same command
	./Client 127.0.0.1 5555


Observations:

About Server1:
-> It is the most secure server because it is 1 server - 1 client communication.
-> Server uses all its resources for the first come process(FIFO).
-> So, it is also the least memory usage relative to other servers.
-> convoy effect.

About Server2:
-> It is the most resource intensive Server (due to it makes all the variable, data copy for all the processes). 
-> But it is the best method in terms of response time (where multi-client takes place).

About Server3:
-> It is not that resource intensive than Server2.
-> Bad Turnaround time(because it takes time to find different sockets).
In this we implemented client-server based Que-Ans game using message queue.

To execute =>
Firstly compile both the files server.c and client.c by =>
	gcc server.c -o server
	gcc client.c -o client

And then to see the interactions open two terminal windows
In one of the terminal run the server and in the another run the client by=>

	// (in First Window) // 
	./server 

	// (in Second Window) //
	./client

As you run the client file, a line appears in the server window =>
How many client(players) you wanted? => In this type how many players are there(as mentioned in assignment).

Now Round 0 will start in both terminal.
and you will ask in server terminal what Que you want to give to all the other client.
Ex. you can type =>
What is your name?

Now in client window you can see the changes like Client 0, give the ans...
Now Client 0, will give his ans for that...
Ex. => Vedanshu

Then Client 1, Then Client 2.... Till Client k-1

And you will see the required changes in the server window also

After completion of a Round all the scores will be shown in the server terminal as well as the current round 
winners(if there is a tie).

And again the whole same process runs give the ques then all the client give the ans...

As any of the client reaches the MAX_SCORE(50) the game will stops.
And in the server terminal you will see the game winners.
And client terminal will also close(because of QUIT signal).


// possible bugs
if admin or clients give input when program are not asking from them (We are not sure but this can possibly can give not logical ans).
This is a very basic implemented shell in c.

To compile this c file you mandatory have to do it in linux pc.

If we want to run the shell in batch mode we have to insert this command after compilation
./shell commands.txt

1.Simple shell is running properly and it is accepting input command and executing it properly.

2.Foreground and Background process
All the processes are running

3.Internal and External command
According to the assignment these internal commands are working:
clear: clear the screen
env: displays all environment parameters
cd <dir>
pwd: prints the current directory
mkdir <dir>: creates a directory called "dir"
rmdir <dir>: removes the directory called "dir"
ls: lists files in current directory ( ls –l: option also need to be supported )
history: displays the last commands the user ran, with an offset next to each command. Last commands can be stored in a file and may be displayed to user when the command is issued.
history <argument>: displays the given number of commands as specified
exit: exits the shell

These Extenal commands are working:
cat filename
cal
date

4. We have done some basic Exception Handling also, like if you give him a wrong command then it will tell you it can't executable.

5.Mode of Implementation
The shell is running in both the mode. We checked all the commands and it is giving correct output.
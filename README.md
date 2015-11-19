Memdisplay
==========

Simple fun app to display the contents of an application's memory in real time on a pixel display. Each pixel is a bit (white = 1, black = 0), lowest memory addresses are on the top left, increasing to the right then the next row.

Library dependencies are: SDL2. The Makefile places the binary in bin/[name of folder Makefile is in], e.g. bin/memdisplay

Root is usually required (reads /proc/{pid}/mem). Example usages are:

./memdisplay $(pgrep -x ssh)

./memdisplay $(pgrep -x tint2) heap start

Full usage is:

./memdisplay (PID) (memtype) (start|end)

where PID is a process folder in /proc, memtype is a word in brackets in the last column of /proc/(PID)/maps (e.g. "stack" for the stack, "heap" for heap), and either "start" or "end" to either display the start of the range at the top left, or end of the range at the bottom right ("stack" is usually at the end, "heap" is usually at the start). Memtype and start/end are optional, and default to "stack" and "end".

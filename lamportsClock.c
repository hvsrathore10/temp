/*Write a program to simulate logical clock synchronization using Lamport’s logical clock
algorithm and vector clocks.*/
#include <stdio.h>
#include <stdlib.h>
int max(int a, int b)
{
return (a > b) ? a : b;
}
void lamport_clock(int p1_events[], int p2_events[], int n)
{
int p1_clock = 0, p2_clock = 0;
printf("Event\tProcess 1\tProcess 2\n");
for (int i = 0; i < n; i++)
{
p1_clock++;
p2_clock++;
if (p1_events[i] != -1 && p2_events[i] != -1)
{
// Communication between processes
p1_clock = max(p1_clock, p2_clock) + 1;
p2_clock = max(p1_clock, p2_clock) + 1;
}
printf("E%d\t%d\t\t%d\n", i + 1, p1_clock, p2_clock);
}
}
int main()
{
int p1_events[] = {1, 0,
-1, 1, 0}; // -1 means no communication
int p2_events[] = {0, 1,
-1, 1, 0};
int n = sizeof(p1_events) / sizeof(p1_events[0]);
lamport_clock(p1_events, p2_events, n);
return 0;
}
#include <stdio.h>
#include <stdlib.h>
int lamport_clock = 0;
void send_request(int process_id)
{
    lamport_clock++;
    printf("Process %d sends request at time %d\n", process_id, lamport_clock);
}
void receive_request(int process_id, int sender_id, int sender_time)
{
    lamport_clock = (sender_time > lamport_clock) ? sender_time + 1 : lamport_clock + 1;
    printf("Process %d receives request from Process %d at time %d\n",
           process_id, sender_id, lamport_clock);
}
void enter_critical_section(int process_id)
{
    printf("Process %d entering critical section at time %d\n", process_id,
           lamport_clock);
}
void exit_critical_section(int process_id)
{
    printf("Process %d exiting critical section at time %d\n", process_id,
           lamport_clock);
}
int main()
{
    // Simulating mutual exclusion between two processes
    int process_1 = 1;
    int process_2 = 2;
    send_request(process_1);
    receive_request(process_2, process_1, lamport_clock);
    enter_critical_section(process_1);
    exit_critical_section(process_1);
    send_request(process_2);
    receive_request(process_1, process_2, lamport_clock);
    enter_critical_section(process_2);
    exit_critical_section(process_2);
    return 0;
}
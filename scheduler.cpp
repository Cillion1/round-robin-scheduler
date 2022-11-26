#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm> 

using namespace std;

struct myProcess {
    int processNumber;
    int arrival;
    int burst;
    float wait = 0;
    char state = ' ';
};

vector<myProcess> processes;
vector<myProcess> process;
vector<myProcess> readyQueue;
vector<myProcess> cpu;
vector<myProcess> done;

int jobs_remaining = 0;

bool compare (myProcess lhs, myProcess rhs) {
    return lhs.burst < rhs.burst;
}

void printResults(int time) {
    if (time < 10) {
        printf("  %d", time);
    } else {
        printf(" %d", time);
    }
    for (int i = 0; i < processes.size(); i++) {
        if (!processes.empty()) {
            for (int j = 0; j < done.size(); j++) {
                if (!done.empty() && processes[i].processNumber == done[j].processNumber) {
                    processes[i].state = ' ';
                    break;
                }
            }

            for (int j = 0; j < readyQueue.size(); j++) {
                if (!readyQueue.empty() && processes[i].processNumber == readyQueue[j].processNumber) {
                    processes[i].state = '.';
                    processes[i].wait++;
                    break;
                }
            }

            if (!cpu.empty() && processes[i].processNumber == cpu[0].processNumber) {
                processes[i].state = '#';
            }
        }
        printf("  %c", processes[i].state);
    }
    printf("\n");
}

void srt() {
    int curr_time = 0;
    while(1) {
        if (jobs_remaining == 0) {
            break;
        }
        // if process in cpu is done 
        if (!cpu.empty() && cpu[0].burst == 0) {
            // mark process done
            // set CPU idle
            done.push_back(cpu[0]);
            cpu.erase(cpu.begin());
            jobs_remaining--;
            continue;
        }
        // if new process is arriving
        if (!process.empty() && process[0].arrival == curr_time) {
            // add new process to RQ
            readyQueue.push_back(process[0]);
            std::sort(readyQueue.begin(), readyQueue.end(), compare); 
            process.erase(process.begin());
            continue;
        }
        // if cpu is idle and RQ not empty
        if (cpu.empty() && !readyQueue.empty()) {
            // move process from RQ to CPU
            cpu.push_back(readyQueue[0]);
            readyQueue.erase(readyQueue.begin());
            continue;
        }

        // execute one burst of job on CPU
        if (!cpu.empty()) {
            cpu[0].burst--;
        }
        printResults(curr_time);
        curr_time++;
    }
}

void rr(int timeSlice) {
    int counter = 0;
    int curr_time = 0;
    while(1) {
        if (jobs_remaining == 0) {
            break;
        }
        // if process in cpu is done 
        if (!cpu.empty() && cpu[0].burst == 0) {
            // mark process done
            // set CPU idle
            done.push_back(cpu[0]);
            cpu.erase(cpu.begin());
            jobs_remaining--;
            counter = 0;
            continue;
        }
        // if process exceeds timeslice
        if (counter == timeSlice) {
            readyQueue.push_back(cpu[0]);
            cpu.erase(cpu.begin());
            counter = 0;
            continue;
        }
        // if new process is arriving
        if (!process.empty() && process[0].arrival == curr_time) {
            // add new process to RQ
            //printf("Adding process %d into ready queue\n\n", process[0].processNumber);
            readyQueue.push_back(process[0]);
            process.erase(process.begin());
            continue;
        }
        // if cpu is idle and RQ not empty
        if (cpu.empty() && !readyQueue.empty()) {
            // move process from RQ to CPU
           // printf("Moving a process from RQ to CPU\n\n");
            cpu.push_back(readyQueue[0]);
            readyQueue.erase(readyQueue.begin());
            continue;
        }

        // execute one burst of job on CPU
        if (!cpu.empty()) {
            cpu[0].burst--;
            counter++;
        }
        printResults(curr_time);
        curr_time++;
    }
}

int main(int argc, char ** argv) {

    float count = 0;
    int slice = 0;

    // Read in the config file name from the commanda-line arguments
    if (argc < 3 || argc > 4) {
        std::cout << "Usage: <Config file> <Scheduler> <Time Slice for RR>\n";
        exit(-1);
    } 

    std::string str(argv[2]);

    if (str.compare("SJF") != 0) {
        if (str.compare("RR") == 0 && argc != 4) {
            std::cout << "Usage: <Config file> <Scheduler> <Time Slice for RR>\n";
            exit(-1);
        } else if (str.compare("RR") == 0) {
            slice = atoi(argv[3]);
        } else {
            std::cout << "Usage: <Config file> <Scheduler> <Time Slice for RR>\n";
            exit(-1);
        }
    }

    std::ifstream file(argv[1]);
    if (file.is_open()) {
        int a, b;
        while (file >> a >> b) {
            processes.push_back ({jobs_remaining, a, b});
            process.push_back ({jobs_remaining, a, b});
            jobs_remaining++;
            count++;
        }
        file.close();
    }

    printf("Time ");
    for (int i = 0; i < process.size(); i++) {
        printf("P%d ", process[i].processNumber);
    }
    printf("\n------------------------------------------------------------\n");
    if (str.compare("SJF") == 0) {
        srt();
    } else {
        rr(slice);
    }
    printf("------------------------------------------------------------\n");

    int totalWait = 0;
    for (int i = 0; i < processes.size(); i++) {
        printf("P%d waited %.3f sec.\n", i, processes[i].wait);
        totalWait = totalWait + processes[i].wait;
    }
    printf("Average waiting time = %.3f.\n", (totalWait / count));
    
    return 0;
}
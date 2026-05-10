#include <fstream>
#include <iostream>
#include <ostream>
#include <cmath>

using namespace std;

struct NRKState {
    int timestamp_ms;
    int fl_ticks;
    int fr_ticks;
    int bl_ticks;
    int br_ticks;
};

struct NRKPos {
    int timestamp_ms;
    float x;
    float y;
    float theta;
};

const int ticks_per_revolution = 1024;
const float wheel_radius_m = 0.3;
const float wheelbase_m = 1.0;

void readFile(char* fileName, NRKState* states, int& count) {
    ifstream file(fileName);

    if (!file) {
        cerr << "No file";
        return;
    }

    count = -1;

    do {
        states[++count] = NRKState {};
    }
    while (file >> states[count].timestamp_ms >> states[count].fl_ticks >> states[count].fr_ticks >> states[count].bl_ticks >> states[count].br_ticks);
}

NRKPos calculatePos(const NRKState& curr, const NRKState& prev, const NRKPos& prevPos) {
        int d_fl = curr.fl_ticks - prev.fl_ticks;
        int d_fr = curr.fr_ticks - prev.fr_ticks;
        int d_bl = curr.bl_ticks - prev.bl_ticks;
        int d_br = curr.br_ticks - prev.br_ticks;

        float d_left  = (static_cast<float>(d_fl + d_bl)) / 2;
        float d_right = (static_cast<float>(d_fr + d_br)) / 2;

        float distance_per_tick = 2 * M_PI * wheel_radius_m / ticks_per_revolution;
        float dL = d_left  * distance_per_tick;
        float dR = d_right * distance_per_tick;

        float d = (dL + dR) / 2;              // пройдена вiдстань центру
        float dtheta = (dR - dL) / wheelbase_m;    // змiна орiєнтацiї

        return NRKPos {
            .timestamp_ms = curr.timestamp_ms,
            .x = prevPos.x + d * cos(prevPos.theta + dtheta / 2),
            .y = prevPos.y + d * sin(prevPos.theta + dtheta / 2),
            .theta = prevPos.theta + dtheta
        };
}

void printPosition(const NRKPos& pos) {
    cout << pos.timestamp_ms << " " << pos.x << " " << pos.y << " " << pos.theta << endl; 
}

void printPositions(NRKPos* pos, int count) {
    for (int i = 0; i < count; i++) {
        printPosition(pos[i]);
    }
}

void calculate(char* fileName) {
    ifstream file(fileName);

    if (!file) {
        cerr << "No file";
        return;
    }

    int count = 0;

    NRKState prev;
    NRKState state;
    NRKPos prevPos = NRKPos {.x = 0, .y = 0, .theta = 0, .timestamp_ms = 0};
    printPosition(prevPos);

    while (file >> state.timestamp_ms >> state.fl_ticks >> state.fr_ticks >> state.bl_ticks >> state.br_ticks) {
        
        if (count > 0) {
            prevPos = calculatePos(state, prev, prevPos);
            printPosition(prevPos);
        }

        prev.bl_ticks = state.bl_ticks;
        prev.br_ticks = state.br_ticks;
        prev.fl_ticks = state.fl_ticks;
        prev.fr_ticks = state.fr_ticks;
        prev.timestamp_ms = state.timestamp_ms;

        count++;
    }
}

void printStates(NRKState* states, int count) {
    cout << "States: " << endl;
    for (int i = 0; i < count; i++) {
        cout << states[i].timestamp_ms << " ";
        cout << states[i].fl_ticks << " ";
        cout << states[i].fr_ticks << " ";
        cout << states[i].bl_ticks << " ";
        cout << states[i].br_ticks << endl;
    }
    cout << "====" << endl;
}

int main(int argc, char** argv) {
    // The program expects exactly one argument: a path to telemetry samples.
    if (argc != 2) {
        std::cerr << "usage: ugv_odometry <input_path>\n";
        return 1;
    }

    char* filePath = argv[1];
    NRKState* states = new NRKState[100];
    int count = 0;

    readFile(filePath, states, count);
    calculate(filePath);
    return 0;
}


// Deprecated array solution

void calculatePositions(NRKState* states, NRKPos* positions, int count) {
    positions[0] = NRKPos {
        .timestamp_ms = states[0].timestamp_ms,
        .x = 0,
        .y = 0,
        .theta = 0
    };

    for (int i = 1; i < count; i++) {
        NRKState curr = states[i];
        NRKState prev = states[i - 1];

        positions[i] = calculatePos(curr, prev, positions[i - 1]);
    }
}

int main2(int argc, char** argv) {
    // The program expects exactly one argument: a path to telemetry samples.
    if (argc != 2) {
        std::cerr << "usage: ugv_odometry <input_path>\n";
        return 1;
    }

    char* filePath = argv[1];
    NRKState* states = new NRKState[100];
    int count = 0;

    readFile(filePath, states, count);
    // printStates(states, count);

    NRKPos* positions = new NRKPos[100];
    calculatePositions(states, positions, count);
    printPositions(positions, count);

    return 0;
}

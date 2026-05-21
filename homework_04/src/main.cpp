#include <fstream>
#include <iostream>
#include <ostream>
#include <cmath>
#include <sstream>

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

void addPosition(const NRKPos& pos, stringstream& result) {
    result << pos.timestamp_ms << " " << pos.x << " " << pos.y << " " << pos.theta << endl; 
}

int calculate(char* fileName) {
    ifstream file(fileName);

    if (!file) {
        cerr << "No file\n";
        return 1;
    }

    int count = 0;

    NRKState prev;
    NRKState state;
    NRKPos prevPos = NRKPos { .timestamp_ms = 0, .x = 0, .y = 0, .theta = 0 };
    string line;
    stringstream lineStream;
    stringstream result;
    
    while (getline(file, line)) {
        string extra;
        lineStream.clear(); 
        lineStream.str(line);
        if (!(lineStream >> state.timestamp_ms >> state.fl_ticks >> state.fr_ticks >> state.bl_ticks >> state.br_ticks && !(lineStream >> extra))) {
            cerr << "Bad input data in file\n";
            return 2;
        }
        if (count > 0) {
            prevPos = calculatePos(state, prev, prevPos);
            addPosition(prevPos, result);
        }

        prev.bl_ticks = state.bl_ticks;
        prev.br_ticks = state.br_ticks;
        prev.fl_ticks = state.fl_ticks;
        prev.fr_ticks = state.fr_ticks;
        prev.timestamp_ms = state.timestamp_ms;

        count++;
    }

    cout << result.str();

    return 0;
}

int main(int argc, char** argv) {
    // The program expects exactly one argument: a path to telemetry samples.
    if (argc != 2) {
        std::cerr << "usage: ugv_odometry <input_path>\n";
        return 1;
    }

    char* filePath = argv[1];
    return calculate(filePath);
}

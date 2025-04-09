#include<iostream>
#include<string>


enum STATE {
    STANDING=0,
    DRIVING
};

class ImuObj {

    public:
        std::string timestamp;
        int id;
        int yaw;
        int heading;
        STATE st;
        double acceleration;
};

STATE stringToState(const std::string& state) {
    if (state == "STANDING" || state == "0") return STATE::STANDING;
    if (state == "DRIVING" || state == "1") return STATE::DRIVING;
    return STATE::STANDING;
}


void printImuObject(const ImuObj& obj) {
    std::cout << "Timestamp: " << obj.timestamp << "\n"
              << "ID: " << obj.id << "\n"
              << "Yaw: " << obj.yaw << "\n"
              << "Heading: " << obj.heading << "\n"
              << "State: " << (obj.st == STATE::STANDING ? "STANDING" : "DRIVING") << "\n"
              << "Acceleration: " << obj.acceleration << "\n"
              << "------------------------\n";
}
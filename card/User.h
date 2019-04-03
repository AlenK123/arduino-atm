#pragma once

namespace user {
    class user {
    private:
        const char * name;
        const char * rfid;
    public:
        user() { }
        user(char * _name, char * _rfid) : name(_name), rfid(_rfid) { }
        const char * get_name() {
            return this->name;
        }
        const char * get_rfid() {
            return this->rfid;
        }
    };
};

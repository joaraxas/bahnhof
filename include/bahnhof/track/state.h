#pragma once

typedef int trackid;
typedef int nodeid;
typedef int switchid;
typedef int signalid;

struct State
{
    State();
    State(trackid trackstart, float nodediststart, bool alignedwithtrack);
    operator bool() const {return track>0;};
    trackid track;
    float nodedist;
    bool alignedwithtrack;
};

State flipstate(State state);


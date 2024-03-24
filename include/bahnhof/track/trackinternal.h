#pragma once
#include<map>
#include "state.h"
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/sprite.h"

class Rendering;
class Train;

class Tracksystem;
class Node;
class Track;
class Switch;
class Signal;

class Node
{
friend class Switch;
public:
    Node(Tracksystem& newtracksystem, Vec posstart, float dirstart, nodeid myid);
    void connecttrack(trackid track, bool fromupordown);
    void render(Rendering* r);
    Vec getpos();
    float getdir();
    bool hasswitch();
    trackid trackup = 0;
    trackid trackdown = 0;
    Train* reservedfor = nullptr;
private:
    Tracksystem* tracksystem;
    std::unique_ptr<Switch> switchup = nullptr;
    std::unique_ptr<Switch> switchdown = nullptr;
    nodeid id;
    Vec pos;
    float dir;
};

class Switch
{
friend class Node;
public:
    Switch(Node* node, trackid track, bool updown);
    void render(Rendering* r);
    void setswitch(int newstate);
    Vec pos();
    int getstate();
private:
    void addtrack(trackid track);
    Tracksystem* tracksystem;
    Node* node;
    bool updown;
    switchid id;
    int switchstate = 0;
    std::vector<trackid> tracks;
    Sprite sprite;
};

class Track
{
public:
    Track(Tracksystem& newtracksystem, nodeid previous, nodeid next, trackid myid);
    void render(Rendering* r);
    Vec getpos(float nodedist);
    Vec getpos(float nodedist, float transverseoffset);
    State getcloseststate(Vec pos);
    float getarclength(float nodedist);
    float getorientation(float nodedist);
    bool isabovepreviousnode();
    bool isbelownextnode();
    void addsignal(State signalstate, signalid signal);
    signalid nextsignal(State state, bool startfromtrackend=false, bool mustalign=true);
    float radius;
    nodeid previousnode, nextnode;
private:
    Tracksystem* tracksystem;
    trackid id;
    float phi;
    float previousdir;
    float nextdir;
    Vec previouspos;
    Vec nextpos;
    std::map<float,signalid> signals;
};

struct Trackblock
{
    std::vector<nodeid> switchblocks;
    std::vector<signalid> signalblocks;
};

class Signal
{
public:
    Signal(Tracksystem& newtracksystem, State signalstate, signalid myid);
    void render(Rendering* r);
    void update();
    void set(int redgreenorflip);
    int getcolorforordergeneration();
    Vec getpos();
    bool isred(Train* train);
    State state;
    Train* reservedfor = nullptr;
private:
    Vec pos;
    signalid id;
    Tracksystem* tracksystem;
    bool isgreen = true;
    Trackblock blocks;
    Sprite sprite;
};

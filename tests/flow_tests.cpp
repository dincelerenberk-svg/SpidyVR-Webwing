#include "spidy/traversal_flow.hpp"
#include <iostream>
#include <limits>
using namespace spidy;
int main() {
    int checks = 0;
    const auto check = [&](bool ok) { ++checks; if (!ok) throw std::runtime_error("Flow assertion failed"); };
    try {
        TraversalFlowInput in;
        in.focused = in.airborne = in.allowed = true;
        in.tracked = {true, true};
        in.hands = {Vec3{-.2f,-.2f,0}, Vec3{.2f,-.2f,0}};
        in.look = {0,-1,0};
        TraversalFlow flow;
        Vec3 v{0,-10,-20};
        flow.step(.01f, in, v, 65);
        check(v.y == -10); // Opt-in only.
        TraversalFlowConfig config; config.enabled = true; flow.configure(config);
        flow.step(.01f, in, v, 65);
        check(v.y < -10);
        in.hands = {Vec3{-.7f,-.2f,0}, Vec3{.7f,-.2f,0}};
        v = {0,-10,-20}; flow.step(.01f,in,v,65); check(v.y == -10);
        in.move = {1,0,0}; const float before = length(v);
        flow.step(.01f,in,v,65); check(std::abs(length(v)-before) < .001f && v.x > 0);
        in.move = {}; in.attached[0] = true; flow.step(.01f,in,v,65); check(flow.combo() == 0);
        in.attached[0] = false;
        for (int i=0;i<60;++i) flow.step(.01f,in,v,65);
        in.attached[1] = true; flow.step(.01f,in,v,65); check(flow.combo() == 1);
        const float boosted = length(v);
        for (int i=0;i<100;++i) flow.step(.01f,in,v,65);
        check(std::abs(length(v)-boosted) < .001f); // Holding does not farm boosts.
        in.attached[1] = false; flow.step(.01f,in,v,65);
        in.attached[0] = true; flow.step(.01f,in,v,65); check(flow.combo() == 2);
        for (int i=0;i<410;++i) flow.step(.01f,in,v,65); check(flow.combo() == 0);
        flow.reset(); in.attached = {}; v={0,-10,-20};
        in.attached[0]=true; flow.step(.01f,in,v,65);
        in.attached={false,true}; flow.step(.01f,in,v,65); check(flow.combo()==0); // Rapid trigger spam.
        in.focused=false; flow.step(.01f,in,v,65); check(flow.combo()==0 && length(v)<65);
        in.focused=true; in.tracked[0]=false; const auto saved=v;
        flow.step(.01f,in,v,65); check(length(v-saved)<.001f);
        in.tracked[0]=true; v={0,-100,-100}; in.attached={};
        flow.step(.01f,in,v,65); check(length(v)<=65.001f);
        v={0,-10,-20}; flow.step(.2f,in,v,65); check(v.y==-10);
        v={0,5,-10}; const auto wallSpeed=length(v); flow.wallLaunch(v,65);
        check(length(v)>wallSpeed && length(v)<=65);
        flow.wallLaunch(v,10); check(length(v)<=10.001f);
        config.turnRate=std::numeric_limits<float>::quiet_NaN(); bool rejected=false;
        try {flow.configure(config);} catch(const std::invalid_argument&) {rejected=true;}
        check(rejected);
        std::cout << "PASS " << checks << " traversal flow checks\n";
    } catch(const std::exception& e) {std::cerr << e.what() << '\n'; return 1;}
}

#include "spidy/glider.hpp"
#include "spidy/swing.hpp"
#include <functional>
#include <iostream>
#include <limits>
#include <string>

using namespace spidy;
namespace {
int total{}, failed{};
void check(bool ok, const char* message) { if (!ok) throw std::runtime_error(message); }
void near(float a, float b, float epsilon = .005f) { check(std::abs(a - b) <= epsilon, "Values differ"); }
void test(const char* name, const std::function<void()>& run) {
    ++total;
    try { run(); std::cout << "PASS " << name << '\n'; }
    catch (const std::exception& e) { ++failed; std::cerr << "FAIL " << name << ": " << e.what() << '\n'; }
}
GliderInput pose() {
    GliderInput in;
    in.hands = {Vec3{-.65f, -.2f, 0}, Vec3{.65f, -.2f, 0}};
    in.tracked = {true, true};
    in.look = {0, 0, -1};
    in.focused = in.airborne = true;
    return in;
}
Glider open() { Glider g; GliderConfig c; c.enabled = true; g.configure(c); return g; }
Vec3 fly(Glider& g, GliderInput in, int steps = 120, float dt = 1.f / 120) {
    Vec3 v{0, -12, -20};
    for (int i = 0; i < steps; ++i) { v.y -= 9.81f * dt; g.step(dt, in, v, 9.81f, 65); }
    return v;
}
struct EmptyWorld : World {
    std::optional<RayHit> raycast(Vec3, Vec3, float) const override { return {}; }
    bool exists(std::uint64_t) const override { return true; }
    MoveResult sweep(Vec3, Vec3 to, float radius) const override {
        if (to.y < radius) return {{to.x, radius, to.z}, {0, 1, 0}, true};
        return {to, {}, false};
    }
};
Input swingPose() {
    Input in; in.look = {0, 0, -1};
    const auto sample = pose();
    for (int i = 0; i < 2; ++i) {
        in.hands[i].tracked = true;
        in.hands[i].gripRelativeToHead = sample.hands[i];
    }
    return in;
}
void enable(Swing& s) { GliderConfig c; c.enabled = true; s.configureGlider(c); }
}
int main() {
    test("default off does not change velocity", [] {
        Glider g; Vec3 v{3, -12, -20}, before = v;
        g.step(.01f, pose(), v, 9.81f, 65);
        near(length(v - before), 0); check(!g.active(), "Default deployed");
    });
    test("deployment requires a deliberate held pose", [] {
        auto g = open(); fly(g, pose(), 18); check(!g.active(), "Opened too early");
        fly(g, pose(), 12); check(g.active(), "Did not deploy");
    });
    test("steady descent preserves forward speed and does not climb", [] {
        auto g = open(); const auto v = fly(g, pose(), 600);
        near(v.y, -g.config().sinkSpeed); near(length(Vec3{v.x, 0, v.z}), 20);
    });
    test("recovery from a dive is bounded and smooth", [] {
        auto g = open(); fly(g, pose()); Vec3 v{0, -50, -20};
        g.step(.01f, pose(), v, 9.81f, 65);
        check(v.y > -50 && v.y < -49.7f, "Instant recovery or no lift");
    });
    test("pose hysteresis allows small hand drift", [] {
        auto g = open(); fly(g, pose()); auto in = pose();
        in.hands[0].x = -.46f; in.hands[1].x = .46f;
        Vec3 v{0, -5, -20}; g.step(.01f, in, v, 9.81f, 65);
        check(g.active(), "Small drift folded wings");
        in.hands[0].x = -.35f; in.hands[1].x = .35f;
        g.step(.01f, in, v, 9.81f, 65); check(!g.active(), "Folded pose stayed active");
    });
    test("every interruption cancels immediately and requires a fresh hold", [] {
        for (int cause = 0; cause < 12; ++cause) {
            auto g = open(); fly(g, pose()); auto in = pose(); Vec3 v{0, -8, -20};
            switch (cause) {
            case 0: in.focused = false; break;
            case 1: in.airborne = false; break;
            case 2: in.blocked = true; break;
            case 3: in.tracked[0] = false; break;
            case 4: in.tracked[1] = false; break;
            case 5: in.grips[0] = .5f; break;
            case 6: in.triggers[1] = .5f; break;
            case 7: in.hands[0].y = -1; break;
            case 8: in.hands[1].z = 1; break;
            case 9: v = {0, -8, -1}; break;
            case 10: v.y = 5; break;
            case 11: in.hands[0].y = -.6f; in.hands[1].y = .1f; break;
            }
            g.step(.01f, in, v, 9.81f, 65); check(!g.active(), "Interruption kept wings open");
            v = {0, -8, -20}; g.step(.01f, pose(), v, 9.81f, 65);
            check(!g.active(), "Interruption skipped deployment hold");
        }
    });
    test("tracking spikes and invalid times never corrupt velocity", [] {
        const float nan = std::numeric_limits<float>::quiet_NaN();
        for (float dt : {0.f, -.01f, .2f, nan}) {
            auto g = open(); fly(g, pose()); Vec3 v{0, -8, -20};
            g.step(dt, pose(), v, 9.81f, 65);
            check(!g.active() && finite(v), "Invalid delta kept state"); near(v.y, -8);
        }
        auto g = open(); auto in = pose(); in.hands[0].x = nan;
        Vec3 v{0, -8, -20}; g.step(.01f, in, v, 9.81f, 65); check(!g.active(), "NaN pose accepted");
    });
    test("steering turns toward gaze without increasing horizontal speed", [] {
        auto g = open(); fly(g, pose()); auto in = pose(); in.look = {1, 0, 0};
        Vec3 v{0, -5, -20}; g.step(.05f, in, v, 9.81f, 65);
        check(v.x > 0 && v.x < 1.5f, "Steering went wrong way or snapped");
        near(length(Vec3{v.x, 0, v.z}), 20);
        in.look = {0, 1, 0}; g.step(.01f, in, v, 9.81f, 65); check(finite(v), "Vertical gaze failed");
    });
    test("speed limit and no-gravity modes stay bounded", [] {
        auto g = open(); fly(g, pose()); Vec3 v{0, -30, -50};
        g.step(.01f, pose(), v, 0, 25); check(length(v) <= 25.001f, "Exceeded speed limit");
    });
    test("different simulation rates converge to the same descent", [] {
        for (float rate : {60.f, 90.f, 120.f, 144.f}) {
            auto g = open(); const auto v = fly(g, pose(), static_cast<int>(5 * rate), 1.f / rate);
            near(v.y, -g.config().sinkSpeed); near(v.z, -20);
        }
    });
    test("invalid configurations are rejected", [] {
        for (int fault = 0; fault < 6; ++fault) {
            GliderConfig c;
            switch (fault) {
            case 0: c.sinkSpeed = 0; break;
            case 1: c.foldSpan = c.deploySpan; break;
            case 2: c.liftAcceleration = 31; break;
            case 3: c.turnResponse = -1; break;
            case 4: c.openSeconds = std::numeric_limits<float>::infinity(); break;
            case 5: c.deploySeconds = -1; break;
            }
            bool rejected = false;
            try { Glider g; g.configure(c); } catch (const std::invalid_argument&) { rejected = true; }
            check(rejected, "Bad configuration accepted");
        }
    });
    test("solver glides and still lands through collision sweeps", [] {
        EmptyWorld w; Swing s; enable(s); s.reset({{0, 20, 0}, {0, -8, -20}, false});
        const auto in = swingPose();
        for (int i = 0; i < 120; ++i) s.update(1.f / 120, in, w);
        check(s.gliding(), "Solver did not glide");
        for (int i = 0; i < 1400 && !s.body().grounded; ++i) s.update(1.f / 120, in, w);
        check(s.body().grounded && !s.gliding(), "Glider bypassed ground or stayed open");
        check(s.body().position.y >= s.config().radius - .001f, "Below collision floor");
    });
    test("native prediction preserves authoritative position and suspension", [] {
        EmptyWorld w; Swing s; enable(s); auto in = swingPose(); Body actual{{0, 50, 0}, {0, -12, -20}, false};
        for (int i = 0; i < 120; ++i) {
            auto out = s.predictNativeStep(1.f / 120, in, w, actual);
            check(out.valid, "Invalid native prediction");
            near(length(s.body().position - actual.position), 0);
            actual = {out.target, out.velocity, false};
        }
        check(s.gliding(), "Native glide never deployed");
        s.allowGliderFlight(false);
        s.predictNativeStep(.01f, in, w, actual); check(!s.gliding(), "Suspension ignored");
        s.allowGliderFlight(true); s.reset(actual); check(!s.gliding(), "Reset kept glide state");
    });
    test("disabled solver matches baseline exactly", [] {
        EmptyWorld w; Swing a, b; GliderConfig c; b.configureGlider(c);
        Body initial{{0, 50, 0}, {2, -4, -20}, false}; a.reset(initial); b.reset(initial);
        for (int i = 0; i < 100; ++i) { a.update(.01f, swingPose(), w); b.update(.01f, swingPose(), w); }
        near(length(a.body().position - b.body().position), 0, 0);
        near(length(a.body().velocity - b.body().velocity), 0, 0);
    });
    std::cout << total - failed << '/' << total << " glider tests passed\n";
    return failed ? 1 : 0;
}

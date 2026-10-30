#pragma once
#include "math.hpp"
#include <stdexcept>

namespace spidy {
// Optional web-wing locomotion. No game memory, camera rotation or collision bypass.
struct GliderConfig {
    bool enabled = false;
    float sinkSpeed = 3.5f, liftAcceleration = 14.f, turnResponse = 1.4f;
    float minimumSpeed = 4.f, deploySpan = 1.05f, foldSpan = .8f;
    float deploySeconds = .2f, openSeconds = .3f;
};
struct GliderInput {
    std::array<Vec3, 2> hands{}; // tracking metres, relative to the head
    std::array<bool, 2> tracked{};
    std::array<float, 2> grips{}, triggers{};
    Vec3 look{}, move{}; // world-space steering; +Y up
    bool focused{}, airborne{}, blocked{};
};
class Glider {
  public:
    void configure(GliderConfig config) {
        const float positive[] = {config.sinkSpeed, config.liftAcceleration, config.minimumSpeed,
                                  config.deploySpan, config.foldSpan, config.deploySeconds, config.openSeconds};
        for (float value : positive)
            if (!std::isfinite(value) || value <= 0)
                throw std::invalid_argument("Invalid glider configuration");
        if (!std::isfinite(config.turnResponse) || config.turnResponse < 0 || config.turnResponse > 5 ||
            config.sinkSpeed > 20 || config.liftAcceleration > 30 || config.minimumSpeed > 30 ||
            config.deploySpan > 3 || config.foldSpan >= config.deploySpan || config.deploySeconds > 2 ||
            config.openSeconds > 2)
            throw std::invalid_argument("Glider configuration out of range");
        config_ = config;
        reset();
    }
    void reset() { held_ = openness_ = 0; active_ = false; }
    bool active() const { return active_; }
    float openness() const { return openness_; }
    const GliderConfig& config() const { return config_; }

    void step(float seconds, const GliderInput& in, Vec3& velocity, float gravity, float maxSpeed) {
        const auto cancel = [&] { reset(); };
        if (!config_.enabled || !std::isfinite(seconds) || seconds <= 0 || seconds > .1f ||
            !in.focused || !in.airborne || in.blocked || !finite(velocity) ||
            !std::isfinite(gravity) || gravity < 0 || !std::isfinite(maxSpeed) || maxSpeed <= 0) {
            cancel();
            return;
        }
        for (int i = 0; i < 2; ++i) {
            if (!in.tracked[i] || !finite(in.hands[i]) || !std::isfinite(in.grips[i]) ||
                !std::isfinite(in.triggers[i]) || in.grips[i] < 0 || in.triggers[i] < 0 ||
                in.grips[i] > .15f || in.triggers[i] > .15f ||
                in.hands[i].y < -.7f || in.hands[i].y > .25f || std::abs(in.hands[i].z) > .7f) {
                cancel();
                return;
            }
        }
        const float span = in.hands[1].x - in.hands[0].x;
        const Vec3 horizontal{velocity.x, 0, velocity.z};
        const float speed = length(horizontal);
        if (span < (active_ ? config_.foldSpan : config_.deploySpan) ||
            in.hands[0].x > -.2f || in.hands[1].x < .2f ||
            std::abs(in.hands[0].y - in.hands[1].y) > .5f ||
            !std::isfinite(speed) || speed < config_.minimumSpeed || velocity.y > 0) {
            cancel();
            return;
        }
        held_ += seconds;
        if (held_ + 1e-6f < config_.deploySeconds)
            return;
        active_ = true;
        openness_ = std::min(1.f, openness_ + seconds / config_.openSeconds);
        // Add only the lift required to approach the sink rate. Account for this
        // step's gravity; never turn a fall into a climb, or create forward speed.
        const float needed = std::max(0.f, -config_.sinkSpeed - velocity.y);
        velocity.y += std::min(needed, (gravity + config_.liftAcceleration * openness_) * seconds);
        Vec3 direction = horizontal / speed;
        Vec3 target = finite(in.look) ? normalized(Vec3{in.look.x, 0, in.look.z}) : Vec3{};
        if (length(target) < .5f)
            target = direction;
        const Vec3 stick = finite(in.move) ? limited(Vec3{in.move.x, 0, in.move.z}, 1) : Vec3{};
        if (length(stick) > .15f)
            target = normalized(target + stick * .65f);
        // Turn on the horizontal plane by a bounded angle, preserving momentum.
        const float angle = std::atan2(cross(direction, target).y, dot(direction, target));
        const float turn = std::clamp(angle, -config_.turnResponse * seconds, config_.turnResponse * seconds);
        direction = Quat::yaw(turn).rotate(direction);
        velocity.x = direction.x * speed;
        velocity.z = direction.z * speed;
        velocity = limited(velocity, maxSpeed);
    }

  private:
    GliderConfig config_{};
    float held_{}, openness_{};
    bool active_{};
};
} // namespace spidy

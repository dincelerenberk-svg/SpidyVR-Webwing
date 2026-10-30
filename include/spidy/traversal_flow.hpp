#pragma once
#include "math.hpp"
#include <array>
#include <stdexcept>

namespace spidy {
struct TraversalFlowConfig {
    bool enabled = false;
    float diveAcceleration = 5.f, turnRate = .6f, transferBoost = .6f;
};
struct TraversalFlowInput {
    bool focused{}, airborne{}, allowed{}, blocked{};
    std::array<bool, 2> tracked{}, attached{};
    std::array<Vec3, 2> hands{};
    Vec3 look{}, move{};
};
// An optional bounded supplement to the existing collision-owned swing solver.
// Transfers require a different hand, time airborne and distance integrated from the solver velocity.
class TraversalFlow {
  public:
    void configure(TraversalFlowConfig c) {
        if (!std::isfinite(c.diveAcceleration) || c.diveAcceleration < 0 || c.diveAcceleration > 12 ||
            !std::isfinite(c.turnRate) || c.turnRate < 0 || c.turnRate > 2 ||
            !std::isfinite(c.transferBoost) || c.transferBoost < 0 || c.transferBoost > 2)
            throw std::invalid_argument("Invalid traversal flow configuration");
        config_ = c;
        reset();
    }
    void reset() { age_ = distance_ = comboAge_ = 0; combo_ = 0; lastHand_ = -1; attached_ = {}; }
    unsigned combo() const { return combo_; }
    void wallLaunch(Vec3& velocity, float maxSpeed) const {
        if (config_.enabled && finite(velocity) && std::isfinite(maxSpeed) && maxSpeed > 0 &&
            length(velocity) > 4)
            velocity = limited(velocity + normalized(velocity) * (config_.transferBoost * 2), maxSpeed);
    }
    void step(float dt, const TraversalFlowInput& in, Vec3& velocity, float maxSpeed) {
        if (!config_.enabled || !std::isfinite(dt) || dt <= 0 || dt > .1f ||
            !std::isfinite(maxSpeed) || maxSpeed <= 0 || !finite(velocity) ||
            !in.focused || !in.airborne || !in.allowed || in.blocked ||
            !in.tracked[0] || !in.tracked[1] || !finite(in.hands[0]) || !finite(in.hands[1])) {
            reset();
            return;
        }
        age_ += dt;
        comboAge_ += dt;
        distance_ += length(velocity) * dt;
        if (comboAge_ > 4) combo_ = 0;
        for (int hand = 0; hand < 2; ++hand) {
            if (in.attached[hand] && !attached_[hand]) {
                // The initial attachment seeds a chain, but never grants energy.
                if (lastHand_ >= 0 && hand != lastHand_ && age_ >= .45f && distance_ >= 6 &&
                    length(velocity) >= 6) {
                    combo_ = std::min(combo_ + 1, 5u);
                    velocity = limited(velocity + normalized(velocity) *
                        (config_.transferBoost * static_cast<float>(combo_)), maxSpeed);
                    comboAge_ = 0;
                }
                lastHand_ = hand;
                age_ = distance_ = 0;
            }
        }
        attached_ = in.attached;
        if (in.attached[0] || in.attached[1]) return;
        // Tuck both hands toward the chest and look down to deliberately dive.
        const float span = in.hands[1].x - in.hands[0].x;
        const bool tucked = span >= 0 && span < .6f &&
            length(in.hands[0]) < .8f && length(in.hands[1]) < .8f;
        if (tucked && finite(in.look) && in.look.y < -.65f && velocity.y < -3)
            velocity.y -= config_.diveAcceleration * dt;
        // Stick steering turns existing horizontal momentum; it adds no speed.
        Vec3 horizontal{velocity.x, 0, velocity.z};
        const float speed = length(horizontal);
        if (speed > 4 && finite(in.move) && length(Vec3{in.move.x, 0, in.move.z}) > .2f) {
            Vec3 direction = horizontal / speed;
            Vec3 target = normalized(Vec3{in.move.x, 0, in.move.z});
            const float angle = std::atan2(cross(direction, target).y, dot(direction, target));
            direction = Quat::yaw(std::clamp(angle, -config_.turnRate * dt, config_.turnRate * dt)).rotate(direction);
            velocity.x = direction.x * speed;
            velocity.z = direction.z * speed;
        }
        velocity = limited(velocity, maxSpeed);
    }
  private:
    TraversalFlowConfig config_{};
    std::array<bool, 2> attached_{};
    float age_{}, distance_{}, comboAge_{};
    unsigned combo_{};
    int lastHand_ = -1;
};
} // namespace spidy

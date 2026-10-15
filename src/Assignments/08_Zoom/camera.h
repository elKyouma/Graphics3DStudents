#pragma once
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace {
    inline float logistic(float y) {
        return 1.0f / (1.0f + std::exp(-y));
    }

    inline float logit(float x) {
        return std::log(x / (1.0f - x));
    }
}

class Camera {
public:

    void look_at(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
        V_ = glm::lookAt(eye, center, up);
    }

    void perspective(float fov, float aspect, float near, float far) {
        fov_ = fov;
        aspect_ = aspect;
        near_ = near;
        far_ = far;
    }

    void set_aspect(float aspect) {
        aspect_ = aspect;
    }

    glm::mat4 view() const { return V_; }

    glm::mat4 projection() const { return glm::perspective(fov_, aspect_, near_, far_); }
	
    void zoom(float y_offset) {
        auto x = fov_ / glm::pi<float>();
        auto y = logit(x);
        y += y_offset;
        x = glm::clamp(logistic(y), 0.001f, 0.999f);
        fov_ = x * glm::pi<float>();
		//printf("Zoom: fov = %f\n", fov_);
	}


private:
    float fov_;
    float aspect_;
    float near_;
    float far_;

    glm::mat4 V_;
};
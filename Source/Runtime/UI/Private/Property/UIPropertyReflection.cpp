#include "Property/UIPropertyReflection.h"

namespace VGE {
namespace UI {

// Built-in type registration
namespace {
    struct TypeRegistrar {
        TypeRegistrar() {
            // Register fundamental types
            REGISTER_TYPE(bool);
            REGISTER_TYPE(int);
            REGISTER_TYPE(float);
            REGISTER_TYPE(double);
            REGISTER_TYPE(std::string);
            
            // Register vector types
            REGISTER_TYPE(glm::vec2);
            REGISTER_TYPE(glm::vec3);
            REGISTER_TYPE(glm::vec4);
            
            // Register matrix types
            REGISTER_TYPE(glm::mat2);
            REGISTER_TYPE(glm::mat3);
            REGISTER_TYPE(glm::mat4);
            
            // Register quaternion type
            REGISTER_TYPE(glm::quat);
        }
    };
    
    static TypeRegistrar registrar;
}
}
} 
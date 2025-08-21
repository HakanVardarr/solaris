#include "Graphics/Vulkan/Vertex.hpp"
#include <cstddef>
#include <vulkan/vulkan_enums.hpp>

namespace Solaris::Graphics::Vulkan {

vk::VertexInputBindingDescription Vertex::getBindingDescription() {
    vk::VertexInputBindingDescription bindingDescription{};
    bindingDescription.setBinding(0);
    bindingDescription.setStride(sizeof(Vertex));
    bindingDescription.setInputRate(vk::VertexInputRate::eVertex);

    return bindingDescription;
}

std::array<vk::VertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() {
    std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

    attributeDescriptions[0].setBinding(0);
    attributeDescriptions[0].setLocation(0);
    attributeDescriptions[0].setFormat(vk::Format::eR32G32Sfloat);
    attributeDescriptions[0].setOffset(offsetof(Vertex, pos));

    attributeDescriptions[1].setBinding(0);
    attributeDescriptions[1].setLocation(1);
    attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
    attributeDescriptions[1].setOffset(offsetof(Vertex, color));

    return attributeDescriptions;
}

}  // namespace Solaris::Graphics::Vulkan
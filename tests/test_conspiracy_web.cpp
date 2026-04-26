#include "../src/systems/ConspiracyWeb.h"
#include <iostream>
#include <cassert>

using namespace systems;

int main() {
    ConspiracyWeb web;

    std::cout << "Testing ConspiracyWeb..." << std::endl;

    assert(web.getTotalNodes() == 0);
    assert(web.getDiscoveredCount() == 0);
    assert(web.getDiscoveryRatio() == 0.0f);

    const ConspiracyNode* node = web.getNode("test_node");
    assert(node == nullptr);

    web.discoverNode("test_node");
    assert(!web.isDiscovered("test_node"));

    std::cout << "All ConspiracyWeb tests passed!" << std::endl;
    return 0;
}
//
// Created by david on 20/12/2019.
//

#include "ComputeNode.h"

void ComputeNode::execute() {
    func(inputs, outputs);
}

ComputeNode::ComputeNode() {

}

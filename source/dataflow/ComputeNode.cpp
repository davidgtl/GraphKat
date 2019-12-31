//
// Created by david on 20/12/2019.
//

#include "ComputeNode.h"

void ComputeNode::execute() {
    func(inputs, outputs);
}

ComputeNode::ComputeNode() {

}

ComputeNode::ComputeNode(Context *in_ctx, Context *out_ctx, void (*func)(Context *, Context *)) {
    this->inputs = in_ctx;
    this->outputs = out_ctx;
    this->func = func;
}

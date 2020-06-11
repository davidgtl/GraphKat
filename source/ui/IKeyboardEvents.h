//
// Created by david on 11/06/2020.
//

#pragma once

class IKeyboardEvents {
    virtual void on_button(int button, int action, int mods) {}
};
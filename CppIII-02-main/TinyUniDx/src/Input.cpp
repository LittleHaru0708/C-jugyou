#include "Input.h"

#include <memory>
#include <Keyboard.h>

namespace UniDx{

std::unique_ptr<DirectX::Keyboard> Input::keyboard;
DirectX::Keyboard::State Input::nowKeyState;
DirectX::Keyboard::State Input::prevKeyState;

}

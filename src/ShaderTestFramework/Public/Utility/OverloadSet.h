#pragma once

template<class... Ts> 
struct OverloadSet : Ts... 
{ 
    using Ts::operator()...; 
};
#pragma once

#include "Hazel/Core/Events/Event.h"
#include "Hazel/Core/Timestep.h"

namespace Hazel {

    class Layer
    {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();
        virtual void OnAttach() {} // 应用添加此层时执行
        virtual void OnDetach() {} // 应用分离此层时执行
        virtual void OnUpdate(Timestep ts) {} // 更新层, 由应用层每帧调用
        virtual void OnEvent(Event& event) {}// 每层处理事件
        virtual void OnImGuiRender() {}      // 每层渲染imgui
        inline const std::string& GetName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;
    };

}

#pragma once

#include "Hazel/Core/Events/Event.h"

namespace Hazel {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();
		virtual void OnAttach() {} // 应用添加此层时执行
		virtual void OnDetach() {} // 应用分离此层时执行
		virtual void OnUpdate() {} // 更新层, 由应用层每帧调用
		virtual void OnEvent(Event& event) {}// 每层处理事件
		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}

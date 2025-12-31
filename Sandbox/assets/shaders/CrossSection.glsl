// 三维剖面着色器
// 使用 Clip Plane 方法实现剖切效果

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform mat4 u_Model;

// 剖切平面 (Ax + By + Cz + D = 0)
uniform vec4 u_ClipPlane;

out vec3 v_WorldPos;
out vec3 v_Normal;
out vec2 v_TexCoord;
out float v_ClipDistance;

void main()
{
    // 计算世界坐标
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_WorldPos = worldPos.xyz;
    
    // 计算法线（世界空间）
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    v_TexCoord = a_TexCoord;
    
    // 计算到剖切平面的距离
    v_ClipDistance = dot(worldPos, u_ClipPlane);
    
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 FragColor;

in vec3 v_WorldPos;
in vec3 v_Normal;
in vec2 v_TexCoord;
in float v_ClipDistance;

uniform vec3 u_Color;
uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;
uniform bool u_EnableClipping;
uniform bool u_ShowCrossSection;
uniform vec3 u_CrossSectionColor;

void main()
{
    // 剖切测试：如果在剖切平面的负侧，丢弃像素
    if (u_EnableClipping && v_ClipDistance < 0.0) {
        discard;
    }
    
    // 简单的 Phong 光照
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(u_LightPos - v_WorldPos);
    vec3 viewDir = normalize(u_ViewPos - v_WorldPos);
    
    // 环境光
    vec3 ambient = 0.3 * u_Color;
    
    // 漫反射
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_Color;
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.5 * spec * vec3(1.0);
    
    vec3 finalColor = ambient + diffuse + specular;
    
    // 如果启用了剖切面显示，且在剖切面附近，显示特殊颜色
    if (u_ShowCrossSection && abs(v_ClipDistance) < 0.02) {
        finalColor = u_CrossSectionColor;
    }
    
    FragColor = vec4(finalColor, 1.0);
}

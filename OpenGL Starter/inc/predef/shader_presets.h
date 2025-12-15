#pragma once

namespace ShaderPresets
{
	namespace DebugInvalid
	{
		const char* vertexSource = R"(
			#version 330 core

			uniform mat4 model;
			uniform mat4 viewProjection;
			
			layout (location = 0) in vec3 aPos;
			
			void main()
			{
				gl_Position = viewProjection * model * vec4(aPos, 1.0);
			}
		)";

		const char* fragmentSource = R"(
			#version 330 core

			out vec4 FragColor;
			
			void main()
			{
				FragColor = vec4(1, 0, 1, 1);
			}
		)";
	}

	namespace DebugBaseColor
	{
		const char* vertexSource = R"(
			#version 330 core

			uniform mat4 model;
			uniform mat4 viewProjection;
			
			layout (location = 0) in vec3 aPos;
			
			void main()
			{
				gl_Position = viewProjection * model * vec4(aPos, 1.0);
			}
		)";

		const char* fragmentSource = R"(
			#version 330 core

			uniform vec4 color = vec4(0, 1, 0, 1);

			out vec4 FragColor;
			
			void main()
			{
				FragColor = color;
			}
		)";
	}

	namespace DebugUv
	{
		const char* vertexSource = R"(
			#version 330 core

			uniform mat4 model;
			uniform mat4 viewProjection;
			
			layout (location = 0) in vec3 iPos;
			layout (location = 1) in vec2 iUv;

			out vec2 Uv;
			
			void main()
			{
				gl_Position = viewProjection * model * vec4(iPos, 1.0);
				Uv = iUv;
			}
		)";

		const char* fragmentSource = R"(
			#version 330 core

			in vec2 Uv;

			out vec4 FragColor;
			
			void main()
			{
				FragColor = vec4(vec3(length(Uv)), 1);
			}
		)";
	}

	namespace DebugNormals
	{
		const char* vertexSource = R"(
			#version 330 core

			uniform mat4 model;
			uniform mat4 viewProjection;
			uniform bool worldSpace = false;

			layout (location = 0) in vec3 iPos;
			layout (location = 2) in vec3 iNorm;

			out vec3 Norm;
			
			void main()
			{
				gl_Position = viewProjection * model * vec4(iPos, 1.0);

				if (worldSpace) {
					mat3 normalMatrix = transpose(inverse(mat3(model)));
					Norm = normalize(normalMatrix * iNorm);
				}
				else {
					Norm = normalize(iNorm);
				}
			}
		)";

		const char* fragmentSource = R"(
			#version 330 core

			in vec3 Norm;

			out vec4 FragColor;

			void main()
			{
				FragColor = vec4(Norm * 0.5 + 0.5, 1.0);
			}
		)";
	}
}